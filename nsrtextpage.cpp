#include "nsrtextpage.h"
#include "nsrregulararearect.h"
#include "nsrtextselection.h"
#include "nsrtinytextentity.h"
#include "nsrsearchpoint.h"
#include "nsrwordwithcharacters.h"
#include "nsrregiontext.h"
#include "nsrtextxycut.h"

#include <cstring>

/* Text comparison functions */

static bool
caseInsensitiveCmpFn (const QStringRef& from, const QStringRef& to)
{
	return from.compare (to, Qt::CaseInsensitive) == 0;
}

static bool
caseSensitiveCmpFn (const QStringRef& from, const QStringRef& to)
{
	return from.compare (to, Qt::CaseSensitive) == 0;
}

static int
stringLengthAdaptedWithHyphen (const QString&					str,
			       const NSRTinyTextEntityList::ConstIterator&	it,
			       const NSRTinyTextEntityList::ConstIterator&	textListEnd)
{
	int len = str.length ();

	/* Hyphenated '-' must be at the end of a word, so hyphenation means
	 * we have a '-' just followed by a '\n' character, check if the
	 * string contains a '-' character if the '-' is the last entry
	 */
	if (str.endsWith ('-')) {
		/* Validity chek of it + 1 */
		if ((it + 1) != textListEnd) {
			/* 1. If the next character is '\n' */
			const QString &lookahedStr = (*(it+1))->getText ();

			if (lookahedStr.startsWith ('\n'))
				len -= 1;
			else {
				/* 2. If the next word is in a different line or not */
				const NSRNormalizedRect& hyphenArea    = (*it)->getArea ();
				const NSRNormalizedRect& lookaheadArea = (*(it + 1))->getArea ();

				/* Lookahead to check whether both the '-' rect and next
				 * character rectangle overlap
				 */
				if (!NSRTextXYCut::doesConsumeY (hyphenArea, lookaheadArea, 70))
					len -= 1;
			}
		}
	}
	/* Else if it is the second last entry - for example in PDF format */
	else if (str.endsWith ("-\n"))
		len -= 2;

	return len;
}

NSRTextPage::NSRTextPage (const QSize&					size,
			  NSRAbstractDocument::NSRDocumentRotation	rotation,
			  NSRAbstractDocument::NSRDocumentRotation	orientation) :
	_size (size),
	_rotation (rotation),
	_orientation (orientation)
{
}

NSRTextPage::NSRTextPage (const QSize&					size,
			  NSRAbstractDocument::NSRDocumentRotation	rotation,
			  NSRAbstractDocument::NSRDocumentRotation	orientation,
			  const NSRTextEntityList&			words) :
	_size (size),
	_rotation (rotation),
	_orientation (orientation)
{
	NSRTextEntityList::ConstIterator it    = words.constBegin ();
	NSRTextEntityList::ConstIterator itEnd = words.constEnd ();

	for (; it != itEnd; ++it) {
		NSRTextEntity *e = *it;

		if (!e->getText().isEmpty ())
			_words.append (new NSRTinyTextEntity (e->getText (), *e->getArea ()));
	}
}

NSRTextPage::~NSRTextPage ()
{
	qDeleteAll (_searchPoints);
	qDeleteAll (_words);
}

void
NSRTextPage::append (const QString& text, const NSRNormalizedRect& area)
{
	if (!text.isEmpty ())
		_words.append (new NSRTinyTextEntity (text.normalized (QString::NormalizationForm_KC), area));
}

