#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QObject>
#include <QWidget>
#include "global.h"
//template<typename T>
//void const drawSomething(QPointF start,QPointF end, bool drawing,QGraphicsSceneMouseEvent *event, qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen, T (*draw)(qreal, qreal, qreal, qreal, const QPen &));

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsScene(QObject *parent = Q_NULLPTR);
    void setCurType(int value);
    void setColor(const QColor &value);
    //第一次落点
    QPointF start;
//    第二次落点
    QPointF end;
    //会话中
    void setPenStyle(int value);
    bool getIsDarwing() const;
    void setIsDarwing(bool value);
    void initOthersDarwing(QJsonObject &json);
    void darwSomething(int penType, QRect &rect);
signals:
    void signalsSendDrawingInfo(QString);
protected:
    //需要重写 声明鼠标事件触发函数
    void mousePressEvent(QGraphicsSceneMouseEvent  *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
private:
    int curType=TYPE_POINT; //当前画笔样式
    int penStyle=POINT_PEN; //用于区分 线和 点
    int eraseRadius=5;// 橡皮擦半径
    const int ERASE_PER_CHANGE=5; //橡皮擦每一次变化的值
    int lastPressButton; //用于记录上一次按下的鼠标按钮 是什么 用于 判断move
    int drawPointRadius=2; //记录 绘图点的 半径
    const int POINT_PER_CHANGE=1; //点变化的值
    const int LINE_PER_CHANGE=2;
    int penWidth=30;
    QColor backgroundColor;
    QColor color;
    QPen pen;
    QPen linePen;
    QBrush brush;
    QGraphicsEllipseItem *eraser=nullptr; //跟着鼠标走的橡皮擦
    qreal x,y,w,h; //全局 变量 用于记录 鼠标
    QPen bgPen;
    QBrush bgBrush;
    QJsonObject drawJson; //构建一个json用于 上传到服务器再dispatch
      bool pendown=0;
      bool isDarwing=1;
    void initAndSendJson(); //初始化json的信息
    void changeSizeByType(const int type, QGraphicsSceneWheelEvent *event, int &r, const int per);
};

#endif // GRAPHICSSCENE_H
