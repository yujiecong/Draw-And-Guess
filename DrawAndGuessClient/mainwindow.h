#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicsscene.h"
#include "tcpsocket.h"
#include "userwidget.h"

#include <QColorDialog>
#include <QMainWindow>
#include "loginwindow.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void slotsShowMessage(QString mes);
    void slotsNewCreateSocket(); //对 新创建的socket 进行操作
    void slotsWhenClientDisconnected(); //当客户端断开时
    void slotsDispatchMessage(QString);
    void slotsTurnFinished();
    void slotsSendText(QString str);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_18_clicked();


    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();


    void on_pushButton_20_clicked();

    void on_pushButton_19_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
    LoginWindow *lw;
    GraphicsScene *scene;
    userWidget *myUserWidget=nullptr;
    QMap<QString,userWidget*> userMap;


    const int PER_TURN_TIME=10; //一次回合的秒数
    int turnLimit=PER_TURN_TIME;
    QString curKeyWord;//当前关键词
    void init();


};

#endif // MAINWINDOW_H
