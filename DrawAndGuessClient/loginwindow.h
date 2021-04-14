#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include "tcpsocket.h"
#include <QMainWindow>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

    TcpSocket *getTcpsocket() const;
    QString getName() const;

signals:
    void signalsCreateTcpSocket();
    void signalsDisconnected(); // 客户端已经断开
private slots:
    void on_tryConnBtn_clicked(); //登录草
    void slotsSetInfo2Socket(); //将 客户端所有信息 存入socket
    void slotsSocketDisconnected(); //服务端断开 或者自己断开
private:
    Ui::LoginWindow *ui;
    QString name;
    TcpSocket *tcpsocket=nullptr;
};

#endif // LOGINWINDOW_H
