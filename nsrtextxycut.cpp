#include "nsrtextxycut.h"

#include <QtAlgorithms>
#include <QVarLengthArray>
#include <QMap>

static bool compareTinyTextEntityX (const NSRWordWithCharacters& first,
				    const NSRWordWithCharacters& second)
{
	QRect firstArea  = first.getArea().roundedGeometry (1000, 1000);
	QRect secondArea = second.getArea().roundedGeometry (1000, 1000);

	return firstArea.left () < secondArea.left ();
}

static bool compareTinyTextEntityY (const NSRWordWithCharacters& first,
				    const NSRWordWithCharacters& second)
{
	const QRect firstArea  = first.getArea().roundedGeometry (1000, 1000);
	const QRect secondArea = second.getArea().roundedGeometry (1000, 1000);

	return firstArea.top () < secondArea.top ();
}

/*
 * Returns true if segments [left1, right1] and [left2, right2] on the real line
 * overlap within threshold percent, i.e. if the ratio of the length of the
 * intersection of the segments to the length of the shortest of the two input segments
 * is not smaller than the threshold.
 */
static bool segmentsOverlap (double left1, double right1, double left2, double right2, int threshold)
{
	/* Check if one consumes another fully (speed optimization) */
	if (left1 <= left2 && right1 >= right2)
		return true;

	if (left1 >= left2 && right1 <= right2)
		return true;

	/* Check if there is overlap above threshold */
	if (right2 >= left1 && right1 >= left2) {
		double overlap = (right2 >= right1) ? right1 - left2
						    : right2 - left1;

		double length1 = right1 - left1;
		double length2 = right2 - left2;

		return overlap * 100.0 >= threshold * qMin (length1, length2);
	}

	return false;
}

bool
NSRTextXYCut::doesConsumeY (const QRect& first, const QRect& second, int threshold)
{
	return segmentsOverlap (first.top (), first.bottom (), second.top (), second.bottom (), threshold);
}

bool
NSRTextXYCut::doesConsumeY (const NSRNormalizedRect& first, const NSRNormalizedRect& second, int threshold)
{
	return segmentsOverlap (first.getTop (), first.getBottom (),
				second.getTop (), second.getBottom (),
				threshold);
}

void
NSRTextXYCut::XYCut (NSRTinyTextEntityList& list, const QSize& pageSize)
{
	if (pageSize.width () * pageSize.height () <= 0)
		return;

	/* Page width and height are in pixels at 100% zoom level, and thus depend
	 * on display DPI. We scale pageWidth and pageHeight to remove the dependence.
	 * Otherwise bugs would be more difficult to reproduce and app could fail in
	 * extreme cases like a large TV with low DPI.
	 */
	const double scalingFactor = 2000.0 / (pageSize.width () + pageSize.height ());
	const int pageWidth        = (int) (scalingFactor * pageSize.width ());
	const int pageHeight       = (int) (scalingFactor * pageSize.height ());

	NSRTinyTextEntityList characters = list;

	/** Remove spaces from the text */
	removeSpace (characters);

	/** Construct words from characters */
	NSRWordWithCharactersList wordsWithCharacters = makeWordFromCharacters (characters,
										pageWidth,
										pageHeight);

	/** Make a XY cut tree for segmentation of the texts */
	NSRRegionTextList tree = XYCutForBoundingBoxes (wordsWithCharacters,
							NSRNormalizedRect (0, 0, 1, 1),
							pageWidth,
							pageHeight);

	/** Add spaces to the word */
	NSRWordWithCharactersList listWithWordsAndSpaces = addNecessarySpace (tree,
									      pageWidth,
									      pageHeight);

	qDeleteAll (list);

	/** Break the words into characters */
	foreach (const NSRWordWithCharacters &word, listWithWordsAndSpaces)
	    list.append (word.getCharacters ());
}

void
NSRTextXYCut::removeSpace (NSRTinyTextEntityList& words)
{
	NSRTinyTextEntityList::Iterator it = words.begin ();
	const QString str (' ');

	while (it != words.end ()) {
		if ((*it)->getText () == str)
			it = words.erase (it);
		else
			++it;
	}
}

