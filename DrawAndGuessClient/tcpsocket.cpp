#include "tcpsocket.h"
#include "QDebug"
TcpSocket::TcpSocket(QObject *parent):QTcpSocket(parent)
{
    connect(this,&TcpSocket::readyRead,this,&TcpSocket::slotsReadMessage);
    connect(this,&TcpSocket::disconnected,this,&TcpSocket::slotsDisconnected);
}

QString TcpSocket::getSd() const
{
    return sd;
}

void TcpSocket::setSd(QString value)
{
    sd = value;
}

int TcpSocket::getGameState() const
{
    return gameState;
}

void TcpSocket::setGameState(int value)
{
    gameState = value;
}

void TcpSocket::slotsConnect2Host(QString ip, int port)
{
    //初始化接收到的大小信息
    messageSize=0;
    //取消已有的连接
    abort();
    connectToHost(ip,port);
}

void TcpSocket::slotsSendMessage(QString str)
{
    // 是用json转过来的str
    //将自身的信息发送给 服务端,再由服务端转发 群发客户端
    //这里要搞一下 发送给server

    //延时啊 不然会堵住..

    if(str.size()<MAX_MESSAGE_SIZE){
        QByteArray ba;
        QDataStream out(&ba,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_9);
        //占位符 用于 描述发送消息的大小
        out<<(quint16)0;
        out<<str;
        //覆盖之前的 0 然后 写入大小
        out.device()->seek(0);
        out<<((quint16)(ba.size()-sizeof(quint16)));
        write(ba);
        waitForBytesWritten();

    }


}

//void TcpSocket::slotsReadReady()
//{
//    //tcpserver其实是用一个另外的socket代表了已经连接的客户端,所以要在这里操作
//    //在客户端发送消息时, 实际上 这个触发的是这个
//}


void TcpSocket::slotsReadMessage()
{

    qDebug()<<bytesAvailable();
    QDataStream in(this);
    //    !设置数据流版本,这里要和服务器端相同
    in.setVersion(QDataStream::Qt_5_9);//如果是刚开始接收数据
    if (messageSize==0){
        //                判断接收的数据是否大于两字节,也就是文件的大小信息所占的空间

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
    //交给mainwindow处理
    messageSize=0;

    emit signalsShowMessage(message);
        message=QString();



    
}
void TcpSocket::slotsDisconnected(){

}

