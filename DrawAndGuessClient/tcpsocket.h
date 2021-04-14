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

    int getGameState() const;
    void setGameState(int value);

private:
    QString message;
    quint16 messageSize;
    const int MAX_MESSAGE_SIZE=65536;
    QString sd; //服务端控制客户端唯一标识符
    int gameState=GAME_STATE::WAIT_DRAWING; //代表 处于wait_状态
signals:
    void signalsShowMessage(QString);
public slots:
    void slotsConnect2Host(QString ip,int port); // 连接服务端 要做的一些事
    void slotsSendMessage(QString); //用于发送 客户端的一些数据 例如名字 等
    void slotsDisconnected(); //断开
private slots:

    void slotsReadMessage();// 读取服务端发来的消息
//    void slotsReadReady(); //用于 收到数据 处理

    
};

#endif // TCPSOCKET_H