NSRWordWithCharactersList
NSRTextXYCut::makeWordFromCharacters (const NSRTinyTextEntityList&	characters,
				      int				pageWidth,
				      int				pageHeight)
{
	/* We will traverse characters and try to create words from the
	 * NSRTinyTextEntity's in it. We will search NSRTinyTextEntity blocks
	 * and merge them until we get a space between two consecutive
	 * NSRTinyTextEntity's. When we get a space we can take it as a end
	 * of word. Then we store the word as a NSRTinyTextEntity and keep it in newList.
	 *
	 * We create a NSRRegionText named regionWord that contains the word
	 * and the characters associated with it and a rectangle area of the element
	 * in newList.
	 */
	NSRWordWithCharactersList wordsWithCharacters;

	NSRTinyTextEntityList::ConstIterator it    = characters.begin ();
	NSRTinyTextEntityList::ConstIterator itEnd = characters.end ();
	NSRTinyTextEntityList::ConstIterator tmpIt;

	int newLeft;
	int newRight;
	int newTop;
	int newBottom;
	int index = 0;

	for (; it != itEnd ; it++) {
		QString textString = (*it)->getText ();
		QString newString;
		QRect lineArea = (*it)->getArea().roundedGeometry (pageWidth, pageHeight);
		QRect elementArea;
		NSRTinyTextEntityList wordCharacters;

		tmpIt     = it;
		int space = 0;

		while (!space) {
			if (textString.length ()) {
				newString.append (textString);

				/* When textString is the start of the word */
				if (tmpIt == it) {
					NSRNormalizedRect newRect (lineArea, pageWidth, pageHeight);
					wordCharacters.append (new NSRTinyTextEntity (textString.normalized
										      (QString::NormalizationForm_KC),
										      newRect));
				} else {
					NSRNormalizedRect newRect (elementArea, pageWidth, pageHeight);
					wordCharacters.append(new NSRTinyTextEntity (textString.normalized
										     (QString::NormalizationForm_KC),
										     newRect));
				}
			}

			++it;

			/* We must have to put this line before the if condition of (it == itEnd)
			 * otherwise the last character can be missed
			 */
			if (it == itEnd)
				break;

			elementArea = (*it)->getArea().roundedGeometry (pageWidth, pageHeight);

			if (!doesConsumeY (elementArea, lineArea, 60)) {
				--it;
				break;
			}

			const int textY1 = elementArea.top ();
			const int textX1 = elementArea.left ();
			const int textY2 = elementArea.y () + elementArea.height ();
			const int textX2 = elementArea.x () + elementArea.width ();

			const int lineY1 = lineArea.top ();
			const int lineX1 = lineArea.left ();
			const int lineY2 = lineArea.y () + lineArea.height ();
			const int lineX2 = lineArea.x () + lineArea.width ();

			space = elementArea.left () - lineArea.right ();

			if (space != 0) {
				it--;
				break;
			}

			newLeft   = (textX1 < lineX1) ? textX1 : lineX1;
			newRight  = (lineX2 > textX2) ? lineX2 : textX2;
			newTop    = (textY1 > lineY1) ? lineY1 : textY1;
			newBottom = (textY2 > lineY2) ? textY2 : lineY2;

			lineArea.setLeft   (newLeft);
			lineArea.setTop    (newTop);
			lineArea.setWidth  (newRight - newLeft);
			lineArea.setHeight (newBottom - newTop);

			textString = (*it)->getText ();
		}

		/* If newString is not empty, save it */
		if (!newString.isEmpty ()) {
			const NSRNormalizedRect newRect (lineArea, pageWidth, pageHeight);
			NSRTinyTextEntity *word = new NSRTinyTextEntity (newString.
									 normalized (QString::NormalizationForm_KC),
									 newRect);
			wordsWithCharacters.append (NSRWordWithCharacters (word, wordCharacters));
			index++;
		}

		if (it == itEnd)
			break;
	}

	return wordsWithCharacters;
}

