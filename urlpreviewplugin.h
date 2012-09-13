/*
 * urlpreviewplugin.h - plugin
 * Copyright (C) 2009-2012 Isupov Andrey
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef URLPREVIERWPLUGIN_H
#define URLPREVIEWPLUGIN_H

#include "psiplugin.h"
#include "stanzafilter.h"
#include "optionaccessor.h"
#include "activetabaccessor.h"
#include "plugininfoprovider.h"
#include "toolbariconaccessor.h"
#include "applicationinfoaccessor.h"
#include "ui_settings.h"
#include <QSslError>

class OptionAccessingHost;
class ActiveTabAccessingHost;
class ApplicationInfoAccessingHost;
class JuickDownloader;
class QDomDocument;

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;


class UrlPreviewPlugin : public QObject, public PsiPlugin, public OptionAccessor, public ActiveTabAccessor,
			public StanzaFilter, public ApplicationInfoAccessor, public PluginInfoProvider, public ToolbarIconAccessor
{
	Q_OBJECT
	Q_INTERFACES(PsiPlugin OptionAccessor ActiveTabAccessor StanzaFilter
			ApplicationInfoAccessor PluginInfoProvider ToolbarIconAccessor)

public:
    UrlPreviewPlugin();
	virtual QString name() const;
	virtual QString shortName() const;
	virtual QString version() const;
	virtual QWidget* options();
	virtual bool enable();
	virtual bool disable();
	virtual void applyOptions();
	virtual void restoreOptions();
	virtual void setOptionAccessingHost(OptionAccessingHost* host);
	virtual void optionChanged(const QString& ) {}
	virtual void setActiveTabAccessingHost(ActiveTabAccessingHost* host);
	virtual void setApplicationInfoAccessingHost(ApplicationInfoAccessingHost* host);
	virtual QString pluginInfo();
	virtual QList < QVariantHash > getButtonParam() { return QList < QVariantHash >(); }
	virtual QAction* getAction(QObject* parent, int account, const QString& contact);

	virtual bool incomingStanza(int account, const QDomElement& stanza);
	virtual bool outgoingStanza(int , QDomElement& ) { return false; }

private slots:
    void removeWidget();
	void updateWidgets(const QList<QByteArray> &urls);

private slots:
    void netmanFinished(QNetworkReply *);
    void authenticationRequired(QNetworkReply *, QAuthenticator *);
    void netmanSslErrors(QNetworkReply *, const QList<QSslError> &);

private:

    const QRegExp &getLinkRegExp();
    void checkLink(QString &link, QString from, qint64 id);
    void widgetUpdate(QString uid, QString pstr);
    void createCacheDir();
    void clearCache();


private:
	bool enabled;
	OptionAccessingHost* psiOptions;
	ActiveTabAccessingHost* activeTab;
	ApplicationInfoAccessingHost* applicationInfo;

    QNetworkAccessManager *m_netman;

    QSize m_maxImageSize;
    quint64 m_maxFileSize;
    bool m_enableYoutubePreview;
    bool m_enableImagesPreview;
    bool m_enableSitePreview;
    bool m_enableHTML5Audio;
    bool m_enableHTML5Video;

    QString m_template;
    QString m_imageTemplate;
    QString m_youtubeTemplate;
    QString m_siteTemplate;
    QString m_html5AudioTemplate;
    QString m_html5VideoTemplate;

    QPointer<QWidget> optionsWid;
	QList<QWidget*> logs_;
    Ui::UrlPreviewSettingsForm ui_;
	JuickDownloader* downloader_;
};

#endif // URLPREVIEWPLUGIN_H
