#include "nsrcharsetdetector.h"

NSRCharsetDetector::NSRCharsetDetector () : nsUniversalDetector (NS_FILTER_ALL)
{
}

NSRCharsetDetector::~NSRCharsetDetector ()
{
}

void NSRCharsetDetector::addData (const QByteArray &bytes, int len)
{
	len = qMin (len, bytes.size ());
	nsUniversalDetector::HandleData (bytes.constData (), len);
}

void NSRCharsetDetector::finishData ()
{
	nsUniversalDetector::DataEnd ();
}

bool NSRCharsetDetector::isCharsetDetected () const
{
	return !_charset.isEmpty ();
}

QString NSRCharsetDetector::getCharset () const
{
	return _charset;
}

void NSRCharsetDetector::reset ()
{
	nsUniversalDetector::Reset ();
	_charset.clear ();
}

void NSRCharsetDetector::Report (const char *aCharset)
{
	_charset = QString (aCharset);
}
