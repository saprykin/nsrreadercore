#ifndef NSRPAGESCACHE_H_
#define NSRPAGESCACHE_H_

#include "nsrrenderrequest.h"

#include <QObject>
#include <QHash>
#include <QList>

class NSRPagesCache : public QObject
{
	Q_OBJECT
public:
	NSRPagesCache (QObject *parent = 0);
	virtual ~NSRPagesCache ();

	bool isPageExists (int number) const;
	NSRRenderRequest getPage (int number) const;
	void addPage (const NSRRenderRequest &page);
	void removePage (int number);
	void clearStorage ();
	void updatePagePositions (int			number,
				  const QPointF&	pos,
				  const QPointF& 	textPos);
	void removePagesWithoutImages ();
	void removePagesWithImages ();

private:
	QHash<int, NSRRenderRequest>	_hash;
	QList<int>			_pages;
	qint64				_usedMemory;
};

#endif /* NSRPAGESCACHE_H_ */