NSRRegularAreaRect *
NSRTextPage::findText (int			searchID,
		       const QString&		query,
		       SearchDirection		direct,
		       Qt::CaseSensitivity	caseSensitivity)
{
	SearchDirection dir = direct;

	/* Invalid search request */
	if ( _words.isEmpty () || query.isEmpty ())
		return NULL;

	NSRTinyTextEntityList::ConstIterator start;
	NSRTinyTextEntityList::ConstIterator end;

	int startOffset = 0;

	const QMap< int, NSRSearchPoint * >::const_iterator sIt = _searchPoints.constFind (searchID);
	if (sIt == _searchPoints.constEnd ()) {
		/* If no previous run of this search is found, then set it to start
		 * from the beginning (respecting the search direction)
		 */
		if (dir == NextResult)
			dir = FromTop;
		else if (dir == PreviousResult)
			dir = FromBottom;
	}

	bool forward = true;
	switch (dir) {
	case FromTop:
		start		= _words.constBegin ();
		startOffset	= 0;
		end		= _words.constEnd ();
		break;
	case FromBottom:
		start		= _words.constEnd ();
		startOffset	= 0;
		end		= _words.constBegin ();
		forward		= false;
		break;
	case NextResult:
		start		= (*sIt)->getEnd ();
		startOffset	= (*sIt)->getOffsetEnd ();
		end		= _words.constEnd ();
		break;
	case PreviousResult:
		start		= (*sIt)->getBegin ();
		startOffset	= (*sIt)->getOffsetBegin ();
		end		= _words.constBegin ();
		forward		= false;
		break;
	};

	NSRRegularAreaRect* ret = NULL;
	const NSRTextComparisonFunction cmpFn = (caseSensitivity == Qt::CaseSensitive)
			? caseSensitiveCmpFn : caseInsensitiveCmpFn;
	if (forward)
		ret = findTextInternalForward (searchID, query, cmpFn, start, startOffset, end);
	else
		ret = findTextInternalBackward (searchID, query, cmpFn, start, startOffset, end);

	return ret;
}

QString
NSRTextPage::text (const NSRRegularAreaRect *area) const
{
    return text (area, AnyPixelTextAreaInclusionBehaviour);
}

QString
NSRTextPage::text (const NSRRegularAreaRect *area, TextAreaInclusionBehaviour b) const
{
	if (area && area->isNull ())
		return QString ();

	NSRTinyTextEntityList::ConstIterator it    = _words.constBegin ();
	NSRTinyTextEntityList::ConstIterator itEnd = _words.constEnd ();

	QString ret;

	if (area) {
		for ( ; it != itEnd; ++it ) {
			if (b == AnyPixelTextAreaInclusionBehaviour) {
				if (area->intersects ((*it)->getArea ()))
					ret += (*it)->getText ();
			} else {
				NSRNormalizedPoint center = (*it)->getArea().center ();

				if (area->contains (center.getX (), center.getY ()))
					ret += (*it)->getText ();
			}
		}
	} else {
		for (; it != itEnd; ++it)
			ret += (*it)->getText ();
	}

	return ret;
}

NSRTextEntityList
NSRTextPage::words (const NSRRegularAreaRect *area, TextAreaInclusionBehaviour b) const
{
	if (area && area->isNull ())
		return NSRTextEntityList ();

	NSRTextEntityList ret;

	if (area) {
		foreach (NSRTinyTextEntity *te, _words) {
			if (b == AnyPixelTextAreaInclusionBehaviour) {
				if (area->intersects (te->getArea ()))
					ret.append (new NSRTextEntity (te->getText (),
								       new NSRNormalizedRect (te->getArea ())));
			} else {
				const NSRNormalizedPoint center = te->getArea().center ();

				if (area->contains (center.getX (), center.getY ()))
					ret.append (new NSRTextEntity (te->getText (),
								       new NSRNormalizedRect (te->getArea ())));
			}
		}
	} else {
		foreach (NSRTinyTextEntity *te, _words)
			ret.append (new NSRTextEntity (te->getText (),
						       new NSRNormalizedRect (te->getArea ())));

	}

	return ret;
}

NSRRegularAreaRect *
NSRTextPage::wordAt (const NSRNormalizedPoint& p, QString *word) const
{
	NSRTinyTextEntityList::ConstIterator itBegin = _words.constBegin ();
	NSRTinyTextEntityList::ConstIterator itEnd   = _words.constEnd ();
	NSRTinyTextEntityList::ConstIterator it      = itBegin;
	NSRTinyTextEntityList::ConstIterator posIt   = itEnd;

	for (; it != itEnd; ++it) {
		if ((*it)->getArea().contains (p.getX (), p.getY ())) {
			posIt = it;
			break;
		}
	}

	QString text;

	if (posIt != itEnd) {
		if ((*posIt)->getText().simplified().isEmpty ())
			return NULL;

		/* Find the first NSRTinyTextEntity of the word */
		while (posIt != itBegin) {
			--posIt;
			const QString itText = (*posIt)->getText ();

			if (itText.right(1).at(0).isSpace ()) {
				if (itText.endsWith ("-\n")) {
					/* Is an hyphenated word,
					 * continue searching the start of the word back
					 */
					continue;
				}

				if (itText == "\n" && posIt != itBegin) {
					--posIt;

					if ((*posIt)->getText().endsWith ("-")) {
						/* Is an hyphenated word,
						 * continue searching the start of the word back
						 */
						continue;
					}

					++posIt;
				}

				++posIt;
				break;
			}
		}

		NSRRegularAreaRect *ret = new NSRRegularAreaRect ();

		for (; posIt != itEnd; ++posIt) {
			const QString itText = (*posIt)->getText ();

			if ( itText.simplified().isEmpty ())
				break;

			ret->appendShape ((*posIt)->getArea ());
			text += (*posIt)->getText ();

			if (itText.right(1).at(0).isSpace ()) {
				if (!text.endsWith ("-\n"))
					break;
			}
		}

		if (word)
			*word = text;

		return ret;
	} else
		return NULL;
}