NSRRegionTextList
NSRTextXYCut::XYCutForBoundingBoxes (const NSRWordWithCharactersList&	wordsWithCharacters,
				     const NSRNormalizedRect&		boundingBox,
				     int				pageWidth,
				     int				pageHeight)
{
	NSRRegionTextList tree;
	QRect contentRect (boundingBox.geometry (pageWidth, pageHeight));
	const NSRRegionText root (wordsWithCharacters, contentRect);

	/* Start the tree with the root, it is our only region at the start */
	tree.push_back (root);

	int i = 0;

	/* While traversing the tree has not been ended */
	while (i < tree.length ()) {
		const NSRRegionText node = tree.at (i);
		QRect regionRect = node.getArea ();

		/* 1. Calculation of projection profiles */

		/* Allocate the size of proj profiles and initialize with 0 */
		int sizeProjY = node.getArea().height ();
		int sizeProjX = node.getArea().width ();

		/* Dynamic memory allocation */
		QVarLengthArray<int> projOnXaxis (sizeProjX);
		QVarLengthArray<int> projOnYaxis (sizeProjY);

		for (int j = 0; j < sizeProjY; ++j)
			projOnYaxis[j] = 0;

		for (int j = 0; j < sizeProjX; ++j)
			projOnXaxis[j] = 0;

		const QList<NSRWordWithCharacters> list = node.getText ();

		/* Calculate tcx and tcy locally for each new region */
		int wordSpacing;
		int lineSpacing;
		int columnSpacing;

		calculateStatisticalInformation (list,
						 pageWidth,
						 pageHeight,
						 &wordSpacing,
						 &lineSpacing,
						 &columnSpacing);

		const int tcx = wordSpacing * 2;
		const int tcy = lineSpacing * 2;

		int maxX = 0;
		int maxY = 0;
		int avgX = 0;
		int count;

		/* For every text in the region */
		for (int j = 0; j < list.length (); ++j) {
			NSRTinyTextEntity *ent = list.at(j).getWord ();
			const QRect entRect = ent->getArea().geometry (pageWidth, pageHeight);

			/* Calculate vertical projection profile projOnXaxis */
			for (int k = entRect.left (); k <= entRect.left () + entRect.width (); ++k) {
				if ((k - regionRect.left ()) < sizeProjX && (k - regionRect.left ()) >= 0)
					projOnXaxis[k - regionRect.left ()] += entRect.height ();
			}

			/* Calculate horizontal projection profile in the same way */
			for (int k = entRect.top (); k <= entRect.top () + entRect.height (); ++k) {
				if ((k - regionRect.top ()) < sizeProjY && (k - regionRect.top ()) >= 0)
					projOnYaxis[k - regionRect.top ()] += entRect.width ();
			}
		}

		for (int j = 0; j < sizeProjY; ++j) {
			if (projOnYaxis[j] > maxY)
				maxY = projOnYaxis[j];
		}

		avgX = count = 0;

		for (int j = 0; j < sizeProjX; ++j) {
			if (projOnXaxis[j] > maxX)
				maxX = projOnXaxis[j];

			if (projOnXaxis[j]) {
				count++;
				avgX += projOnXaxis[j];
			}
		}

		if (count)
			avgX /= count;


		/* 2. Cleanup boundary white spaces and removal of noise */
		int xbegin = 0;
		int xend   = sizeProjX - 1;
		int ybegin = 0;
		int yend   = sizeProjY - 1;

		while (xbegin < sizeProjX && projOnXaxis[xbegin] <= 0)
			xbegin++;

		while (xend >= 0 && projOnXaxis[xend] <= 0)
			xend--;

		while (ybegin < sizeProjY && projOnYaxis[ybegin] <= 0)
			ybegin++;

		while (yend >= 0 && projOnYaxis[yend] <= 0)
			yend--;

		/* Update the regionRect */
		int oldLeft = regionRect.left ();
		int oldTop  = regionRect.top ();

		regionRect.setLeft   (oldLeft + xbegin);
		regionRect.setRight  (oldLeft + xend);
		regionRect.setTop    (oldTop  + ybegin);
		regionRect.setBottom (oldTop  + yend);

		int tnx = (int) ((double) avgX * 10.0 / 100.0 + 0.5);
		int tny = 0;

		for (int j = 0; j < sizeProjX; ++j)
			projOnXaxis[j] -= tnx;

		for (int j = 0; j < sizeProjY; ++j)
			projOnYaxis[j] -= tny;

		/* 3. Find the widest gap */
		int gapHor = -1;
		int posHor = -1;
		int begin  = -1;
		int end    = -1;

		/* Find all horizontal gaps and find the maximum between them */
		for (int j = 1; j < sizeProjY; ++j) {
			/* Transition from white to black */
			if (begin >= 0 && projOnYaxis[j-1] <= 0 && projOnYaxis[j] > 0)
				end = j;

			/* Transition from black to white */
			if (projOnYaxis[j-1] > 0 && projOnYaxis[j] <= 0)
				begin = j;

			if (begin > 0 && end > 0 && end - begin > gapHor) {
				gapHor = end - begin;
				posHor = (end + begin) / 2;
				begin  = -1;
				end    = -1;
			}
		}

		begin      = -1;
		end        = -1;
		int gapVer = -1;
		int posVer = -1;

		/* Find all the vertical gaps and find the maximum between them */
		for (int j = 1; j < sizeProjX; ++j) {
			/* Transition from white to black */
			if (begin >= 0 && projOnXaxis[j-1] <= 0 && projOnXaxis[j] > 0)
				end = j;

			/* Transition from black to white */
			if (projOnXaxis[j-1] > 0 && projOnXaxis[j] <= 0)
				begin = j;

			if (begin > 0 && end > 0 && end - begin > gapVer) {
				gapVer = end - begin;
				posVer = (end + begin) / 2;
				begin  = -1;
				end    = -1;
			}
		}

		int cutPosX = posVer;
		int cutPosY = posHor;
		int gapX    = gapVer;
		int gapY    = gapHor;

		/* 4. Cut the region and make nodes (left, right) or (up, down) */
		bool cutHor = false;
		bool cutVer = false;

		/* For horizontal cut */
		const int topHeight = cutPosY - (regionRect.top () - oldTop);
		const QRect topRect (regionRect.left  (),
				     regionRect.top   (),
				     regionRect.width (),
				     topHeight);
		const QRect bottomRect (regionRect.left   (),
					regionRect.top    () + topHeight,
					regionRect.width  (),
					regionRect.height () - topHeight);

		/* For vertical Cut */
		const int leftWidth = cutPosX - (regionRect.left () - oldLeft);
		const QRect leftRect (regionRect.left (),
				      regionRect.top  (),
				      leftWidth,
				      regionRect.height ());
		const QRect rightRect (regionRect.left   () + leftWidth,
				       regionRect.top    (),
				       regionRect.width  () - leftWidth,
				       regionRect.height ());

		if (gapY >= gapX && gapY >= tcy)
			cutHor = true;
		else if (gapY >= gapX && gapY <= tcy && gapX >= tcx)
			cutVer = true;
		else if (gapX >= gapY && gapX >= tcx)
			cutVer = true;
		else if (gapX >= gapY && gapX <= tcx && gapY >= tcy)
			cutHor = true;
		/* No cut possible */
		else {
			/* We can now update the node rectangle with the shrinked rectangle */
			NSRRegionText tmpNode = tree.at (i);
			tmpNode.setArea (regionRect);
			tree.replace (i, tmpNode);
			i++;
			continue;
		}

		NSRWordWithCharactersList list1, list2;

		/* Horizontal cut, topRect and bottomRect */
		if (cutHor) {
			for (int j = 0; j < list.length (); ++j) {
				const NSRWordWithCharacters word = list.at (j);
				const QRect wordRect = word.getArea().geometry (pageWidth, pageHeight);

				if (topRect.intersects (wordRect))
					list1.append (word);
				else
					list2.append (word);
			}

			NSRRegionText node1 (list1, topRect);
			NSRRegionText node2 (list2, bottomRect);

			tree.replace (i, node1);
			tree.insert (i + 1, node2);
		}
		/* Vertical cut, leftRect and rightRect */
		else if (cutVer) {
			for (int j = 0; j < list.length (); ++j) {
				const NSRWordWithCharacters word = list.at (j);
				const QRect wordRect = word.getArea().geometry (pageWidth, pageHeight);

				if (leftRect.intersects (wordRect))
					list1.append (word);
				else
					list2.append (word);
			}

			NSRRegionText node1 (list1, leftRect);
			NSRRegionText node2 (list2, rightRect);

			tree.replace (i, node1);
			tree.insert (i + 1, node2);
		}
	}

	return tree;
}

