#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpsocket.h"

#include <QObject>
#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include "global.h"
class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QObject *parent = Q_NULLPTR);
    void setPort(int value);
    void slotsSendEveryOne(QString str); // 广播给所有人 xx进来了
    QMap<QString, TcpSocket *> getSd2Socket() const;

    void slotsSendOne(TcpSocket &ts, QString str);
    QString getCurSd() const;

signals:
    void signalsShowOutMessage(QString); //显示 客户端信息
    void signalsClientLeaved(QString);
protected:
    void incomingConnection(int sd); //在分配sd时截断 赋值为我们自己的 socket
private:
    int port=PORT;
    const int MAX_MESSAGE_SIZE=65536;
    QMap<QString,TcpSocket *> sd2Socket;
     QString curSd;//最新的一个sd

    QByteArray QString2QByteArray(QString &str);
private slots:

    void slotsSetClientSD(TcpSocket &ts, QString sd);
    void slotsAcceptClientMessage(); //接收来自客户端的消息
    void slotsLeaved(); // 向走了的客户端 解除绑定 顺便告诉所有人


};

#endif // TCPSERVER_H
