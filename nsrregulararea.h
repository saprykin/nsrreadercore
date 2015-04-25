#ifndef __NSRREGULARAREA_H__
#define __NSRREGULARAREA_H__

/**
 * @file nsrregulararea.h
 * @author Alexander Saprykin
 * @brief Regular area
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include <QList>
#include <QTransform>

/** The side(s) to be considered when merging areas */
enum NSRRegularAreaMergeSide {
	MergeRight	= 0,	/**< Merge only if the right side of the first area intersects	*/
	MergeBottom	= 1,	/**< Merge only if the bottom side of the first area intersects	*/
	MergeLeft	= 2,	/**< Merge only if the left side of the first area intersects	*/
	MergeTop	= 3,	/**< Merge only if the top side of the first area intersects	*/
	MergeAll	= 4	/**< Merge if the areas intersect, no matter which side(s)	*/
};

/** @cond PRIVATE */
/** @internal */
template <typename T>
void doDelete (T& t)
{
	(void) t;
}

/** @internal */
template <typename T>
T* givePtr (T& t)
{
	return &t;
}

/** @internal */
template <typename T>
T& deref (T& t)
{
	return t;
}

/** @internal */
template <typename T>
static void doDelete (T* t)
{
	delete t;
}

/** @internal */
template <typename T>
static T* givePtr (T* t)
{
	return t;
}

/** @internal */
template <typename T>
static T& deref (T* t)
{
	return *t;
}
/** @endcond */

/**
 * @class NSRRegularArea nsrregulararea.h
 * @brief Regular area
 *
 * A regular area of NormalizedShape which normalizes a Shape.
 *
 * Class #NormalizedShape @b must have the following functions/operators defined:
 * - bool contains (double x, double y);
 * - bool intersects (const NormalizedShape& shape);
 * - bool isNull ();
 * - Shape geometry (int xScale, int yScale);
 * - operator |= (const NormalizedShape& shape) which unite two #NormalizedShape
 * objects.
 */
template <class NormalizedShape, class Shape>
class NSRRegularArea : public  QList<NormalizedShape>
{
public:
	/** Destroys a regular area */
	~NSRRegularArea ();

	/**
	 * @brief Checks whether the regular area contains the normalized point
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 * @return True in case of success, false otherwise.
	 */
	bool contains (double x, double y) const;

	/**
	 * @brief Checks whether the regular area contains the given shape
	 * @param shape Shape to check.
	 * @return True in case of success, false otherwise.
	 */
	bool contains (const NormalizedShape& shape) const;

	/**
	 * @brief Checks whether the regular area intersects with the given area
	 * @param area Area to check intersection with.
	 * @return True in case of success, false otherwise.
	 */
	bool intersects (const NSRRegularArea<NormalizedShape, Shape> *area) const;

	/**
	 * @brief Checks whether the regular area intersects with the given shape
	 * @param shape Shape to check intersection with.
	 * @return True in case of success, false otherwise.
	 */
	bool intersects (const NormalizedShape& shape) const;

	/**
	 * @brief Appends the given area to the regular area
	 * @param area Area to append.
	 */
	void appendArea (const NSRRegularArea<NormalizedShape, Shape> *area);

	/**
	 * @brief Appends the given shape to the regular area
	 * @param shape Shape to append.
	 * @param side Merging mode.
	 */
	void appendShape (const NormalizedShape& shape, NSRRegularAreaMergeSide side = MergeAll);

	/** Simplifies the regular area by merging its intersecting subareas */
	void simplify ();

	/**
	 * @brief Checks whether the regular area is a null area
	 * @return True in case of success, false otherwise.
	 */
	bool isNull () const;

	/**
	 * @brief Gets subareas of the regular areas as shapes for the given scaling factors
	 * and translated by given offsets
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 * @param dx Offset for x-axis.
	 * @param dy Offset for y-axis.
	 * @return Subareas of the regular areas as shapes.
	 */
	QList<Shape> geometry (int xScale, int yScale, int dx = 0, int dy = 0) const;

