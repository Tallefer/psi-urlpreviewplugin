/*
 * juickplugin.cpp - plugin
 * Copyright (C) 2009-2012 Kravtsov Nikolai, Khryukin Evgeny
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

#include <QDomElement>
#include <QMessageBox>
#include <QColorDialog>
#include <QTextEdit>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>

#include "optionaccessinghost.h"
#include "activetabaccessinghost.h"
#include "applicationinfoaccessinghost.h"

#include "urlpreviewplugin.h"
#include "juickdownloader.h"
#include "defines.h"



//static void debugElement(const QDomElement& e)
//{
//	QString out;
//	QTextStream str(&out);
//	e.save(str, 3);
//	qDebug() << out;
//}


static void nl2br(QDomElement *body,QDomDocument* e, const QString& msg)
{
	foreach (const QString& str, msg.split("\n")) {
		body->appendChild(e->createTextNode(str));
		body->appendChild(e->createElement("br"));
	}
	body->removeChild(body->lastChild());
}

//-----------------------------
//------UrlPreviewPlugin-------
//-----------------------------
Q_EXPORT_PLUGIN(UrlPreviewPlugin)

UrlPreviewPlugin::UrlPreviewPlugin():
    m_netman(new QNetworkAccessManager(this))

    , enabled(false)
	, psiOptions(0), activeTab(0), applicationInfo(0)
    , m_enableHTML5Audio(true)
    , m_enableHTML5Video (true)
    , m_enableImagesPreview (true)
    , m_enableYoutubePreview (true)
    , m_enableSitePreview (true)
    , m_maxFileSize(100000)
    , m_maxImageSize(QSize(800,600))
	, downloader_(0)

{
    m_template = "<br><b>" % tr("URL Preview") % "</b>: <i>%TYPE%, %SIZE% " % tr("bytes") % "</i><br>";
    m_imageTemplate = "<img src=\"%URL%\" style=\"display: none;\" "
                                     "onload=\"if (this.width>%MAXW%) this.style.width='%MAXW%px'; "
                                     "if (this.height>%MAXH%) { this.style.width=''; this.style.height='%MAXH%px'; } "
                                     "this.style.display='';\"><br>";
    m_youtubeTemplate =	"<img src=\"http://img.youtube.com/vi/%YTID%/1.jpg\">"
                                       "<img src=\"http://img.youtube.com/vi/%YTID%/2.jpg\">"
                                       "<img src=\"http://img.youtube.com/vi/%YTID%/3.jpg\"><br>";
    m_siteTemplate =	"<br><img src=\"%URL%\"><br>";

    m_html5AudioTemplate = "<audio controls=\"controls\" preload=\"none\"><source src=\"%AUDIOURL%\" type=\"%FILETYPE%\"/>" % tr("Something went wrong.") % "</audio>";

    m_html5VideoTemplate = "<video controls=\"controls\" preload=\"none\"><source src=\"%VIDEOURL%\" type=\"%VIDEOTYPE%\" />" % tr("Something went wrong.") % "</video>";



    connect(m_netman, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*))
                );
    connect(m_netman, SIGNAL(finished(QNetworkReply*)),
                SLOT(netmanFinished(QNetworkReply*))
                );
    connect(m_netman, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                SLOT(netmanSslErrors(QNetworkReply*,QList<QSslError>))
                );
}

QString UrlPreviewPlugin::name() const
{
	return constPluginName;
}

QString UrlPreviewPlugin::shortName() const
{
    return "urlpreview";
}

QString UrlPreviewPlugin::version() const
{
	return constVersion;
}

QString UrlPreviewPlugin::pluginInfo()
{
    return tr("Authors: ") + "Eraser <eraser1981@gmail.com>\n\n"
    + trUtf8("Url preview link");
}

QWidget* UrlPreviewPlugin::options()
{
	if (!enabled) {
		return 0;
	}
	optionsWid = new QWidget();
	ui_.setupUi(optionsWid);

	restoreOptions();

	return optionsWid;
}

bool UrlPreviewPlugin::enable()
{
	enabled = true;

    psiOptions->setPluginOption(constVersionOpt, constVersion);
    createCacheDir();

     m_maxImageSize = psiOptions->getPluginOption(constmaxImageSize,m_maxImageSize).toSize();
     m_maxFileSize = psiOptions->getPluginOption(constmaxFileSize,m_maxFileSize).toDouble();
     m_enableYoutubePreview = psiOptions->getPluginOption(constenableYoutubePreview, m_enableYoutubePreview).toBool();
     m_enableImagesPreview = psiOptions->getPluginOption(constenableImagesPreview, m_enableImagesPreview).toBool();
     m_enableSitePreview = psiOptions->getPluginOption(constenableSitePreview, m_enableSitePreview).toBool();
     m_enableHTML5Audio = psiOptions->getPluginOption(constenableHTML5Audio, m_enableHTML5Audio).toBool();
     m_enableHTML5Video = psiOptions->getPluginOption(constenableHTML5Video, m_enableHTML5Video).toBool();

    applicationInfo->getProxyFor(constPluginName); // init proxy settings for plugin

    Proxy appprx = applicationInfo->getProxyFor(constPluginName);

    QNetworkProxy prx;

    if(!appprx.host.isEmpty()) {
        prx.setType(QNetworkProxy::HttpCachingProxy);
        if(appprx.type == "socks")
            prx.setType(QNetworkProxy::Socks5Proxy);
        prx.setPort(appprx.port);
        prx.setHostName(appprx.host);
        if(!appprx.user.isEmpty()) {
            prx.setUser(appprx.user);
            prx.setPassword(appprx.pass);
        }
    }

    m_netman->setProxy(prx);

	downloader_ = new JuickDownloader(applicationInfo, this);
	connect(downloader_, SIGNAL(finished(QList<QByteArray>)), SLOT(updateWidgets(QList<QByteArray>)));

	return true;
}

bool UrlPreviewPlugin::disable()
{
	enabled = false;
	logs_.clear();

    clearCache();

	downloader_->disconnect();
	downloader_->deleteLater();
	downloader_ = 0;

	return true;
}

void UrlPreviewPlugin::createCacheDir()
{
    QDir dir(applicationInfo->appHomeDir(ApplicationInfoAccessingHost::CacheLocation));
    dir.mkpath("urlpreview");
    if (!dir.exists("urlpreview"))
    {
        QMessageBox::warning(0, tr("Warning"),tr("can't create folder %1 \ncaching avatars will be not available")
                     .arg(applicationInfo->appHomeDir(ApplicationInfoAccessingHost::CacheLocation)+"/urlpreview"));
    }
}
void UrlPreviewPlugin::clearCache()
{
    QDir dir(applicationInfo->appHomeDir(ApplicationInfoAccessingHost::CacheLocation)+"/urlpreview");
    foreach(const QString& file, dir.entryList(QDir::Files)) {
        QFile::remove(dir.absolutePath()+"/"+file);
    }
}

void UrlPreviewPlugin::applyOptions()
{
	if (!optionsWid)
		return;


    m_maxImageSize.setHeight(ui_.maxHeight->value());
    m_maxImageSize.setWidth(ui_.maxWidth->value());
    m_maxFileSize = ui_.maxFileSize->value();
    m_enableYoutubePreview = ui_.youtubePreview->isChecked();
    m_enableImagesPreview = ui_.imagesPreview->isChecked();
    m_enableSitePreview = ui_.sitePreview->isChecked();
    m_enableHTML5Audio = ui_.HTML5Audio->isChecked();
    m_enableHTML5Video = ui_.HTML5Video->isChecked();


    psiOptions->setPluginOption(constmaxImageSize, m_maxImageSize);
    psiOptions->setPluginOption(constmaxFileSize, m_maxFileSize);
    psiOptions->setPluginOption(constenableYoutubePreview, m_enableYoutubePreview);
    psiOptions->setPluginOption(constenableImagesPreview, m_enableImagesPreview);
    psiOptions->setPluginOption(constenableSitePreview, m_enableSitePreview);
    psiOptions->setPluginOption(constenableHTML5Audio, m_enableHTML5Audio);
    psiOptions->setPluginOption(constenableHTML5Video, m_enableHTML5Video);

}

void UrlPreviewPlugin::restoreOptions()
{
	if (!optionsWid)
		return;

        ui_.maxFileSize->setValue(m_maxFileSize);
        ui_.maxWidth->setValue(m_maxImageSize.width());
        ui_.maxHeight->setValue(m_maxImageSize.height());
        ui_.youtubePreview->setChecked(m_enableYoutubePreview);
        ui_.imagesPreview->setChecked(m_enableImagesPreview);
        ui_.sitePreview->setChecked(m_enableSitePreview);
        ui_.HTML5Audio->setChecked(m_enableHTML5Audio);
        ui_.HTML5Video->setChecked(m_enableHTML5Video);

}


void UrlPreviewPlugin::setOptionAccessingHost(OptionAccessingHost* host)
{
	psiOptions = host;
}

void UrlPreviewPlugin::setActiveTabAccessingHost(ActiveTabAccessingHost* host)
{
	activeTab = host;
}

void UrlPreviewPlugin::setApplicationInfoAccessingHost(ApplicationInfoAccessingHost* host)
{
	applicationInfo = host;
}

const QRegExp &UrlPreviewPlugin::getLinkRegExp()
{
#if 0
    static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
                              "(([a-zA-Z]+://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
                              Qt::CaseInsensitive);
#else
    static QRegExp linkRegExp("([a-z]+(\\+[a-z]+)?://|www\\.)"
                              "[\\w-]+(\\.[\\w-]+)*\\.\\w+"
                              "(:\\d+)?"
                              "(/[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*"
                              "(\\?[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*)?"
                              "(#[\\w\\+\\.\\[\\]!%\\$/\\\\\\(\\)\\|,:;@\\'&=~-]*)?)?",
                              Qt::CaseInsensitive, QRegExp::RegExp2);
#endif
    Q_ASSERT(linkRegExp.isValid());
    return linkRegExp;
}



bool UrlPreviewPlugin::incomingStanza(int /*account*/, const QDomElement& stanza)
{
	if(!enabled)
		return false;

	if (stanza.tagName() == "message" ) {
		const QString jid(stanza.attribute("from").split('/').first());

        QDomDocument doc = stanza.ownerDocument();
        QDomElement nonConstStanza = const_cast<QDomElement&>(stanza);

        //—оздаем xhtml-im элемент
        QDomElement element =  doc.createElement("html");
        element.setAttribute("xmlns","http://jabber.org/protocol/xhtml-im");
        QDomElement body = doc.createElement("body");
        body.setAttribute("xmlns","http://www.w3.org/1999/xhtml");

        QString html = nonConstStanza.firstChildElement("body").text();
                QString newHtml;
                const QRegExp &linkRegExp = getLinkRegExp();
                int pos = 0;
                int lastPos = 0;
                while (((pos = linkRegExp.indexIn(html, pos)) != -1)) {
                    html.midRef(lastPos, pos - lastPos).appendTo(&newHtml);
                    static int uid = 1;
                    QString link = linkRegExp.cap(0);
                    const QString oldLink = link;
                    //newHtml += link;

                    nl2br(&body, &doc, newHtml);
                    newHtml = "";

                    body.appendChild(doc.createElement("br"));
                    QDomElement ahref = doc.createElement("a");
                    ahref.setAttribute("href",link);
                    ahref.appendChild(doc.createTextNode(link));
                    body.appendChild(ahref);

                    QDomElement span = doc.createElement("span");
                    span.setAttribute("id",QString::fromLatin1("urlpreview%1").arg(uid));
                    span.appendChild(doc.createTextNode(" Url checking..."));
                    body.appendChild(span);

                    checkLink(link, jid, uid);

                    uid++;
                    pos = lastPos = pos + oldLink.size();
                }
                html.midRef(lastPos, html.size() - lastPos).appendTo(&newHtml);
                html = newHtml;
                nl2br(&body, &doc, newHtml);

            element.appendChild(body);
            nonConstStanza.appendChild(element);
	}
	return false;
}

