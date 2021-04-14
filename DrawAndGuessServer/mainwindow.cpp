#include "jsonop.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include "global.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    scene(new GraphicsScene),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    //默认在6666 然后

    tcpserver=new TcpServer(this);
    //将 服务端收到的消息显示
    connect(tcpserver,&TcpServer::signalsShowOutMessage,this,&MainWindow::slotsDispatchMessage);
    connect(tcpserver,&TcpServer::signalsClientLeaved,this,&MainWindow::slotsClientLeaved);

    //    userJson["messageType"]=MESSAGE_TYPE::CLIENT_INFO;
    //    userJson["content"]=QJsonObject();
    //    QString::number(tcpserver->getCurSd()),infoJson
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_closeServerBtn_clicked()
{
    tcpserver->close();
    tcpserver=nullptr;
}

void MainWindow::slotsDispatchMessage(QString mesJson)
{
    //用于处理 客户端消息
    QJsonObject json=JsonOp::qString2Json(mesJson);
    QJsonObject content=json["content"].toObject();
    qDebug()<<mesJson.toUtf8().data();
    //格式
    // message type
    int t=json["messageType"].toInt();

    if(t==MESSAGE_TYPE::CHAT){
        //将聊天的信息转发到所有客户端看到
        tcpserver->slotsSendEveryOne(mesJson);

        //判断是否说中了
        QString sd=content["sd"].toString();
        QString text=content["text"].toString();



        QJsonObject p2pjson;
        p2pjson["messageType"]=MESSAGE_TYPE::POINT_TO_POINT;
        QJsonObject contentJson;

        if(text==curKeyWord){
            //这货说中了,返回一个值给他,
            //一个游戏状态 代表猜错猜对
            contentJson["gameState"]=GAME_BINGO;

        }
        else{
             contentJson["gameState"]=GAME_FAILED;
        }
        p2pjson["content"]=contentJson;


        //显示在屏幕上
        ui->listWidget->addItem(tcpserver->getSd2Socket()[sd]->getName()+":"+text);

        //trick
        QTimer::singleShot(10,this,[=](){
            tcpserver->slotsSendOne(*tcpserver->getSd2Socket()[sd] ,JsonOp::JsonToQstring(p2pjson));
        });






    }
    else if(t==MESSAGE_TYPE::DRAW){
        //如果收到了结束信息,清空数据流
        scene->slotsInitData(json["content"].toObject());
          //转发给各个客户端
        tcpserver->slotsSendEveryOne(mesJson);
    }

    else if(t==MESSAGE_TYPE::NEW_CLIENT_BROADCAST){
        slotsSendNewInfo2Client(json,content);
    }
    else if(t==MESSAGE_TYPE::POINT_TO_POINT){
        //用于处理回合
        //将sd对应的state设为1即可

        tcpserver->getSd2Socket()[content["sd"].toString()]->setGameState(GAME_STATE::FINISNED_DRAWING);
//        //判断是否还有人没画
//        tcpserver->getSd2Socket()[content["sd"].toString()]->setGuessState(GAME_FAILED);

        //    保存画布
//        QPixmap::grabWidget(ui->graphicsView,ui->graphicsView->rect()).save(QTime::currentTime().toString(),"jpg");;
        //画布清空
        scene->clear();
        //清空数据流

        if(turnIdx<curPersonNum-1){
            turnIdx++;
            slotsChooseOneStart();
        }
        else{
            //结束游戏要做的事
            slotsFinishedGame();
        }

    }

}

void MainWindow::slotsClientLeaved(QString sd)
{
    userJson.remove(sd);
    labelMap.take(sd)->deleteLater();
    curPersonNum--;

}

void MainWindow::slotsSendNewInfo2Client(QJsonObject &json,QJsonObject &content)
{
    //有新用户来了, 直接更新所有客户端的在线列表
    // 在服务端记录一下新用户后发送回去所有用户列表
    curPersonNum++;

    QJsonObject infoJson;
    infoJson["name"]=content["name"].toString();
    infoJson["ip"]=content["ip"].toString();

    //设置name
    tcpserver->getSd2Socket()[tcpserver->getCurSd()]->setName(content["name"].toString());
    //设置ip
    tcpserver->getSd2Socket()[tcpserver->getCurSd()]->setIp(content["ip"].toString());

    //    构建信息发送给所有人
    userJson[tcpserver->getCurSd()]=infoJson;
    json["content"]=userJson;


    tcpserver->slotsSendEveryOne(JsonOp::JsonToQstring(json));
    QLabel *l=new QLabel(infoJson["name"].toString());
    labelMap[tcpserver->getCurSd()]=l;
    ui->verticalLayout->insertWidget(ui->verticalLayout->count()-1,l);
}

void MainWindow::slotsChooseOneStart()
{


   //判断第几个人
    QString indexSD=tcpserver->getSd2Socket().keys()[turnIdx];
    QJsonObject json;
    json["messageType"]=MESSAGE_TYPE::SERVER_BROADCAST;
    //判断随机到的用户的游戏状态
    int gs=tcpserver->getSd2Socket()[indexSD]->getGameState();
    if (gs==GAME_STATE::WAIT_DRAWING){
        currentDrawer=indexSD;
        QJsonObject turnJson;
        turnJson["sd"]=indexSD;
        turnJson["keyword"]=slotsRandomChoiceKeyWord();
        tcpserver->getSd2Socket()[indexSD]->setGameState(GAME_STATE::IS_DRAWING);
        json["content"]=turnJson;
        QString str=JsonOp::JsonToQstring(json);
        tcpserver->slotsSendEveryOne(str);
    }

}

void MainWindow::slotsFinishedGame()
{
    // 初始化
    dataInit();
    // 判断是否有人猜出

    //设置所有人的状态为等待
    foreach (QString sd, tcpserver->getSd2Socket().keys()) {
        tcpserver->getSd2Socket()[sd]->setGameState(GAME_STATE::WAIT_DRAWING);

    }
    //

}
void MainWindow::resizeEvent(QResizeEvent *event){
    QMainWindow::resizeEvent(event);
    ui->graphicsView->setSceneRect(0,0,ui->graphicsView->width(),ui->graphicsView->height());
}

QString MainWindow::slotsRandomChoiceKeyWord()
{
    QString r("小鸡");
    curKeyWord=r;
    return r;
}

void MainWindow::dataInit()
{
    turnIdx=0;
}



void MainWindow::on_pushButton_clicked()
{
        slotsChooseOneStart();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->graphicsView->scene()->clear();
}
