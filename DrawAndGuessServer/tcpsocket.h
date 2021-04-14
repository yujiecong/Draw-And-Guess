#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QWidget>
#include "global.h"
class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(QObject *parent = Q_NULLPTR);
    QString getSd() const;
    void setSd(QString value);

    void setGameState(int value);

    int getGameState() const;

    QString getName() const;
    void setName(const QString &value);

    QString getIp() const;
    void setIp(const QString &value);

    int getGuessState() const;
    void setGuessState(int value);

signals:
    void signalsReceivedClientMessage(QString);

private slots:
    void slotsDataReceivedFromClient();
    void slotsDisconnected();
private:
    //以下是存储客户端的一些属性值
    //是客户端连接时发送的json信息, 需要解析
    QString name;
    QString ip;
    QString sd;
    int gameState=GAME_STATE::WAIT_DRAWING; //游戏状态
    int guessState=GAME_FAILED;
    QString message;
    quint16 messageSize;
};

#endif // TCPSOCKET_H
