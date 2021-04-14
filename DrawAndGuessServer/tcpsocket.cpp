#include "jsonop.h"
#include "tcpsocket.h"
#include "QDebug"
TcpSocket::TcpSocket(QObject *parent ):QTcpSocket(parent)
{
    //初始化接收到的 message
       messageSize=0;
    connect(this,&TcpSocket::readyRead,this,&TcpSocket::slotsDataReceivedFromClient);
    connect(this,&TcpSocket::disconnected,this,&TcpSocket::slotsDisconnected);
}

QString TcpSocket::getSd() const
{
    return sd;
}

void TcpSocket::slotsDataReceivedFromClient()
{

    //检查字节数
    QDataStream in(this);
    //    !设置数据流版本,这里要和服务器端相同
    in.setVersion(QDataStream::Qt_5_9);//如果是刚开始接收数据

    if (messageSize==0){
        // 判断接收的数据是否大于两字节,也就是文件的大小信息所占的空间

        if(this ->bytesAvailable()<(int)sizeof( quint16))
            return;
        //如果是则保存到blockSize变量中,否则直接返回,继续接收数据
        in >>messageSize;
    }
    //
    //如果没有得到全部的数据,则返回,继续接收数据
    if(this->bytesAvailable() <messageSize)
        return;
    //将接收到的数据存放到变量中
    in >>message;

    messageSize=0;

//    qDebug()<<message.toUtf8().data() ;
    //将收到的数据发送给scene处理

    emit signalsReceivedClientMessage(message);

}

void TcpSocket::slotsDisconnected()
{

//    deleteLater();
}

int TcpSocket::getGuessState() const
{
    return guessState;
}

void TcpSocket::setGuessState(int value)
{
    guessState = value;
}

QString TcpSocket::getIp() const
{
    return ip;
}

void TcpSocket::setIp(const QString &value)
{
    ip = value;
}

QString TcpSocket::getName() const
{
    return name;
}

void TcpSocket::setName(const QString &value)
{
    name = value;
}

int TcpSocket::getGameState() const
{
    return gameState;
}

void TcpSocket::setGameState(int value)
{
    gameState = value;
}

void TcpSocket::setSd(QString value)
{
    sd = value;
}
