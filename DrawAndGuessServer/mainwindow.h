#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicsscene.h"
#include "tcpserver.h"

#include <QFocusEvent>
#include <QLabel>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:


    void on_closeServerBtn_clicked();
    void slotsDispatchMessage(QString); //用于处理客户端发来的消息
    void slotsClientLeaved(QString sd);//溜力
    void slotsSendNewInfo2Client(QJsonObject &json, QJsonObject &content);//有新用户来了， 广播通知
    void slotsChooseOneStart();//开始游戏
    void slotsFinishedGame();//结束游戏
//     void slotsAllPersonFinish();//判断所有人是否已经画过一遍了





    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

protected:
    void resizeEvent(QResizeEvent *event);
private:
    Ui::MainWindow *ui;
    TcpServer *tcpserver=nullptr;
    GraphicsScene *scene;
    //sd 是QStringQStringQStringQStringQStringQStringQStringQStringQStringQStringQStringQStringQStringQStringQStringQString
    QMap<QString,QLabel*>labelMap;
    QJsonObject userJson; //存储 用户的json信息
    QString currentDrawer;
    int turnIdx=0; //用于记录当前是第几个人在画
    int curPersonNum=0;//当前人数
    QString curKeyWord;

    QString slotsRandomChoiceKeyWord();

    void dataInit();
//    QMap<int,
};

#endif // MAINWINDOW_H
