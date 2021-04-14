#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwindow.h"
#include "jsonop.h"
#include "chatwidget.h"


#include <QJsonObject>
#include <QMessageBox>
#include <QScrollBar>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <qevent.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),lw(new LoginWindow(this)),scene(new GraphicsScene),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 显示登录窗口
    lw->show();
    //绑定 消息显示
    connect(lw,&LoginWindow::signalsCreateTcpSocket,this,&MainWindow::slotsNewCreateSocket);
    //绑定scene
    ui->graphicsView->setScene(scene);
    //设置动态抓取
    setMouseTracking(1);
    ui->graphicsView->setMouseTracking(1);

    ui->graphicsView->setSceneRect(0,0,760,440);
    //初始化
    init();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}



void MainWindow::slotsShowMessage(QString mes)
{
    Q_UNUSED(mes);
}

void MainWindow::slotsNewCreateSocket()
{

    // 客户端发送绘图消息到服务器
    connect(scene,&GraphicsScene::signalsSendDrawingInfo,lw->getTcpsocket(),&TcpSocket::slotsSendMessage);
    connect(lw->getTcpsocket(),&TcpSocket::signalsShowMessage,this,&MainWindow::slotsDispatchMessage);
    connect(lw,&LoginWindow::signalsDisconnected,this,&MainWindow::slotsWhenClientDisconnected);
}

void MainWindow::slotsWhenClientDisconnected()
{
    //可以访问是否重连
    //    QMessageBox::warning(this,"已经掉线","是否选择重连?");
}


