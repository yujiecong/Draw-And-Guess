#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>

namespace Ui {
class userWidget;
}

class userWidget : public QWidget
{
    Q_OBJECT

public:
    explicit userWidget(QString name,QWidget *parent = 0);
    ~userWidget();

    void setScore(int value);

    int getScore() const;

    void setSd(const QString &value);

    void setIp(const QString &value);

    QString getName() const;


    void setNameLabel(QString);

    QString getSd() const;
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::userWidget *ui;
    QString name;
    QString ip;
    QString sd;
    int score=0;
};

#endif // USERWIDGET_H