NSRWordWithCharactersList
NSRTextXYCut::addNecessarySpace (NSRRegionTextList& tree, int pageWidth, int pageHeight)
{
	/* 1. Call makeAndSortLines () before adding spaces in between words in a line.
	 * 2. Now add spaces between every two words in a line.
	 * 3. Finally, extract all the space separated texts from each region and return it.
	 */

	/* Only change the texts under NSRRegionTexts, not the area */
	for (int j = 0; j < tree.length (); j++) {
		NSRRegionText& tmpRegion = tree[j];

		/* Step 01 */
		QList< QPair<NSRWordWithCharactersList, QRect> > sortedLines =
				makeAndSortLines (tmpRegion.getText (), pageWidth, pageHeight);

		/* Step 02 */
		for (int i = 0; i < sortedLines.length (); i++) {
			NSRWordWithCharactersList& list = sortedLines[i].first;
			for (int k = 0; k < list.length (); k++) {
				const QRect area1 = list.at(k).getArea().roundedGeometry (pageWidth, pageHeight);

				if (k + 1 >= list.length ())
					break;

				const QRect area2 = list.at(k+1).getArea().roundedGeometry (pageWidth, pageHeight);
				const int space = area2.left () - area1.right ();

				if (space != 0) {
					/* Make a NSRTinyTextEntity of string space and push it between it and it + 1 */
					const int left   = area1.right ();
					const int right  = area2.left ();
					const int top    = area2.top () < area1.top () ? area2.top () : area1.top ();
					const int bottom = area2.bottom () > area1.bottom () ? area2.bottom () : area1.bottom ();

					const QString spaceStr (" ");
					const QRect rect (QPoint (left, top), QPoint (right, bottom));
					const NSRNormalizedRect entRect (rect, pageWidth, pageHeight);
					NSRTinyTextEntity *ent1 = new NSRTinyTextEntity (spaceStr, entRect);
					NSRTinyTextEntity *ent2 = new NSRTinyTextEntity (spaceStr, entRect);
					NSRWordWithCharacters word (ent1, QList<NSRTinyTextEntity *> () << ent2);

					list.insert (k + 1, word);

					/* Skip the space */
					k++;
				}
			}
		}

		NSRWordWithCharactersList tmpList;
		for (int i = 0; i < sortedLines.length (); i++)
			tmpList += sortedLines.at(i).first;

		tmpRegion.setText (tmpList);
	}

	/* Step 03 */
	NSRWordWithCharactersList tmp;

	for (int i = 0; i < tree.length (); i++)
		tmp += tree.at(i).getText ();

	return tmp;
}

