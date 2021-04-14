#include "loginwindow.h"

#include "jsonop.h"
#include "ui_loginwindow.h"
#include "global.h"
#include <QMessageBox>
#include <QTimer>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
//    QTimer::singleShot(1000,this,&LoginWindow::on_tryConnBtn_clicked);
//    on_tryConnBtn_clicked();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_tryConnBtn_clicked()
{
    //创建一个 socket开始连接
    name=ui->nameInput->text();
    if(tcpsocket==nullptr){
        tcpsocket=new TcpSocket();
        // 告诉 主控 开始创建socket
         emit signalsCreateTcpSocket();
        //连接成功后发送信息给 服务端,用于初始化注册信息
        connect(tcpsocket,&TcpSocket::connected,this,&LoginWindow::slotsSetInfo2Socket);
        // 断开时要做的事
        connect(tcpsocket,&TcpSocket::disconnected,this,&LoginWindow::slotsSocketDisconnected);
        tcpsocket->slotsConnect2Host(ui->ipInput->text(),ui->portInput->text().toInt());


        //如果在1s内没连接成功
        //Note: This function may fail randomly on Windows. Consider using the event loop and the connected() signal if your software will run on Windows.
        if(tcpsocket->waitForConnected(1000)){
            qDebug()<<"连接成功";
            hide();
            //等待数据发过来
            //等待回应
        }
        else{
            qDebug()<<tcpsocket->error();
            tcpsocket=nullptr;
            //连接失败时释放disconnected
        }
    }

}

void LoginWindow::slotsSetInfo2Socket()
{
    //名字
    qDebug()<<tcpsocket->waitForReadyRead(1000);
    QJsonObject json;
    QJsonObject infoJson;
    infoJson.insert("name",name);
    infoJson["ip"]=ui->ipInput->text();
    infoJson["sd"]=tcpsocket->getSd();
    json["content"]=infoJson;
    json["messageType"]=MESSAGE_TYPE::NEW_CLIENT_BROADCAST;
    //构建成json 转 string 传输
    tcpsocket->slotsSendMessage(JsonOp::JsonToQstring(json));

}

void LoginWindow::slotsSocketDisconnected()
{
    //可以尝试再访问,即心跳检测,不过这里先断开
    tcpsocket->disconnectFromHost();
    tcpsocket->deleteLater();
    tcpsocket=nullptr;
    show();
    QTimer *timer=new QTimer();
    timer->setInterval(100);
    connect(timer,&QTimer::timeout,[=](){
         on_tryConnBtn_clicked();
         if (tcpsocket!=nullptr){
             timer->deleteLater();
         }
    });
    timer->start();
    emit signalsDisconnected();
}

QString LoginWindow::getName() const
{
    return name;
}


TcpSocket *LoginWindow::getTcpsocket() const
{
    return tcpsocket;
}