NSRRegularAreaRect *
NSRTextPage::textArea (const NSRTextSelection& sel) const
{
	if (_words.isEmpty ())
		return new NSRRegularAreaRect ();

	/*
	 * It works like this.
	 * There are two cursors, we need to select all the text between them.
	 * The coordinates are normalised, leftTop is (0,0), rightBottom is (1,1),
	 * so for cursors start (sx, sy) and end (ex, ey) we start with finding text
	 * rectangles under those points, if not we search for the first that is to
	 * the right to it in the same baseline, if none found, then we search for
	 * the first rectangle with a baseline under the cursor, having two points
	 * that are the best rectangles to both of the cursors: (rx, ry) x (tx, ty)
	 * for start and (ux, uy) x (vx, vy) for end, we do a:
	 * 1. (rx, ry) x (1,  ty)
	 * 2. (0,  ty) x (1,  uy)
	 * 3. (0,  uy) x (vx, vy)
	 *
	 * To find the closest rectangle to cursor (cx, cy) we search for a rectangle
	 * that either contains the cursor or that has a left border >= cx and
	 * bottom border >= cy.
	 */
	NSRRegularAreaRect * ret = new NSRRegularAreaRect ();
	const QTransform matrix  = getTransformMatrix ();

	const double scaleX = _size.width ();
	const double scaleY = _size.height ();

	NSRNormalizedPoint startC = sel.getStart ();
	NSRNormalizedPoint endC   = sel.getEnd ();
	NSRNormalizedPoint temp;

	/* If startPoint is right to endPoint swap them */
	if (startC.getX () > endC.getX ()) {
		temp   = startC;
		startC = endC;
		endC   = temp;
	}

	/* minX, maxX, minY, maxY give the bounding rectangle coordinates of the document */

	const NSRNormalizedRect boundingRect = NSRNormalizedRect (0, 0, 1, 1);
	const QRect content = boundingRect.geometry (scaleX, scaleY);
	const double minX = content.left   ();
	const double maxX = content.right  ();
	const double minY = content.top    ();
	const double maxY = content.bottom ();

	/*
	 * We will now find out the NSRTinyTextEntity for the startRectangle and
	 * NSRTinyTextEntity for the endRectangle. We have four cases:
	 *
	 * Case 1 (a): both startpoint and endpoint are out of the bounding
	 * rectangle and at one side, so the rectangle made of start and endPoint
	 * are out of the bounding rectangle (do not intersect).
	 *
	 * Case 1 (b): both startpoint and endpoint are out of bounding rectangle,
	 * but they are in different side, so is their rectangle.
	 *
	 * Case 2 (a): find the rectangle which contains start and endpoint and
	 * having some NSRTextEntity.
	 *
	 * Case 2 (b): if 2 (a) fails (if startPoint and endPoint both are unchanged),
	 * then we check whether there is any NSRTextEntity within the rectangle
	 * made by startPoint and endPoint.
	 *
	 * Case 3: now, we may have two type of selection:
	 * 1. startpoint is left-top of start_end and endpoint is right-bottom;
	 * 2. startpoint is left-bottom of start_end and endpoint is top-right.
	 *
	 * Also, as 2 (b) is passed, we might have it, itEnd or both unchanged,
	 * but the fact is that we have text within them. So, we need to search
	 * for the best suitable text position for start and end.
	 *
	 * Case 3 (a): we search the nearest rectangle consisting of some
	 * NSRTinyTextEntity right to or bottom of the startPoint for selection 01.
	 * And, for selection 02, we have to search for right and top.
	 *
	 * Case 3 (b): for endpoint, we have to find the point top of or left to
	 * endpoint if we have selection 01. Otherwise, the search will be left
	 * and bottom.
	 */

	/* We know that startC.x > endC.x, we need to decide which is top and which is bottom */
	const NSRNormalizedRect startEnd = (startC.getY () < endC.getY ()) ?
				NSRNormalizedRect (startC.getX (), startC.getY (),
						   endC.getX   (), endC.getY   ()) :
				NSRNormalizedRect (startC.getX (), endC.getY   (),
						   endC.getX   (), startC.getY ());

	/* Case 1 (a) */
	if (!boundingRect.intersects (startEnd))
		return ret;
	/* Case 1 (b)
	 *
	 * Note that, after swapping of start and end, we know that,
	 * start is always left to end. But, we cannot say start is
	 * positioned upper than end.
	 */
	else {
		/* If start is left to content rectangle take it to content
		 * rectangle boundary */
		if (startC.getX () * scaleX < minX)
			startC.setX (minX / scaleX);

		if (endC.getX () * scaleX > maxX)
			endC.setX (maxX / scaleX);

		/* If start is top to end (selection type 01) */
		if (startC.getY () * scaleY < minY)
			startC.setY (minY / scaleY);

		if (endC.getY () * scaleY > maxY)
			endC.setY (maxY / scaleY);

		/* If start is bottom to end (selection type 02) */
		if (startC.getY () * scaleY > maxY)
			startC.setY (maxY / scaleY);

		if (endC.getY () * scaleY < minY)
			endC.setY (minY / scaleY);
	}

	NSRTinyTextEntityList::ConstIterator it    = _words.constBegin ();
	NSRTinyTextEntityList::ConstIterator itEnd = _words.constEnd ();
	NSRTinyTextEntityList::ConstIterator start = it;
	NSRTinyTextEntityList::ConstIterator end   = itEnd;
	NSRTinyTextEntityList::ConstIterator tmpIt = it;

	const NSRRegularAreaMergeSide side = (NSRRegularAreaMergeSide) (((int) ((_rotation + _orientation) % 360)) / 90);
	NSRNormalizedRect tmp;

	/* Case 2 (a) */
	for (; it != itEnd; ++it) {
		tmp = (*it)->getArea ();

		if (tmp.contains (startC.getX (), startC.getY ()))
			start = it;

		if (tmp.contains (endC.getX (), endC.getY ()))
			end = it;
	}

	/* Case 2 (b) */
	it = tmpIt;

	if (start == it && end == itEnd) {
		for (; it != itEnd; ++it) {
			/* Is there any text reactangle within the start_end rectangle */
			tmp = (*it)->getArea ();

			if (startEnd.intersects (tmp))
				break;
		}

		/* We have searched every text entities, but none is within the rectangle
		 * created by start and end so, no selection should be done */
		if (it == itEnd)
			return ret;
	}

	it = tmpIt;
	bool selectionTwoStart = false;

	/* Case 3 (a) */
	if (start == it) {
		bool flagV = false;
		NSRNormalizedRect rect;

		/* Selection type 01 */
		if (startC.getY () <= endC.getY ()) {
			for (; it != itEnd; ++it) {
				rect  = (*it)->getArea ();
				flagV = !rect.isBottom (startC);

				if (flagV && rect.isRight (startC)) {
					start = it;
					break;
				}
			}
		}
		/* Selection type 02 */
		else {
			selectionTwoStart = true;
			int distance = scaleX + scaleY + 100;
			int count = 0;

			for (; it != itEnd; ++it) {
				rect = (*it)->getArea ();

				if (rect.isBottomOrLevel (startC) && rect.isRight (startC)) {
					count++;
					QRect entRect = rect.geometry( scaleX, scaleY);
					int xdist, ydist;

					xdist = entRect.center().x () - startC.getX () * scaleX;
					ydist = entRect.center().y () - startC.getY () * scaleY;

					/* Make them positive */
					if (xdist < 0)
						xdist = -xdist;

					if (ydist < 0)
						ydist = -ydist;

					if ((xdist + ydist) < distance) {
						distance = xdist+ ydist;
						start = it;
					}
				}
			}
		}
	}

	/* Case 3 (b) */
	if (end == itEnd) {
		it    = tmpIt;
		itEnd = itEnd - 1;

		bool flagV = false;
		NSRNormalizedRect rect;

		if (startC.getY () <= endC.getY ()) {
			for (; itEnd >= it; itEnd--) {
				rect = (*itEnd)->getArea ();
				flagV = !rect.isTop (endC);

				if (flagV && rect.isLeft (endC)) {
					end = itEnd;
					break;
				}
			}
		} else {
			int distance = scaleX + scaleY + 100;

			for (; itEnd >= it; itEnd--) {
				rect = (*itEnd)->getArea ();

				if (rect.isTopOrLevel (endC) && rect.isLeft (endC)) {
					QRect entRect = rect.geometry (scaleX, scaleY);
					int xdist, ydist;

					xdist = entRect.center().x () - endC.getX () * scaleX;
					ydist = entRect.center().y () - endC.getY () * scaleY;

					/* Make them positive */
					if (xdist < 0)
						xdist = -xdist;

					if (ydist < 0)
						ydist = -ydist;

					if ((xdist + ydist) < distance) {
						distance = xdist+ ydist;
						end = itEnd;
					}

				}
			}
		}
	}

	/* If start and end in selection 02 are in the same column, and we
	 * start at an empty space we have to remove the selection of last
	 * character
	 */
	if (selectionTwoStart && start > end)
		start = start - 1;

	/* If start is less than end swap them */
	if (start > end) {
		it    = start;
		start = end;
		end   = it;
	}

	/* Removes the possibility of crash, in case none of 1 to 3 is true */
	if (end == _words.constEnd ())
		end--;

	for (; start <= end ; start++)
		ret->appendShape ((*start)->getTransformedArea (matrix), side);

	return ret;
}