	/**
	 * @brief Transforms the regular area
	 * @param matrix Transformation matrix.
	 */
	void transform (const QTransform &matrix);
};

template <class NormalizedShape, class Shape>
NSRRegularArea<NormalizedShape, Shape>::~NSRRegularArea ()
{
	int size = this->count ();
	for (int i = 0; i < size; ++i)
		doDelete ((*this)[i]);
}

template <class NormalizedShape, class Shape>
bool NSRRegularArea<NormalizedShape, Shape>::contains (double x, double y) const
{
	if (!this)
		return false;

	if (this->isEmpty ())
		return false;

	typename QList<NormalizedShape>::const_iterator it    = this->begin ();
	typename QList<NormalizedShape>::const_iterator itEnd = this->end ();

	for (; it != itEnd; ++it)
		if ((*it).contains (x, y))
			return true;

	return false;
}

template <class NormalizedShape, class Shape>
bool NSRRegularArea<NormalizedShape, Shape>::contains (const NormalizedShape& shape) const
{
	if (!this)
		return false;

	if (this->isEmpty ())
		return false;

	return QList<NormalizedShape>::contains (shape);
}

template <class NormalizedShape, class Shape>
bool NSRRegularArea<NormalizedShape, Shape>::intersects (const NSRRegularArea<NormalizedShape, Shape> *area) const
{
	if (!this)
		return false;

	if (this->isEmpty ())
		return false;

	typename QList<NormalizedShape>::const_iterator it    = this->begin ();
	typename QList<NormalizedShape>::const_iterator itEnd = this->end ();

	for (; it != itEnd; ++it) {
		typename QList<NormalizedShape>::const_iterator areaIt    = area->begin ();
		typename QList<NormalizedShape>::const_iterator areaItEnd = area->end ();

		for (; areaIt != areaItEnd; ++areaIt) {
			if (!(*it).isNull () && (*it).intersects (*areaIt))
				return true;
		}
	}

	return false;
}

template <class NormalizedShape, class Shape>
bool NSRRegularArea<NormalizedShape, Shape>::intersects (const NormalizedShape& rect) const
{
	if (!this)
		return false;

	if (this->isEmpty ())
		return false;

	typename QList<NormalizedShape>::const_iterator it    = this->begin ();
	typename QList<NormalizedShape>::const_iterator itEnd = this->end ();

	for (; it != itEnd; ++it)
		if (!givePtr(*it)->isNull () && givePtr(*it)->intersects (rect))
			return true;

	return false;
}

template <class NormalizedShape, class Shape>
void NSRRegularArea<NormalizedShape, Shape>::appendArea (const NSRRegularArea<NormalizedShape, Shape> *area)
{
	if (!this)
		return;

	typename QList<NormalizedShape>::const_iterator areaIt    = area->begin ();
	typename QList<NormalizedShape>::const_iterator areaItEnd = area->end ();

	for (; areaIt != areaItEnd; ++areaIt)
		this->append (*areaIt);
}

