/*
 * Copyright 2013 Canonical Ltd.
 *
 * This file is part of webbrowser-app.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WEBAPP_CONTAINER_H__
#define __WEBAPP_CONTAINER_H__

#include "browserapplication.h"

#include "webapp-container-helper.h"

// Qt
#include <QString>
#include <QStringList>
#include <QScopedPointer>

class IntentFilter;
class QQmlContext;

class WebappContainer : public BrowserApplication
{
    Q_OBJECT

public:
    WebappContainer(int& argc, char** argv);

    bool initialize();

protected:
    void qmlEngineCreated(QQmlEngine *);
    virtual QList<QUrl> urls() const;

private:
    virtual void printUsage() const;
    void earlyEnvironment();
    void parseCommandLine();
    void parseExtraConfiguration();
    QString getExtraWebappUrlPatterns() const;
    bool isValidLocalApplicationRunningContext() const;
    bool isValidLocalResource(const QString& resourceName) const;
    bool shouldNotValidateCommandLineUrls() const;
    bool isValidLocalIntentFilterFile(const QString& filename) const;
    void setupLocalIntentFilterIfAny(QQmlContext* context);

private:
    QString m_webappName;
    QString m_webappModelSearchPath;
    QStringList m_webappUrlPatterns;
    QString m_accountProvider;
    bool m_storeSessionCookies;
    bool m_backForwardButtonsVisible;
    bool m_addressBarVisible;
    bool m_localWebappManifest;
    QString m_popupRedirectionUrlPrefixPattern;
    QString m_localCookieStoreDbPath;
    QString m_userAgentOverride;
    QScopedPointer<WebappContainerHelper> m_webappContainerHelper;
    QScopedPointer<IntentFilter> m_intentFilter;

    static const QString URL_PATTERN_SEPARATOR;
    static const QString LOCAL_INTENT_FILTER_FILENAME;
};

#endif // __WEBAPP_CONTAINER_H__