void  MainWindow::slotsDispatchMessage(QString mes)
{
    //处理服务端发过来的消息
    //    qDebug()<<mes.toUtf8().data();
    QJsonObject json=JsonOp::qString2Json(mes);
    QJsonObject contentJson=json["content"].toObject();
//    qDebug()<<"客户端收到";
//    qDebug()<<mes.toUtf8().data();
    int t=json["messageType"].toInt();
    if (t==MESSAGE_TYPE::CHAT){
        //从服务端传过来的信息 加一下到聊天里
        QString text=contentJson["text"].toString();
        QString name=contentJson["name"].toString();
        //构建一个聊天气泡
        chatWidget *cw = new chatWidget(name,text);
        //如果是别人发的就靠左边站
        QString sd=contentJson["sd"].toString();
        if(sd==lw->getTcpsocket()->getSd()){
            //否则靠右
        }
        else{cw->setAlignment(Qt::AlignmentFlag::AlignLeft);}

        ui->verticalLayout_5->insertWidget(ui->verticalLayout_5->count()-1,cw);
        QTimer::singleShot(100,this,[=](){ui->scrollArea_2->verticalScrollBar()->setValue(ui->scrollArea_2->verticalScrollBar()->maximum());
        });

    }
    else if(t==MESSAGE_TYPE::DRAW){
        //如果自己是观察者 那么接收这个信息
        if(!scene->getIsDarwing()){
            scene->initOthersDarwing(contentJson);
        }


    }

    else if (t==MESSAGE_TYPE::NEW_CLIENT_BROADCAST){
        for(int i =0 ; i<ui->verticalLayout_4->count();i++){
            ui->verticalLayout_4->takeAt(i)->widget()->deleteLater();
        }
        foreach (QString sd, userMap.keys()) {
            userMap[sd]->deleteLater();
        }

        userMap.clear();

        if(myUserWidget!=nullptr)
        {
            myUserWidget->deleteLater();

        }


        foreach (QString sd, contentJson.keys()) {
            QJsonObject info=contentJson[sd].toObject();

            userWidget *uw=new userWidget(info["name"].toString());;
            userMap[sd]=uw;
            uw->setIp(info["ip"].toString());
            uw->setSd(sd);
            if(lw->getTcpsocket()->getSd()==sd){
                qDebug()<<"成功绑定";
                myUserWidget=uw;

            }
            ui->verticalLayout_4->insertWidget(ui->verticalLayout_4->count()-1,uw);
        }
        QTimer::singleShot(100,this,[=](){ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());});


    }
    else if (t==MESSAGE_TYPE::SERVER_BROADCAST){
        scene->clear();
        //到谁画了?
        QString sd=contentJson["sd"].toString();
        //
        //如果是我
        userMap[sd]->setNameLabel(userMap[sd]->getName()+"~(正在画)");
        QMessageBox *mb =new QMessageBox(this);
        mb->setText("游戏将在3s后开始");
        mb->setWindowTitle("警告");
        mb->show();
        QTimer::singleShot(1000,mb,[=](){mb->deleteLater();});


        if(sd==lw->getTcpsocket()->getSd()){

            // 可以动
            ui->penWidget->setEnabled(1);

            // //在后面我的名字后面加上(正在画)

            //开始游戏
            curKeyWord=contentJson["keyword"].toString();
            ui->keywordLabel->setText(tr("关键词:")+curKeyWord);
            //阻塞

            //到我了
            slotsSendText("到我画了");

            if(myUserWidget==nullptr){
                qDebug()<<"myuserWidget = null!";
                return;
            }
            //不准说话
            ui->lineEdit->setEnabled(0);
            //获得控制权,其他所有客户端开始看着 不能懂
            scene->setIsDarwing(1);
            //改变游戏状态
            lw->getTcpsocket()->setGameState(GAME_STATE::IS_DRAWING);
            //开始计时
            turnLimit=PER_TURN_TIME;;// 回合 变量
            QTimer *timer=new QTimer();
            timer->setInterval(1000);

            connect(timer,&QTimer::timeout,[=](){
                ui->label->setText(QString::number(turnLimit));
                turnLimit--;
                if(turnLimit==0){
                    timer->deleteLater();
                    ui->label->setText(QString::number(PER_TURN_TIME));
                    //到下一回合
                        lw->getTcpsocket()->setGameState(GAME_STATE::FINISNED_DRAWING);
                    slotsTurnFinished();
                    //公布答案
                    QTimer::singleShot(100,this,[=](){slotsSendText("答案是"+curKeyWord);});
                    ui->lineEdit->setEnabled(1);
                    ui->penWidget->setEnabled(1);

                }
            });
            timer->start();
        }
        else{

            turnLimit=PER_TURN_TIME;;// 回合 变量
            QTimer *timer=new QTimer();
            timer->setInterval(1000);
            ui->keywordLabel->setText(tr("关键词:???"));
            connect(timer,&QTimer::timeout,[=](){
                turnLimit--;
                if(turnLimit==0){
                    timer->deleteLater();
                    //不可以动
                    ui->penWidget->setEnabled(0);

                    //不是到我就还原
                    userMap[sd]->setNameLabel(userMap[sd]->getName());

                    ui->lineEdit->setEnabled(1);


                }
            });
            timer->start();

        }

        //
    }
    else if(t==MESSAGE_TYPE::REGESTER){
        //处理 注册信息
        lw->getTcpsocket()->setSd(contentJson["sd"].toString());
    }
    else if(t==MESSAGE_TYPE::POINT_TO_POINT){
        //用于处理服务端私聊的信息
        //如果猜错了
        int guessState=contentJson["guessState"].toInt();

        if(guessState==GAME_BINGO){
            myUserWidget->setScore(myUserWidget->getScore()+10);
            //猜中了就显示猜中 并且不能说话
            ui->lineEdit->setEnabled(0);
            ui->lineEdit->setPlaceholderText("猜中就别说啦!!");
            //全局消息 发送到服务器转发的
            slotsSendText("这货猜中了");
        }
        else{
            //            myUserWidget->setScore(myUserWidget->getScore()-2);
            //            ui->lineEdit->setPlaceholderText("你可别乱猜,要扣分哒!!");

        }
    }
}