void
NSRTextPage::setWordList (const NSRTinyTextEntityList& list)
{
	qDeleteAll (_words);
	_words = list;
}

void
NSRTextPage::correctTextOrder ()
{
	NSRTextXYCut::XYCut (_words, _size);
}

NSRRegularAreaRect *
NSRTextPage::searchPointToArea (const NSRSearchPoint *sp)
{
	const QTransform matrix = getTransformMatrix ();
	NSRRegularAreaRect *ret = new NSRRegularAreaRect ();

	for (NSRTinyTextEntityList::ConstIterator it = sp->getBegin (); ; it++) {
		const NSRTinyTextEntity *curEntity = *it;
		ret->append (curEntity->getTransformedArea (matrix));

		if (it == sp->getEnd ())
			break;
	}

	ret->simplify ();
	return ret;
}

NSRRegularAreaRect *
NSRTextPage::findTextInternalForward (int						searchID,
				      const QString&					query,
				      NSRTextComparisonFunction				comparer,
				      const NSRTinyTextEntityList::ConstIterator&	start,
				      int						startOffset,
				      const NSRTinyTextEntityList::ConstIterator&	end)
{
	/* Normalize query search all unicode (including glyphs) */
	const QString queryNorm = query.normalized (QString::NormalizationForm_KC);

	/* j is the current position in our query,
	 * len is the length of the string in NSRTextEntity,
	 * queryLeft is the length of the query we have left
	 */
	int j = 0;
	int queryLeft = queryNorm.length ();

	NSRTinyTextEntityList::ConstIterator it = start;
	int offset = startOffset;

	NSRTinyTextEntityList::ConstIterator itBegin = NSRTinyTextEntityList::ConstIterator ();

	/* Dummy initial value to suppress compiler warnings */
	int offsetBegin = 0;

	while (it != end) {
		const NSRTinyTextEntity *curEntity = *it;
		const QString& str = curEntity->getText ();
		int len = stringLengthAdaptedWithHyphen (str, it, _words.constEnd ());

		if (offset >= len) {
			it++;
			offset = 0;
			continue;
		}

		if (itBegin == NSRTinyTextEntityList::ConstIterator ())  {
			itBegin     = it;
			offsetBegin = offset;
		}

		int min = qMin (queryLeft, len - offset);

		/* We have equal (or less than) area of the query left as the length
		 * of the current entity
		 */
		if (!comparer (str.midRef (offset, min), queryNorm.midRef (j, min))) {
			/* We have not matched,
			 * this means we do not have a complete match,
			 * we need to get back to query start
			 * and continue the search from this place
			 */
			j         = 0;
			queryLeft = queryNorm.length ();
			it        = itBegin;
			offset    = offsetBegin + 1;
			itBegin   = NSRTinyTextEntityList::ConstIterator ();
		} else {
			/* We have a match,
			 * move the current position in the query
			 * to the position after the length of this string
			 * we matched, subtract the length of the current
			 * entity from the left length of the query
			 */
			j         += min;
			queryLeft -= min;

			if (queryLeft == 0) {
				/* Save or update the search point for the current searchID */
				QMap< int, NSRSearchPoint * >::iterator sIt = _searchPoints.find (searchID);

				if (sIt == _searchPoints.end ())
					sIt = _searchPoints.insert (searchID, new NSRSearchPoint);

				NSRSearchPoint *sp = *sIt;
				sp->setBegin (itBegin);
				sp->setEnd (it);
				sp->setOffsetBegin (offsetBegin);
				sp->setOffsetEnd (offset + min);

				return searchPointToArea (sp);
			}

			it++;
			offset = 0;
		}

	}
	/* End of loop - it means that we've ended the text entities */

	const QMap< int, NSRSearchPoint * >::iterator sIt = _searchPoints.find (searchID);

	if (sIt != _searchPoints.end ()) {
		NSRSearchPoint *sp = *sIt;
		_searchPoints.erase (sIt);
		delete sp;
	}

	return NULL;
}

