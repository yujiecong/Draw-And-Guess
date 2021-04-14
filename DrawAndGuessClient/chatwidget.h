#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>

namespace Ui {
class chatWidget;
}

class chatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit chatWidget(QString name,QString text,QWidget *parent = 0);
    ~chatWidget();
    void setAlignment(Qt::AlignmentFlag align);

protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::chatWidget *ui;
};

#endif // CHATWIDGET_H
