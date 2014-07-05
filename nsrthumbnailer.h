#ifndef NSRTHUMBNAILER_H_
#define NSRTHUMBNAILER_H_

#include "nsrrenderedpage.h"

#include <QString>
#include <QSettings>

class NSRThumbnailer : public QSettings
{
	Q_OBJECT
public:
	static NSRThumbnailer * instance ();
	static void release ();

	bool isThumbnailExists (const QString& path);
	void saveThumbnail (const QString&		path,
			    const NSRRenderedPage&	page);
	void saveThumbnailEncrypted (const QString& path);
	QString getThumbnailText (const QString& path);
	QString getThumbnailPath (const QString& path);
	bool isThumbnailEncrypted (const QString& path);
	void cleanOldFiles ();
	void removeThumbnail (const QString& path);
	bool isThumbnailOutdated (const QString& path);

private:
	QString filePathToHash (const QString& path);
	QString getThumbnailPathFromHash (const QString& hash);

	NSRThumbnailer ();
	virtual ~NSRThumbnailer () {}

	static NSRThumbnailer *	_instance;
	QString			_configDir;
};

#endif /* NSRTHUMBNAILER_H_ */
