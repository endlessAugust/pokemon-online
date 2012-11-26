#ifndef WEBSERVERPLUGIN_H
#define WEBSERVERPLUGIN_H

#include <QtCore>

#include "WebServerPlugin_global.h"
#include "../Server/plugininterface.h"
#include "../QtWebsocket/QWsServer.h"

class ServerInterface;

extern "C" {
WEBSERVERPLUGINSHARED_EXPORT ServerPlugin * createPluginClass(ServerInterface*);
}

class WEBSERVERPLUGINSHARED_EXPORT WebServerPlugin
    : public QObject, public ServerPlugin
{
    Q_OBJECT

    friend class WebServerConfig;
public:
    WebServerPlugin(ServerInterface *server);
    ~WebServerPlugin();

    QString pluginName() const;
    bool hasConfigurationWidget() const {return true;}

    QWidget *getConfigurationWidget();

public slots:
    void onChatMessage(const QString& message);
    void onServerMessage(const QString& message);
    void dealWithNewConnection();
    void dealWithFrame(const QString& );
    void removeSocket();

signals:
    void sendMessage(const QString &msg);
    void scriptsChanged(const QString&);
    void tiersUpdated();

private:
    ServerInterface *server;
    QWsServer *webserver;

    QSet<QWsSocket*> clients;
    QHash<QString, QVector<int> > attemptsPerIp;

    int port;
    QString pass;

    void broadcast(const QString &);
};

#endif // WEBSERVERPLUGIN_H