void UrlPreviewPlugin::checkLink(QString &link, QString from, qint64 id)
{
    const QString oldLink = link;
    const char *entitiesIn[] = { "&quot;", "&gt;", "&lt;", "&amp;" };
    const char *entitiesOut[] = { "\"", ">", "<", "&" };
    const int entitiesCount = sizeof(entitiesIn) / sizeof(entitiesIn[0]);

    for (int i = 0; i < entitiesCount; ++i) {
        link.replace(QLatin1String(entitiesIn[i]),
                     QLatin1String(entitiesOut[i]),
                     Qt::CaseInsensitive);
    }

    //TODO may be need refinement
    if (link.toLower().startsWith("www."))
        link.prepend("http://");
    link = QUrl::fromEncoded(link.toUtf8()).toString();
    link.replace(" ", QLatin1String("%20"));

    static QRegExp urlrx("youtube\\.com/watch\\?v\\=([^\\&]*)");
    Q_ASSERT(urlrx.isValid());
    if (urlrx.indexIn(link)>-1) {
        link = QLatin1String("http://www.youtube.com/v/") + urlrx.cap(1);
    }

    QString uid = QString::number(id);

    QNetworkRequest request;
    request.setUrl(QUrl(link));
    request.setRawHeader("Ranges", "bytes=0-0");
    QNetworkReply *reply = m_netman->head(request);
    reply->setProperty("uid", uid);
    reply->setProperty("unit", from);
}

