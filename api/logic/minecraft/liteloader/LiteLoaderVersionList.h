/* Copyright 2013-2015 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <QObject>

#include <QString>
#include <QStringList>
#include "BaseVersion.h"
#include "BaseVersionList.h"
#include "tasks/Task.h"
#include "net/NetJob.h"
#include <minecraft/Library.h>
#include <minecraft/VersionFile.h>

#include "multimc_logic_export.h"

class LLListLoadTask;
class QNetworkReply;

class LiteLoaderVersion : public BaseVersion
{
public:
	QString descriptor() override
	{
		if (isLatest)
		{
			return QObject::tr("Latest");
		}
		return QString();
	}
	QString typeString() const override
	{
		return mcVersion;
	}
	QString name() override
	{
		return version;
	}
	VersionFilePtr getVersionFile()
	{
		auto f = std::make_shared<VersionFile>();
		f->mainClass = "net.minecraft.launchwrapper.Launch";
		f->addTweakers += tweakClass;
		f->order = 10;
		f->libraries = libraries;
		auto liteloaderLib = std::make_shared<Library>("com.mumfrey:liteloader:" + version);
		liteloaderLib->setAbsoluteUrl(QString("http://dl.liteloader.com/versions/com/mumfrey/liteloader/%1/%2").arg(mcVersion, file));
		f->libraries.append(liteloaderLib);
		f->name = "LiteLoader";
		f->fileId = "com.mumfrey.liteloader";
		f->version = version;
		f->minecraftVersion = mcVersion;
		return f;
	}

	// important info
	QString version;
	QString file;
	QString mcVersion;
	QString md5;
	int timestamp;
	bool isLatest;
	QString tweakClass;
	QList<LibraryPtr> libraries;

	// meta
	QString defaultUrl;
	QString description;
	QString authors;
};
typedef std::shared_ptr<LiteLoaderVersion> LiteLoaderVersionPtr;

class MULTIMC_LOGIC_EXPORT LiteLoaderVersionList : public BaseVersionList
{
	Q_OBJECT
public:
	friend class LLListLoadTask;

	explicit LiteLoaderVersionList(QObject *parent = 0);

	virtual Task *getLoadTask();
	virtual bool isLoaded();
	virtual const BaseVersionPtr at(int i) const;
	virtual int count() const;
	virtual void sortVersions();
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QList< ModelRoles > providesRoles();

	virtual BaseVersionPtr getLatestStable() const;

protected:
	QList<BaseVersionPtr> m_vlist;

	bool m_loaded = false;

protected
slots:
	virtual void updateListData(QList<BaseVersionPtr> versions);
};

class LLListLoadTask : public Task
{
	Q_OBJECT

public:
	explicit LLListLoadTask(LiteLoaderVersionList *vlist);
	~LLListLoadTask();

	virtual void executeTask();

protected
slots:
	void listDownloaded();
	void listFailed();

protected:
	NetJobPtr listJob;
	Net::Download::Ptr listDownload;
	LiteLoaderVersionList *m_list;
};

Q_DECLARE_METATYPE(LiteLoaderVersionPtr)