NSRRegularAreaRect *
NSRTextPage::findTextInternalBackward (int						searchID,
				       const QString&					query,
				       NSRTextComparisonFunction			comparer,
				       const NSRTinyTextEntityList::ConstIterator&	start,
				       int						startOffset,
				       const NSRTinyTextEntityList::ConstIterator&	end)
{
	/* Normalize query to search all unicode (including glyphs) */
	const QString queryNorm = query.normalized (QString::NormalizationForm_KC);

	/* j is the current position in our query,
	 * len is the length of the string in NSRTextEntity,
	 * queryLeft is the length of the query we have left
	*/
	int j         = queryNorm.length ();
	int queryLeft = queryNorm.length ();

	NSRTinyTextEntityList::ConstIterator it = start;
	int offset = startOffset;

	NSRTinyTextEntityList::ConstIterator itBegin = NSRTinyTextEntityList::ConstIterator ();

	/* Dummy initial value to suppress compiler warnings */
	int offsetBegin = 0;

	while (true) {
		if (offset <= 0) {
			if (it == end)
				break;

			it--;
		}

		const NSRTinyTextEntity *curEntity = *it;
		const QString& str = curEntity->getText ();
		int len = stringLengthAdaptedWithHyphen (str, it, _words.constEnd ());

		if (offset <= 0)
			offset = len;

		if (itBegin == NSRTinyTextEntityList::ConstIterator ()) {
			itBegin     = it;
			offsetBegin = offset;
		}

		int min = qMin (queryLeft, offset);

		/* We have equal (or less than) area of the query left as the length of the current
		 * entity
		 */

		/* Note len is not str.length () so we can't use rightRef here */
		if (!comparer (str.midRef (offset - min, min), queryNorm.midRef (j - min, min))) {
			/* We have not matched, this means we do not have a complete match
			 * we need to get back to query start and continue the search from this place
			 */
			j         = queryNorm.length ();
			queryLeft = queryNorm.length ();
			it        = itBegin;
			offset    = offsetBegin - 1;
			itBegin   = NSRTinyTextEntityList::ConstIterator ();
		} else {
			/* We have a match, move the current position in the query
			 * to the position after the length of this string we matched,
			 * subtract the length of the current entity from the left
			 * length of the query
			 */
			j         -= min;
			queryLeft -= min;

			if (queryLeft == 0) {
				/* Save or update the search point for the current searchID */
				QMap< int, NSRSearchPoint * >::iterator sIt = _searchPoints.find (searchID);

				if (sIt == _searchPoints.end ())
					sIt = _searchPoints.insert (searchID, new NSRSearchPoint ());

				NSRSearchPoint *sp = *sIt;
				sp->setBegin (it);
				sp->setEnd (itBegin);
				sp->setOffsetBegin (offset - min);
				sp->setOffsetEnd (offsetBegin);

				return searchPointToArea (sp);
			}

			offset = 0;
		}
	}
	/* End of loop - it means that we've ended the text entities */

	const QMap< int, NSRSearchPoint * >::iterator sIt = _searchPoints.find (searchID);

	if (sIt != _searchPoints.end ()) {
		NSRSearchPoint *sp = *sIt;
		_searchPoints.erase (sIt);
		delete sp;
	}

	return NULL;
}

QTransform
NSRTextPage::getTransformMatrix () const
{
	QTransform matrix;
	matrix.rotate ((int) _rotation);

	switch (_rotation) {
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_90:
		matrix.translate (0, -1);
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_180:
		matrix.translate (-1, -1);
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270:
		matrix.translate (-1, 0);
		break;
	default:
		break;
	}

	return matrix;
}
