#include "nsrthumbnailer.h"
#include "nsrsettings.h"

#include <QSettings>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

#ifdef Q_OS_BLACKBERRY
#  include <bb/utility/ImageConverter>
using namespace bb::utility;
#endif

#define NSR_CORE_THUMBNAILS_DIR 		NSRSettings::getSettingsDirectory () + "/thumbnails"
#define NSR_CORE_THUMBNAILS_MAX_TEXT_PRE	600
#define NSR_CORE_THUMBNAILS_MAX_TEXT_OUT	500

bool
NSRThumbnailer::isThumbnailExists (const QString& path)
{
	return QFile::exists (getThumnailPath (path));
}

void
NSRThumbnailer::saveThumbnail (const QString&		path,
			       const NSRRenderRequest&	page)
{
	QDir dir;

	if (!dir.exists (NSR_CORE_THUMBNAILS_DIR))
		dir.mkpath (NSR_CORE_THUMBNAILS_DIR);

	QSettings settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	if (!QFile::exists (path))
		return;

	QString hash = filePathToHash (path);

	if (page.getImage().isValid ()) {
		QString fileName = getThumbnailPathFromHash (hash);

#ifdef Q_OS_BLACKBERRY
		ImageConverter::encode (QUrl::fromLocalFile (fileName),
					page.getImage (),
					50);
#else
		page.getImage().save (fileName);
#endif
	}

	QString pageText = page.getText().left (NSR_CORE_THUMBNAILS_MAX_TEXT_PRE);
	int lastIndex = pageText.lastIndexOf (QRegExp ("\\S\\s"));

	/* Do not truncate the whole string */
	if (lastIndex > NSR_CORE_THUMBNAILS_MAX_TEXT_OUT)
		pageText = pageText.left (lastIndex + 1) + "...";

	settings.beginGroup (hash);
	settings.setValue ("path", path);
	settings.setValue ("text", pageText);
	settings.setValue ("encrypted", false);
	settings.setValue ("file-size", QFileInfo (path).size ());
	settings.endGroup ();
	settings.sync ();
}

void
NSRThumbnailer::saveThumbnailEncrypted (const QString&	path)
{
	QDir dir;

	if (!dir.exists (NSR_CORE_THUMBNAILS_DIR))
		dir.mkpath (NSR_CORE_THUMBNAILS_DIR);

	QSettings settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	if (!QFile::exists (path))
		return;

	QString hash = filePathToHash (path);

	settings.beginGroup (hash);
	settings.setValue ("path", path);
	settings.setValue ("encrypted", true);
	settings.setValue ("file-size", QFileInfo (path).size ());
	settings.remove ("text");
	settings.endGroup ();
	settings.sync ();

	QFile::remove (getThumbnailPathFromHash (hash));
}

QString
NSRThumbnailer::getThumnailPath (const QString& path)
{
	return getThumbnailPathFromHash (filePathToHash (path));
}

void
NSRThumbnailer::cleanOldFiles ()
{
	QSettings settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	QStringList files = settings.childGroups ();
	int count = files.count ();

	for (int i = 0; i < count; ++i) {
		settings.beginGroup (files.at (i));

		bool needDelete = false;
		QString filePath = settings.value("path").toString ();

		if (!QFile::exists (filePath)) {
			QFile::remove (getThumnailPath (filePath));
			needDelete = true;
		}

		settings.endGroup ();

		if (needDelete)
			settings.remove (files.at (i));
	}

	settings.endGroup ();
}

QString
NSRThumbnailer::getThumbnailText (const QString& path)
{
	QSettings settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	return settings.value(filePathToHash (path) + "/text", "").toString ();
}

bool
NSRThumbnailer::isThumbnailEncrypted (const QString& path)
{
	QSettings settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	return settings.value(filePathToHash (path) + "/encrypted", false).toBool ();
}

void
NSRThumbnailer::removeThumbnail (const QString& path)
{
	QSettings settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);
	QString hash = filePathToHash (path);

	settings.remove (hash);
	QFile::remove (getThumbnailPathFromHash (hash));
}

bool
NSRThumbnailer::isThumbnailOutdated (const QString& path)
{
	if (!QFile::exists (path))
		return false;

	QSettings	settings (NSR_CORE_THUMBNAILS_DIR + "/thumbnails.ini",
			    	  QSettings::IniFormat);
	QFileInfo	fileInfo (path);
	QString		hash = filePathToHash (path);
	QString		filePath = getThumbnailPathFromHash (hash);
	qint64		fileSize = fileInfo.size ();
	qint64		storedFileSize = settings.value(hash + "/file-size", -1).toInt ();

	if (storedFileSize == -1) {
		settings.setValue (hash + "/file-size", fileSize);
		storedFileSize = fileSize;
	}

	return (!QFile::exists (filePath) || fileSize != storedFileSize ||
		fileInfo.lastModified () > QFileInfo(filePath).lastModified ());
}

QString
NSRThumbnailer::filePathToHash (const QString& path)
{
	return QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();
}

QString
NSRThumbnailer::getThumbnailPathFromHash (const QString& hash)
{
	return NSR_CORE_THUMBNAILS_DIR + "/" + hash + ".png";
}
