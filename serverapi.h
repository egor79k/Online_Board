#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QGraphicsItem>
#include <QString>

#include "serializers.h"


class ServerApi : public QObject
{
    Q_OBJECT

private:
    static struct ServerConfig
    {
        const QString ip_address = "localhost";
        const quint16 port = 5555;
        const int connect_timeout = 10000;
    } config;

    struct ClientProps : public Serializable {
    public:
        bool serialize(QJsonObject& json) const override;
        bool deserialize(const QJsonObject& json) override;

        int client_id;
    };

public:
    explicit ServerApi(QObject* parent = nullptr);

    ServerApi(ServerApi&& other) = delete;
    ServerApi(const ServerApi& other) = delete;
    ServerApi& operator=(const ServerApi& other) = delete;

    static ServerApi* getInstance();

    static bool connectToServer();
    //static QByteArray readData();
    static void sAddLayer(const Serializer& argument);

    static QTcpSocket::SocketError lastError();
    static QString lastErrorStr();

signals:
    //void dataReceived();
    void cInitClient(const Serializer& argument);
    void cAddLayer(const Serializer& argument);

private slots:
    void onReadyRead();
    void onInitClient(const Serializer& argument);
    void onDisconnected();

private:
    bool sendMethod(const QString& method, const Serializer& argument);

    static ServerApi* instance;

    QTcpSocket* socket = nullptr;
    ClientProps props;

    const QMap<QString, void (ServerApi::*)(const Serializer&)> str_to_signal {
        { "init_client", &ServerApi::cInitClient },
        { "c_add_layer", &ServerApi::cAddLayer }
    };
};

#endif // SERVERAPI_H
