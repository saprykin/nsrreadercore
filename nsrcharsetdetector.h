#ifndef __NSRCHARSETDETECTOR_H__
#define __NSRCHARSETDETECTOR_H__

/**
 * @file nsrcharsetdetector.h
 * @author Alexander Saprykin
 * @brief Charset (encoding) detector
 */

#include "universalchardet/nsUniversalDetector.h"

#include <QString>

/**
 * @class NSRCharsetDetector nsrcharsetdetector.h
 * @brief Charset (encoding) detector class
 * @since 1.4.2
 *
 * NSRCharsetDetector tries to detect charset of the given text
 * using universalchardet library from Mozilla project, which
 * was adopted and ported for NSRCharsetDetector internal usage.
 *
 * Supported charsets:
 * - Chinese
 * -- ISO-2022-CN
 * -- BIG5
 * -- EUC-TW
 * -- GB18030
 * -- HZ-GB-23121
 * - Cyrillic
 * -- ISO-8859-5
 * -- KOI8-R
 * -- WINDOWS-1251
 * -- MACCYRILLIC
 * -- IBM866
 * -- IBM855
 * - Greek
 * -- ISO-8859-7
 * -- WINDOWS-1253
 * - Hungarian
 * -- ISO-8859-2
 * -- WINDOWS-1250
 * - Hebrew
 * -- ISO-8859-8
 * -- WINDOWS-1255
 * - Japanese
 * -- ISO-2022-JP
 * -- SHIFT_JIS
 * -- EUC-JP
 * - Korean
 * -- ISO-2022-KR
 * -- EUC-KR
 * - Unicode
 * -- UTF-8
 * -- UTF-16BE / UTF-16LE
 * -- UTF-32BE / UTF-32LE / X-ISO-10646-UCS-4-34121 / X-ISO-10646-UCS-4-21431
 * - Others
 * -- WINDOWS-1252
 * -- WINDOWS-874 (Thai)
 */
class NSRCharsetDetector : public nsUniversalDetector
{
public:
	/** @brief Default constructor */
	NSRCharsetDetector ();

	/** @brief Destructor */
	~NSRCharsetDetector ();

	/**
	 * @brief Adds more text data for charset detection
	 * @param bytes Raw text data.
	 * @param len Length of text data.
	 * @since 1.4.2
	 */
	void addData (const QByteArray& bytes, int len);

	/**
	 * @brief Tells detector that no more data is available
	 * @since 1.4.2
	 *
	 * Detector will try to make the best charset guess if not enough
	 * data is collected. Depending on charset detector can fail to
	 * make best guess.
	 */
	void finishData ();

	/**
	 * @brief Checks whether charset is detected or not
	 * @return True if charset was detected, false otherwise.
	 * @since 1.4.2
	 */
	bool isCharsetDetected () const;

	/**
	 * @brief Gets detected charset of the given text
	 * @return Detected charset in case of success, empty string
	 * otherwise.
	 * @note Empty result also means that text can be in ASCII.
	 * @since 1.4.2
	 */
	QString getCharset () const;

	/**
	 * @brief Resets detector state
	 * @since 1.4.2
	 */
	void reset ();

private:
	/* nsUniversalDetector interface */
	/**
	 * @brief Reports about detected charset
	 * @param aCharset Detected charset.
	 * @since 1.4.2
	 */
	void Report (const char* aCharset);

	QString	_charset; /**< Detected charset */
};

#endif /* __NSRCHARSETDETECTOR_H__ */
