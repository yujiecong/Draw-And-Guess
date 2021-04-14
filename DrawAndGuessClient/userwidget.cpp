#include "userwidget.h"
#include "ui_userwidget.h"

#include <QPainter>

userWidget::userWidget(QString name, QWidget *parent):
    ui(new Ui::userWidget),
    name(name)
{
    ui->setupUi(this);
    ui->name->setText(name);
    ui->score->setText(QString::number(score));
}

userWidget::~userWidget()
{
    delete ui;
}

void userWidget::setScore(int value)
{
    score = value;
    ui->score->setText(QString::number(score));
}

int userWidget::getScore() const
{
    return score;
}

void userWidget::setSd(const QString &value)
{
    sd = value;
    ui->sd->setText("sd="+ sd);
}

void userWidget::setIp(const QString &value)
{
    ip = value;
    ui->ip->setText("ip="+ip);
}

QString userWidget::getName() const
{
    return name;
}

void userWidget::setNameLabel(QString str)
{
    ui->name->setText(str);
}

QString userWidget::getSd() const
{
    return sd;
}

void userWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(72,33,231));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 5, 5);
}
