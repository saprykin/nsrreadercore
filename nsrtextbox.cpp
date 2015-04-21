#include "nsrtextbox.h"

NSRTextBox::NSRTextBox (const QString& text, const QRectF &bBox) :
	_nextWord (NULL),
	_hasSpaceAfter (false)
{
	_text = text;
	_bBox = bBox;
}

NSRTextBox::~NSRTextBox ()
{
}