// Ќа самом деле мы никаких акшенов не добавл€ем.
// «десь мы просто ищем и сохран€ем список уже открытых
// чатов с juick
QAction* UrlPreviewPlugin::getAction(QObject *parent, int /*account*/, const QString &contact)
{
	const QString jid = contact.split("/").first();
	const QString usernameJ = jid.split("@").first();
        QWidget* log = parent->findChild<QWidget*>("log");
        if(log) {
            logs_.append(log);
            connect(log, SIGNAL(destroyed()), SLOT(removeWidget()));
        }
	return 0;
}

void UrlPreviewPlugin::removeWidget()
{
	QWidget* w = static_cast<QWidget*>(sender());
	logs_.removeAll(w);
}

// Ётот слот обновл€ет чатлоги, чтобы они перезагрузили
// картинки с диска.
void UrlPreviewPlugin::updateWidgets(const QList<QByteArray>& urls)
{
	foreach(QWidget *w, logs_) {
		QTextEdit* te = qobject_cast<QTextEdit*>(w);
		if(te) {
			QTextDocument* td = te->document();
			foreach(const QByteArray& url, urls) {
				QUrl u(url);
				td->addResource(QTextDocument::ImageResource, u, QPixmap(u.toLocalFile()));
			}
//			td->adjustSize();
//			te->update();
			te->setLineWrapColumnOrWidth(te->lineWrapColumnOrWidth());
		}
		else {
			QWebView *wv = w->findChild<QWebView*>();
			if(wv) {
				int t = qrand()%(QTime::currentTime().msec() + 1);
				QWebFrame* wf = wv->page()->mainFrame();
				foreach(const QByteArray& url, urls) {
					QUrl u(url);
					QWebElement elem = wf->findFirstElement(QString("img[src=\"%1\"]").arg(u.toString()));
					if(!elem.isNull()) {
						elem.setAttribute("src", u.toString() + "?" + QString::number(++t));
					}
				}
			}
		}
	}
}