void MainWindow::slotsTurnFinished()
{
    //    回合结束要做的事情
    ui->keywordLabel->setText(tr("关键词:???"));
    ui->label->setText(QString::number(PER_TURN_TIME));
    scene->setIsDarwing(0);
    //发射一个信号给服务的告知这个回合结束了
    //在服务端将这个sd 的相关bool值 置1
    //    代表已经画过了

    //发射一段信息给服务端代表已经结束回合
    QJsonObject turnJson;
    turnJson["messageType"]=MESSAGE_TYPE::POINT_TO_POINT;

    QJsonObject myData;
    myData["sd"]=lw->getTcpsocket()->getSd();
    myData["gameState"]=lw->getTcpsocket()->getGameState();
    turnJson["content"]=myData;

    lw->getTcpsocket()->slotsSendMessage(JsonOp::JsonToQstring(turnJson));
    lw->getTcpsocket()->waitForReadyRead(1000);



    //保存画布

    qDebug()<<ui->graphicsView->grab(ui->graphicsView->rect()).save(lw->getName()+QTime::currentTime().toString("HH mm ss a")+".jpg","JPG");

    //清空数据流
    lw->getTcpsocket()->readAll();

    userMap[lw->getTcpsocket()->getSd()]->setNameLabel(myUserWidget->getName()+"~(画完了)");

    QMessageBox::about(this,"本回合结束","图片已保存在当前目录下");
     scene->clear();


}

void MainWindow::slotsSendText(QString str)
{
    //发送信息给服务端
    QJsonObject json;
    json["messageType"]=MESSAGE_TYPE::CHAT;
    //谁说了什么话
    QJsonObject chatJson;

    chatJson["text"]=str;;
    chatJson["sd"]=lw->getTcpsocket()->getSd();
    chatJson["name"]=lw->getName();
    json["content"]=chatJson;
    lw->getTcpsocket()->slotsSendMessage(JsonOp::JsonToQstring(json));
    //其他信息 在服务端都存储了,所以只用这个就行
}
void MainWindow::on_pushButton_19_clicked()
{
    if(!ui->lineEdit->text().isEmpty()){

        slotsSendText(ui->lineEdit->text());
        ui->lineEdit->clear();
    }

}
void MainWindow::on_pushButton_clicked()
{
    scene->setCurType(TYPE_POINT);
}

void MainWindow::on_pushButton_2_clicked()
{
    //将type转变为 line
    scene->setCurType(TYPE_LINE);

}

void MainWindow::on_pushButton_3_clicked()
{
    scene->setCurType(TYPE_CIRCLE);
}

void MainWindow::on_pushButton_4_clicked()
{
    scene->setCurType(TYPE_ECLIPSE);
}

void MainWindow::on_pushButton_5_clicked()
{
    scene->setCurType(TYPE_RECT);
}
void MainWindow::on_pushButton_18_clicked()
{
    scene->setCurType(TYPE_ERASER);

}

void MainWindow::on_pushButton_6_clicked()
{
    // 设置画笔颜色
    scene->setColor(QColorDialog::getColor(Qt::red,this,"选择画笔颜色"));
}

void MainWindow::init()
{


}



void MainWindow::on_pushButton_7_clicked()
{
    scene->setColor(QColor(0,0,0));
}

void MainWindow::on_pushButton_8_clicked()
{
    scene->setColor(QColor(255,255,255));
}

void MainWindow::on_pushButton_9_clicked()
{
    scene->setColor(QColor(0,255,0));
}

void MainWindow::on_pushButton_10_clicked()
{
    scene->setColor(QColor(255,255,0));
}

void MainWindow::on_pushButton_11_clicked()
{
    scene->setColor(QColor(85, 170, 255));

}

void MainWindow::on_pushButton_12_clicked()
{
    scene->setColor(QColor(255, 0, 255));

}

void MainWindow::on_pushButton_13_clicked()
{
    scene->setColor(QColor( 255, 170, 255));

}

void MainWindow::on_pushButton_14_clicked()
{
    scene->setColor(QColor(255, 85, 127));

}

void MainWindow::on_pushButton_15_clicked()
{
    scene->setColor(QColor(255, 0, 127));

}

void MainWindow::on_pushButton_16_clicked()
{
    scene->setColor(QColor( 255, 85, 0));

}

void MainWindow::on_pushButton_17_clicked()
{
    scene->setColor(QColor( 255, 0, 0));
}


void MainWindow::on_pushButton_20_clicked()
{
    scene->setCurType(TYPE_ERASERALL);
    scene->clear();

}



void MainWindow::on_lineEdit_returnPressed()
{
    //发送
    on_pushButton_19_clicked();
}
