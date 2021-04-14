#include "tcpserver.h"
#include "QDebug"
#include "jsonop.h"

#include <QJsonObject>



TcpServer::TcpServer(QObject *parent):QTcpServer(parent)
{
    //直接监听
    if(!listen(QHostAddress::Any,port)){
        close();
    }

}

void TcpServer::setPort(int value)
{
    port = value;
}

void TcpServer::incomingConnection(int sd)
{
    qDebug()<<"发现sd"<<sd;
    //用这种绑定 将 客户端 备份到 服务端 ,可以用来操作客户端 以及根据sd 处理客户端发送的消息
    TcpSocket *client=new TcpSocket(this);
    //??
    addPendingConnection(client);
    client->setSocketDescriptor(sd);
    //client 发送给 server的消息 其实是发给了 server的备份上
    //所以需要 把client的readready给绑定

    //把消息传给mainwindow分发
    connect(client,&TcpSocket::signalsReceivedClientMessage,this,&TcpServer::signalsShowOutMessage);
    // 字典
    sd2Socket[QString::number(sd)]=client;
    curSd=QString::number(sd);
    client->setSd(curSd);
    //设置客户端sd
    slotsSetClientSD(*client,QString::number(sd));
    //断开槽函数
    connect(client,&TcpSocket::disconnected,this,&TcpServer::slotsLeaved);
    qDebug()<<"现在连接的有"<<sd2Socket;
}

QString TcpServer::getCurSd() const
{
    return curSd;
}

QByteArray TcpServer::QString2QByteArray(QString &str)
{
    if(str.size()>=MAX_MESSAGE_SIZE){
       return QByteArray();
    }
    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    //占位符 用于 描述发送消息的大小
    out<<(quint16)0;
    out<<str;
    //覆盖之前的 0 然后 写入大小
    out.device()->seek(0);
    out<<((quint16)(ba.size()-sizeof(quint16)));
    return ba;
}

void TcpServer::slotsSendOne(TcpSocket &ts,QString str)
{
    //用于反向设置socket
    //因为客户端 的 sd 跟 服务端这边不一样的.有毒
    QByteArray ba=QString2QByteArray(str);
    ts.write(ba);

}

void TcpServer::slotsSetClientSD(TcpSocket &ts,QString sd)
{
    QJsonObject json;
    json["messageType"]=MESSAGE_TYPE::REGESTER;
    QJsonObject infoJson;
    infoJson["sd"]=sd;
    json["content"]=infoJson;
    slotsSendOne(ts,JsonOp::JsonToQstring(json));
}

QMap<QString, TcpSocket *> TcpServer::getSd2Socket() const
{
    return sd2Socket;
}

//void TcpServer::slotsWelcome()
//{

//}

void TcpServer::slotsSendEveryOne(QString str)
{
    //有用户进来了要把消息告诉所有人啊
    QByteArray ba=QString2QByteArray(str);
    foreach (QString sd, sd2Socket.keys()) {
//        qDebug()<<sd2Socket[sd];
        sd2Socket[sd]->write(ba);
        //等待数据读入
    }
}

void TcpServer::slotsAcceptClientMessage()
{
    //收到客户端发来的消息啊啊啊
    //用的是 sd 来区分哪个客户端

}

void TcpServer::slotsLeaved()
{
    //       对所有人说 相当于一个广播
       TcpSocket *leaved=qobject_cast<TcpSocket*>(sender());
       leaved->deleteLater();
       sd2Socket.remove(leaved->getSd());
       emit signalsClientLeaved(leaved->getSd());
}

