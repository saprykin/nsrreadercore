#ifndef __NSRTEXTXYCUT_H__
#define __NSRTEXTXYCUT_H__

/**
 * @file nsrtextxycut.h
 * @author Alexander Saprykin
 * @brief XY cut algorithm for text page
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrtinytextentity.h"
#include "nsrwordwithcharacters.h"
#include "nsrregiontext.h"

#include <QSize>
#include <QPair>

/**
 * @class NSRTextXYCut nsrtextxycut.h
 * @brief Implementation of XY cut algorithm for text page
 */
class NSRTextXYCut
{
public:
	/**
	 * @brief Checks whether segments are overlapped with given threshold
	 * @param first First area.
	 * @param second Second area.
	 * @param threshold Overlap thresholsd value.
	 * @return True in case of success, false otherwise.
	 */
	static bool doesConsumeY (const QRect& first, const QRect& second, int threshold);

	/**
	 * @brief Checks whether segments are overlapped with given threshold
	 * @param first First normalized area.
	 * @param second Second normalized area.
	 * @param threshold Overlap thresholsd value.
	 * @return True in case of success, false otherwise.
	 */
	static bool doesConsumeY (const NSRNormalizedRect&	first,
				  const NSRNormalizedRect&	second,
				  int				threshold);

	/**
	 * @brief Performs XY cut algorithm on given text word list
	 * @param[in, out] list List of words.
	 * @param pageSize Size of the page, in pixels.
	 */
	static void XYCut (NSRTinyTextEntityList& list, const QSize& pageSize);

private:
	Q_DISABLE_COPY (NSRTextXYCut)

	/**
	 * @brief Removes all the spaces in between texts
	 * @param[in, out] words List of text words.
	 *
	 * It will make all the generators same, whether they save spaces (like PDF)
	 * or not (like DjVu).
	 */
	static void removeSpace (NSRTinyTextEntityList& words);

	/**
	 * @brief Tries to read text enities from characters and creates words.
	 * @param characters List of text entities to create words from.
	 * @param pageWidth Page width, in pixels.
	 * @param pageHeight Page height, in pixels.
	 * @return List of words with characters.
	 * @note Characters might be already characters for some generators, but we will keep
	 * the nomenclature characters for the generator produced data.
	 *
	 * The caller takes ownership for the words within returned list.
	 */
	static NSRWordWithCharactersList
	makeWordFromCharacters (const NSRTinyTextEntityList&	characters,
				int				pageWidth,
				int				pageHeight);

	/**
	 * @brief Implements the XY Cut algorithm for text page segmentation
	 * @param wordsWithCharacters List of words with characters.
	 * @param boundingBox Text normalized bounding box.
	 * @param pageWidth Page width, in pixels.
	 * @param pageHeight Page height, in pixels.
	 * @return List of text regions.
	 * @note The returned list will contain words and their characters reused from
	 * @a wordsWithCharacters, so no new nor delete memory calls happens in this function.
	 */
	static NSRRegionTextList
	XYCutForBoundingBoxes (const NSRWordWithCharactersList&	wordsWithCharacters,
			       const NSRNormalizedRect&		boundingBox,
			       int				pageWidth,
			       int				pageHeight);

	/**
	 * @brief Adds spaces in between words in a line
	 * @param tree A list of text regions to insert spaces in.
	 * @param pageWidth Page width, in pixels.
	 * @param pageHeight Page height, in pixels.
	 * @return List of words with characters and spaces between.
	 * @note It reuses the pointers passed in @a tree and might add new ones.
	 *
	 * The caller takes ownership for the words within returned list.
	 */
	static NSRWordWithCharactersList
	addNecessarySpace (NSRRegionTextList& tree, int pageWidth, int pageHeight);

	/**
	 * @brief Calculates statistical information from the lines
	 * @param words List of words with characters.
	 * @param pageWidth Page width, in pixels.
	 * @param pageHeight Page height, in pixels.
	 * @param[out] wordSpacing Word spacing statistics.
	 * @param[out] lineSpacing Line spacing statistics.
	 * @param[out] colSpacing Column spacing statistics.
	 */
	static void calculateStatisticalInformation (const NSRWordWithCharactersList&	words,
						     int				pageWidth,
						     int				pageHeight,
						     int *				wordSpacing,
						     int *				lineSpacing,
						     int *				colSpacing);

	/**
	 * @brief Creates lines from the words and sorts them
	 * @param words Words to create lines from.
	 * @param pageWidth Page width, in pixels.
	 * @param pageHeight Page height, in pixels.
	 * @return A list of pairs of line and its area.
	 */
	static QList< QPair<NSRWordWithCharactersList, QRect> >
	makeAndSortLines (const NSRWordWithCharactersList&	words,
			  int					pageWidth,
			  int					pageHeight);

	/** Private constructor */
	NSRTextXYCut ();

	/** Private destructor */
	~NSRTextXYCut ();
};

#endif /* __NSRTEXTXYCUT_H__ */
