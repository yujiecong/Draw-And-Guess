#include "chatwidget.h"
#include "ui_chatwidget.h"

#include <QPainter>
#include <qevent.h>

chatWidget::chatWidget(QString name, QString text, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatWidget)
{
    ui->setupUi(this);
    ui->name->setText(name);
    ui->content->setText(text);
}

chatWidget::~chatWidget()
{
    delete ui;
}

void chatWidget::setAlignment(Qt::AlignmentFlag align)
{
    ui->name->setAlignment(align);
    ui->content->setAlignment(align);
}



void chatWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(72,99,231));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 5, 5);
}