template <class NormalizedShape, class Shape>
void NSRRegularArea<NormalizedShape, Shape>::appendShape (const NormalizedShape&	shape,
							  NSRRegularAreaMergeSide	side)
{
	if (!this)
		return;

	int size = this->count ();

	/* If the list is empty, adds the shape normally */
	if (size == 0)
		this->append (shape);
	else {
		bool intersection = false;
		NormalizedShape& last = (*this)[size - 1];
#define O_LAST givePtr(last)
#  define O_LAST_R O_LAST->getRight ()
#  define O_LAST_L O_LAST->getLeft ()
#  define O_LAST_T O_LAST->getTop ()
#  define O_LAST_B O_LAST->getBottom ()
#define O_NEW givePtr(shape)
#  define O_NEW_R O_NEW->getRight ()
#  define O_NEW_L O_NEW->getLeft ()
#  define O_NEW_T O_NEW->getTop ()
#  define O_NEW_B O_NEW->getBottom ()
		switch (side) {
		case MergeRight:
			intersection = (O_LAST_R >= O_NEW_L) && (O_LAST_L <= O_NEW_R) &&
					((O_LAST_T <= O_NEW_T && O_LAST_B >= O_NEW_B) ||
					 (O_LAST_T >= O_NEW_T && O_LAST_B <= O_NEW_B));
			break;
		case MergeBottom:
			intersection = (O_LAST_B >= O_NEW_T) && (O_LAST_T <= O_NEW_B) &&
					((O_LAST_R <= O_NEW_R && O_LAST_L >= O_NEW_L) ||
					 (O_LAST_R >= O_NEW_R && O_LAST_L <= O_NEW_L));
			break;
		case MergeLeft:
			intersection = (O_LAST_L <= O_NEW_R) && (O_LAST_R >= O_NEW_L) &&
					((O_LAST_T <= O_NEW_T && O_LAST_B >= O_NEW_B) ||
					 (O_LAST_T >= O_NEW_T && O_LAST_B <= O_NEW_B));
			break;
		case MergeTop:
			intersection = (O_LAST_T <= O_NEW_B) && (O_LAST_B >= O_NEW_T) &&
					((O_LAST_R <= O_NEW_R && O_LAST_L >= O_NEW_L) ||
					 (O_LAST_R >= O_NEW_R && O_LAST_L <= O_NEW_L));
			break;
		case MergeAll:
			intersection = O_LAST->intersects (shape);
			break;
		}
#undef O_LAST
#  undef O_LAST_R
#  undef O_LAST_L
#  undef O_LAST_T
#  undef O_LAST_B
#undef O_NEW
#  undef O_NEW_R
#  undef O_NEW_L
#  undef O_NEW_T
#  undef O_NEW_B
		/* If the new shape intersects with the last shape in the list, then
		 * merge it with that and delete the shape */
		if (intersection) {
			deref ((*this)[size - 1]) |= deref (shape);
			doDelete (const_cast<NormalizedShape&> (shape));
		}
		else
			this->append (shape);
	}
}

template <class NormalizedShape, class Shape>
void NSRRegularArea<NormalizedShape, Shape>::simplify ()
{
	int end = this->count () - 1;
	int x = 0;

	for (int i = 0; i < end; ++i) {
		if (givePtr((*this)[x])->intersects (deref ((*this)[i + 1]))) {
			deref ((*this)[x]) |= deref ((*this)[i + 1]);
			NormalizedShape& tobedeleted = (*this)[i + 1];
			this->removeAt (i + 1);
			doDelete (tobedeleted);
			--end;
			--i;
		} else
			x = i + 1;
	}
}

template <class NormalizedShape, class Shape>
bool NSRRegularArea<NormalizedShape, Shape>::isNull () const
{
	if (!this)
		return false;

	if (this->isEmpty ())
		return false;

	typename QList<NormalizedShape>::const_iterator it    = this->begin ();
	typename QList<NormalizedShape>::const_iterator itEnd = this->end ();

	for (; it != itEnd; ++it)
		if (!givePtr(*it)->isNull ())
			return false;

	return true;
}

template <class NormalizedShape, class Shape>
QList<Shape> NSRRegularArea<NormalizedShape, Shape>::geometry (int xScale, int yScale, int dx, int dy) const
{
	if (!this || this->isEmpty ())
		return QList<Shape> ();

	QList<Shape> ret;
	Shape t;
	typename QList<NormalizedShape>::const_iterator it    = this->begin ();
	typename QList<NormalizedShape>::const_iterator itEnd = this->end ();

	for (; it != itEnd; ++it) {
		t = givePtr(*it)->geometry (xScale, yScale);
		t.translate (dx, dy);
		ret.append (t);
	}

	return ret;
}

template <class NormalizedShape, class Shape>
void NSRRegularArea<NormalizedShape, Shape>::transform (const QTransform &matrix)
{
	if (!this)
		return;

	if (this->isEmpty ())
		return;

	for (int i = 0; i < this->count (); ++i)
		givePtr((*this)[i])->transform (matrix);
}

#endif /* __NSRREGULARAREA_H__ */