void
NSRTextXYCut::calculateStatisticalInformation (const NSRWordWithCharactersList&	words,
					       int				pageWidth,
					       int				pageHeight,
					       int *				wordSpacing,
					       int *				lineSpacing,
					       int *				colSpacing)
{
	/* For the region, defined by line rectangles and lines:
	 * 1. Make line statistical analysis to find the line spacing.
	 * 2. Make character statistical analysis to differentiate between
	 *    word spacing and column spacing.
	 */

	/* Step 0 */
	const QList< QPair<NSRWordWithCharactersList, QRect> > sortedLines =
			makeAndSortLines (words, pageWidth, pageHeight);

	/* Step 1 */
	QMap<int, int> lineSpaceStat;
	for (int i = 0; i < sortedLines.length (); i++) {
		const QRect rectUpper = sortedLines.at(i).second;

		if (i + 1 == sortedLines.length ())
			break;

		const QRect rectLower = sortedLines.at(i + 1).second;

		int linespace = rectLower.top () - (rectUpper.top () + rectUpper.height ());

		if (linespace < 0)
			linespace = -linespace;

		if (lineSpaceStat.contains (linespace))
			lineSpaceStat[linespace]++;
		else
			lineSpaceStat[linespace] = 1;
	}

	*lineSpacing      = 0;
	int weightedCount = 0;
	QMapIterator<int, int> iterateLinespace (lineSpaceStat);

	while (iterateLinespace.hasNext ()) {
		iterateLinespace.next ();
		*lineSpacing  += iterateLinespace.value () * iterateLinespace.key ();
		weightedCount += iterateLinespace.value ();
	}

	if (*lineSpacing != 0)
		*lineSpacing = (int) ((double) *lineSpacing / (double) weightedCount + 0.5);

	/* Step 2 */

	/* We would like to use QMap instead of QHash as it will keep the keys sorted */
	QMap<int,int>		horSpaceStat;
	QMap<int,int>		colSpaceStat;
	QList< QList<QRect> >	spaceRects;
	QList<QRect>		maxHorSpaceRects;

	/* Space in every line */
	for (int i = 0; i < sortedLines.length (); i++) {
		const NSRWordWithCharactersList list = sortedLines.at(i).first;
		QList<QRect> lineSpaceRects;
		int maxSpace = 0;
		int minSpace = pageWidth;

		/* For every NSRTinyTextEntity element in the line */
		NSRWordWithCharactersList::ConstIterator it    = list.begin ();
		NSRWordWithCharactersList::ConstIterator itEnd = list.end ();
		QRect maxArea1;
		QRect maxArea2;
		QString beforeMax;
		QString afterMax;

		/* For every line */
		for (; it != itEnd; it++) {
			const QRect area1 = (*it).getArea().roundedGeometry (pageWidth, pageHeight);

			if (it + 1 == itEnd)
				break;

			const QRect area2 = (*(it + 1)).getArea().roundedGeometry (pageWidth, pageHeight);
			int space = area2.left () - area1.right ();

			if (space > maxSpace) {
				maxArea1  = area1;
				maxArea2  = area2;
				maxSpace  = space;
				beforeMax = (*it).getText ();
				afterMax  = (*(it + 1)).getText ();
			}

			if (space < minSpace && space != 0)
				minSpace = space;

			/* If we found a real space, whose length is not zero and also
			 * less than the pageWidth
			 */
			if (space != 0 && space != pageWidth) {
				/* Increase the count of the space amount */
				if (horSpaceStat.contains (space))
					horSpaceStat[space]++;
				else
					horSpaceStat[space] = 1;

				int left = area1.right ();
				int right = area2.left ();

				int top = area2.top () < area1.top () ? area2.top () : area1.top ();
				int bottom = area2.bottom () > area1.bottom () ? area2.bottom () : area1.bottom ();

				QRect rect (left, top, right - left, bottom - top);
				lineSpaceRects.append (rect);
			}
		}

		spaceRects.append (lineSpaceRects);

		if (horSpaceStat.contains (maxSpace)) {
			if (horSpaceStat[maxSpace] != 1)
				horSpaceStat[maxSpace]--;
			else
				horSpaceStat.remove (maxSpace);
		}

		if (maxSpace != 0) {
			if (colSpaceStat.contains (maxSpace))
				colSpaceStat[maxSpace]++;
			else
				colSpaceStat[maxSpace] = 1;

			/* Store the max rectangle of each line */
			const int left  = maxArea1.right ();
			const int right = maxArea2.left ();
			const int top   = (maxArea1.top () > maxArea2.top ()) ?
						maxArea2.top () : maxArea1.top ();
			const int bottom = (maxArea1.bottom () < maxArea2.bottom ()) ?
						maxArea2.bottom () : maxArea1.bottom ();

			const QRect rect (left, top, right - left, bottom - top);
			maxHorSpaceRects.append (rect);
		} else
			maxHorSpaceRects.append (QRect (0, 0, 0, 0));
	}

	/* All the between word space counts are in horSpaceStat */
	*wordSpacing  = 0;
	weightedCount = 0;
	QMapIterator<int, int> iterate (horSpaceStat);

	while (iterate.hasNext ()) {
		iterate.next ();

		if (iterate.key () > 0) {
			*wordSpacing  += iterate.value () * iterate.key ();
			weightedCount += iterate.value ();
		}
	}

	if (weightedCount)
		*wordSpacing = (int) ((double) *wordSpacing / (double) weightedCount + 0.5);

	*colSpacing = 0;
	QMapIterator<int, int> iterateCol (colSpaceStat);

	while (iterateCol.hasNext ()) {
		iterateCol.next ();

		if (iterateCol.value () > *colSpacing)
			*colSpacing = iterateCol.value ();
	}

	*colSpacing = colSpaceStat.key (*colSpacing);

	/* If there is just one line in a region, there is no point in dividing it */
	if (sortedLines.length () == 1)
		*wordSpacing = *colSpacing;
}

