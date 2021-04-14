#include "graphicsscene.h"
#include "global.h"
#include "QDebug"
GraphicsScene::GraphicsScene(QObject *parent):QGraphicsScene(parent),
    bgPen(QColor(255,255,255)),
    bgBrush(QColor(255,255,255))

{

}

void GraphicsScene::slotsInitData(QJsonObject json)
{
    //优化空间比较大
//    qDebug()<<json;
    //根据客户端发过来的绘图信息绘图
    penType=json["penType"].toInt();
    //获取当前color
    color=QColor(json["color"].toObject()["r"].toInt(),
            json["color"].toObject()["g"].toInt(),
            json["color"].toObject()["b"].toInt());
    penSytle=json["pen"].toObject()["penStyle"].toInt();
    penWidth=json["pen"].toObject()["penWidth"].toInt();
    QJsonObject rectJson=json["rect"].toObject();
    rect=QRect(rectJson["x"].toInt(),
            rectJson["y"].toInt(),
            rectJson["w"].toInt(),
            rectJson["h"].toInt());
    //初始化画笔和笔刷
    pen=QPen(color);
    linePen=QPen(color);
    if(penSytle==LINE_PEN){
        linePen.setWidth(penWidth);
    }

    brush=QBrush(color);
    slotsDrawSomething();
}

void GraphicsScene::slotsDrawSomething()
{
    //根据penType在对应位置上画点啊
    switch (penType) {
    case TYPE_POINT:
        addEllipse(rect,pen,brush);
        break;
    case TYPE_LINE:
        addLine(rect.x(),rect.y(),rect.width(),rect.height(),linePen);
        break;
    case TYPE_CIRCLE:
        addEllipse(rect,pen,brush);
        break;
    case TYPE_ECLIPSE:
        addEllipse(rect,pen,brush);
        break;
    case TYPE_RECT:
        addRect(rect,pen,brush);
        break;
    case TYPE_ERASER:
        addEllipse(rect,bgPen,bgBrush);
    case TYPE_ERASERALL:
        clear();
    default:
        break;
    }
}