void UrlPreviewPlugin::widgetUpdate(QString uid, QString pstr)
{
QString js = "urlpreview"+uid+".innerHTML = \""+pstr.replace("\"", "\\\"")+"\";";
foreach(QWidget *w, logs_) {
    QTextEdit* te = qobject_cast<QTextEdit*>(w);
    if(te) {
    }
    else {
        QWebView *wv = w->findChild<QWebView*>();
        if(wv) {
            int t = qrand()%(QTime::currentTime().msec() + 1);
            QWebFrame* wf = wv->page()->mainFrame();
            QMetaObject::invokeMethod(wf, "evaluateJavaScript", Q_ARG(QString, js));

        }
    }
}
}


void UrlPreviewPlugin::netmanFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QString url = reply->url().toString();
    QString uid = reply->property("uid").toString();
    QByteArray typeheader;
    QString decodedUrl = QUrl::fromEncoded(url.toUtf8()).toString();
    QString type;
    QByteArray sizeheader;
    quint64 size=0;
    QRegExp hrx; hrx.setCaseSensitivity(Qt::CaseInsensitive);
    foreach (QString header, reply->rawHeaderList()) {
        if (type.isEmpty()) {
            hrx.setPattern("^content-type$");
            if (hrx.indexIn(header)==0) typeheader = header.toAscii();
        }
        if (sizeheader.isEmpty()) {
            hrx.setPattern("^content-range$");
            if (hrx.indexIn(header)==0) sizeheader = header.toAscii();
        }
        if (sizeheader.isEmpty()) {
            hrx.setPattern("^content-length$");
            if (hrx.indexIn(header)==0) sizeheader = header.toAscii();
        }
    }
    if (!typeheader.isEmpty()) {
        hrx.setPattern("^([^\\;]+)");
        if (hrx.indexIn(reply->rawHeader(typeheader))>=0)
            type = hrx.cap(1);
    }
    if (!sizeheader.isEmpty()) {
        hrx.setPattern("(\\d+)");
        if (hrx.indexIn(reply->rawHeader(sizeheader))>=0)
            size = hrx.cap(1).toInt();
    }

    //debug() << url << reply->rawHeaderList() << type;
    if (type.isNull())
    {
        widgetUpdate(uid," Type unknown");
        return;
    }


    QString pstr;
    bool showPreviewHead = true;
    QRegExp typerx("^text/html");
    if (type.contains(typerx))
    {
        showPreviewHead = false;
    }

    QString localUrl;


    static QRegExp urlrx("^http://www\\.youtube\\.com/v/([\\w\\-]+)");
    if (urlrx.indexIn(url)==0 && m_enableYoutubePreview) {
        pstr = m_template;
        if (type == QLatin1String("application/x-shockwave-flash")) {
            showPreviewHead = false;
            pstr.replace("%TYPE%", tr("YouTube video"));
            pstr += m_youtubeTemplate;
            pstr.replace("%YTID%", urlrx.cap(1));
            pstr.replace("%SIZE%", tr("Unknown"));
        } else {
            pstr.replace("%SIZE%", QString::number(size));
        }
    }


    if (( ( (type == QLatin1String("audio/ogg") || type == QLatin1String("audio/mpeg")) || type == QLatin1String("application/ogg")) || type == QLatin1String("audio/x-wav") ) && m_enableHTML5Audio) {
                pstr = m_template;
                showPreviewHead = false;
                pstr.replace("%TYPE%", tr("HTML5 Audio"));
                pstr += m_html5AudioTemplate;
                if (type == QLatin1String("application/ogg")) {
                    pstr.replace("%FILETYPE%", "audio/ogg");
                } else {
                    pstr.replace("%FILETYPE%", type);
                }
                pstr.replace("%AUDIOURL%", url);
                pstr.replace("%SIZE%", QString::number(size));
            }

    if (((type == QLatin1String("video/webm") || type == QLatin1String("video/ogg")) || type == QLatin1String("video/mp4")) && m_enableHTML5Video) {
                pstr = m_template;
                showPreviewHead = false;
                pstr.replace("%TYPE%", tr("HTML5 Video"));
                pstr += m_html5VideoTemplate;
                pstr.replace("%VIDEOTYPE%", type);
                pstr.replace("%VIDEOURL%", url);
                pstr.replace("%SIZE%", QString::number(size));
            }


    if (showPreviewHead) {
        QString sizestr = size ? QString::number(size) : tr("Unknown");
        pstr = m_template;
        pstr.replace("%TYPE%", type);
        pstr.replace("%SIZE%", sizestr);
    }


    if (!showPreviewHead && m_enableSitePreview)
    {
        const QUrl photoUrl(url);
        const QDir dir(applicationInfo->appHomeDir(ApplicationInfoAccessingHost::CacheLocation)+"/urlpreview");
        const QString path(QString("%1/%2").arg(dir.absolutePath()).arg(uid));
        //url = QString("http://get.thumbshots.ru/?url=%1&size=L").arg(url);
        url = QString("http://mini.s-shot.ru/?%1").arg(url);
        JuickDownloadItem it(path, url);
        downloader_->get(it);
        QString imgdata = uid;
        localUrl = QString(QUrl::fromLocalFile(QString("%1/%2").arg(dir.absolutePath()).arg(imgdata)).toEncoded());

        QString amsg = m_siteTemplate;
        amsg.replace("%URL%", localUrl);
        pstr += amsg;
    }

    typerx.setPattern("^image/");
    if (type.contains(typerx) && 0 < size && size < m_maxFileSize && m_enableImagesPreview) {

        const QUrl photoUrl(url);
        const QDir dir(applicationInfo->appHomeDir(ApplicationInfoAccessingHost::CacheLocation)+"/urlpreview");
        const QString path(QString("%1/%2").arg(dir.absolutePath()).arg(photoUrl.path().replace("/", "%")));
        JuickDownloadItem it(path, url);
        downloader_->get(it);
        QString imgdata = photoUrl.path().replace("/", "%");
        localUrl = QString(QUrl::fromLocalFile(QString("%1/%2").arg(dir.absolutePath()).arg(imgdata)).toEncoded());

        QString amsg = m_imageTemplate;
        amsg.replace("%URL%", localUrl);
        amsg.replace("%UID%", uid);
        amsg.replace("%MAXW%", QString::number(m_maxImageSize.width()));
        amsg.replace("%MAXH%", QString::number(m_maxImageSize.height()));
        pstr += amsg;
    }

    widgetUpdate(uid,pstr);


}

void UrlPreviewPlugin::authenticationRequired(QNetworkReply *, QAuthenticator *)
{

}

void UrlPreviewPlugin::netmanSslErrors(QNetworkReply *, const QList<QSslError> &)
{

}