QList< QPair<NSRWordWithCharactersList, QRect> >
NSRTextXYCut::makeAndSortLines (const NSRWordWithCharactersList&	words,
				int					pageWidth,
				int					pageHeight)
{
	/* We cannot assume that the generator will give us texts in the right order.
	 * We can only assume that we will get texts in the page and their bounding
	 * rectangle. The texts can be character, word, half-word anything.
	 * So, we need to:
	 * 1. Sort rectangles/boxes containing texts by y0 (top).
	 * 2. Create textline where there is y overlap between NSRTinyTextEntity's.
	 * 3. Within each line sort the NSRTinyTextEntity's by x0 (left).
	 */

	QList< QPair<NSRWordWithCharactersList, QRect> > lines;

	/* Make a new copy of the TextList in the words, so that the wordsTmp and lines do
	 * not contain same pointers for all the NSRTinyTextEntity.
	 */
	NSRWordWithCharactersList wordsCopy = words;

	/* Step 1 */
	qSort (wordsCopy.begin (), wordsCopy.end (), compareTinyTextEntityY);

	/* Step 2 */
	QList<NSRWordWithCharacters>::Iterator it    = wordsCopy.begin ();
	QList<NSRWordWithCharacters>::Iterator itEnd = wordsCopy.end ();

	/* For every non-space texts (characters/words) in the textList */
	for (; it != itEnd; it++) {
		const QRect elementArea = (*it).getArea().roundedGeometry (pageWidth, pageHeight);
		bool found = false;

		for (int i = 0; i < lines.length (); i++) {
			/* The line area which will be expanded,
			 * line rectangles is only necessary to preserve the topmin and bottommax
			 * of all the texts in the line, left and right is not necessary at all.
			 */
			QRect &lineArea = lines[i].second;
			const int textY1 = elementArea.top  ();
			const int textY2 = elementArea.top  () + elementArea.height ();
			const int textX1 = elementArea.left ();
			const int textX2 = elementArea.left () + elementArea.width ();

			const int lineY1 = lineArea.top  ();
			const int lineY2 = lineArea.top  () + lineArea.height ();
			const int lineX1 = lineArea.left ();
			const int lineX2 = lineArea.left () + lineArea.width ();

			/* If the new text and the line has y overlapping parts of more than 70%,
			 * the text will be added to this line
			 */
			if (doesConsumeY (elementArea, lineArea, 70)) {
				NSRWordWithCharactersList &line = lines[i].first;
				line.append (*it);

				const int newLeft   = (lineX1 < textX1) ? lineX1 : textX1;
				const int newRight  = (lineX2 > textX2) ? lineX2 : textX2;
				const int newTop    = (lineY1 < textY1) ? lineY1 : textY1;
				const int newBottom = (textY2 > lineY2) ? textY2 : lineY2;

				lineArea = QRect (newLeft, newTop, newRight - newLeft, newBottom - newTop);
				found = true;
			}

			if (found)
				break;
		}

		/* When we have found a new line create a new TextList containing
		 * only one element and append it to the lines
		 */
		if (!found) {
			NSRWordWithCharactersList tmp;
			tmp.append ((*it));
			lines.append (QPair<NSRWordWithCharactersList, QRect> (tmp, elementArea));
		}
	}

	/* Step 3 */
	for (int i = 0; i < lines.length (); i++) {
		NSRWordWithCharactersList& list = lines[i].first;
		qSort (list.begin (), list.end (), compareTinyTextEntityX);
	}

	return lines;
}

NSRTextXYCut::NSRTextXYCut ()
{
}

NSRTextXYCut::~NSRTextXYCut ()
{
}
