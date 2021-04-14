#include "graphicsscene.h"
#include "QDebug"
#include "jsonop.h"

#include <QFocusEvent>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

#define EREASER_EDGE_COLOR QColor(100,100,100)
GraphicsScene::GraphicsScene(QObject *parent):QGraphicsScene(parent)
  ,color(QColor(0,0,0))
  ,backgroundColor(QColor(255,255,255))
{
    bgPen=backgroundColor;
    bgBrush=backgroundColor;
    brush=QBrush(color);
    pen=QPen(color);
    linePen=QPen(color);
    linePen.setWidth(penWidth);
    //初始化这个
    lastPressButton=0;


}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //如果不是在画画的时候,返回所有事件
    if(!isDarwing){
        return;
    }

    //由于都是 x,y,w,h 的形式,我们创建一个统一的格式
    //如果是画点 在当前鼠标位置画点
    lastPressButton=event->button();

    if (curType==TYPE_POINT){
        QPointF poi=event->scenePos();
        x=poi.x();
        y=poi.y();
        w=drawPointRadius;
        h=drawPointRadius;
        //这里可以拓展一下
        addEllipse(x,y,w,h,pen,brush);
        initAndSendJson();
    }
    else if(curType==TYPE_LINE){
        //在当前选择一个起始点
        if(!pendown){
            start=event->scenePos();
            pendown=1;
        }
        else{
            end=event->scenePos();
            pendown=0;
            x=start.x();
            y=start.y();
            w=end.x();
            h=end.y();
            addLine(x,y,w,h,linePen);
            initAndSendJson();
        }
        //        QGraphicsLineItem (*al)(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen);
        //        drawSomething<QGraphicsLineItem>(start,end,drawing, event,start.x(),start.y(),end.x(),end.y(),pen,al);
    }
    else if(curType==TYPE_CIRCLE){
        //在当前选择一个起始点
        if(!pendown){
            start=event->scenePos();
            pendown=1;
        }
        else{
            end=event->scenePos();
    pendown=0;
            //用的是矩形的定义 所以画圆的话必须先处理一下

            //获得高和宽 ,哪个小画哪个
            int width=end.x()-start.x();
            int height=end.y()-start.y();
            //如果height是负的代表从下往上画
            int min=qAbs(width>height?height:width);
            x=start.x()<end.x()?start.x():end.x();
            y=start.y()>end.y()?end.y():start.y();
            w=min;
            h=min;
            addEllipse(x,y,w,h,pen,brush);
            initAndSendJson();
        }

    }
    else if(curType==TYPE_ECLIPSE){
        //在当前选择一个起始点
        if(!pendown){
            start=event->scenePos();
            pendown=1;
        }
        else{
            end=event->scenePos();
    pendown=0;
            //用的是矩形的定义 所以画圆的话必须先处理一下

            //获得高和宽 ,哪个小画哪个

            x=start.x();
            y=start.y();
            w=end.x()-start.x();
            h=end.y()-start.y();
            addEllipse(x,y,w,h,pen,brush);
            initAndSendJson();
        }

    }
    else if(curType==TYPE_RECT){
        //在当前选择一个起始点
        if(!pendown){
            start=event->scenePos();
            pendown=1;
        }
        else{
            end=event->scenePos();
    pendown=0;
            int x2=end.x();
            int x1=start.x();
            int y2=end.y();
            int y1=start.y();
            //判断 哪个点先后哦
            //            如果x2>x1 并且 y2>y1
            w=qAbs( end.x()-start.x());
            h=qAbs(end.y()-start.y());
            if(x2>x1 && y2>y1){
                x=x1;
                y=y1;
                addRect(x,y,w,h,pen,brush);
            }

            //如果x2<x1 y2>y1
            else if(x2<x1 && y2>y1){
                x=x1-w;
                y=y1;

                addRect(x,y,w,h,pen,brush);
            }

            else if(x2>x1 && y1>y2)
            {
                x=x1;
                y=y1-h;
                addRect(x,y,w,h,pen,brush);
            }

            else if(x1>x2 && y1>y2)
            {
                x=x2;
                y=y2;
                addRect(x,y,w,h,pen,brush);
            }
            initAndSendJson();

        }
    }
    else if(curType==TYPE_ERASER){
        // 将 半径内的圆 填充为背景色
        QPointF cur=event->scenePos();
        //创建一个橡皮擦跟着鼠标走啊
        //性能问题
        x=cur.x()-eraseRadius;
        y=cur.y()-eraseRadius;
        w=eraseRadius*2;
        h=eraseRadius*2;
        addEllipse(x,y,w,h,bgPen,bgBrush);

    }
    else if(curType==TYPE_ERASERALL){
        clear();
    }


}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if(!isDarwing){
        return;
    }
    //如果是画点 在当前鼠标位置画点 并且要在左键按下时画点

    if(lastPressButton==Qt::LeftButton){
        QPointF poi=event->scenePos();
        if (curType==TYPE_POINT){
            //这里可以拓展一下
            x=poi.x();
            y=poi.y();
            w=drawPointRadius;
            h=drawPointRadius;
            addEllipse(x,y,w,h,pen,brush);
            initAndSendJson();
        }

        else if(curType==TYPE_ERASER){
            x=poi.x()-eraseRadius;
            y=poi.y()-eraseRadius;
            w=eraseRadius*2;
            h=eraseRadius*2;
            addEllipse(x,y,w,h,bgPen,bgBrush);
            initAndSendJson();
        }


    }
    // 显示 橡皮擦 跟着 鼠标走啊
    if(curType==TYPE_ERASER){

        QPointF cur=event->scenePos();

        //        qDebug()<<cur;
        //        rect 和 pos 不一样啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊

        eraser->setPos(cur.x(),cur.y());

    }

}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isDarwing){
        return;
    }
    Q_UNUSED(event);

    lastPressButton=0;
}

void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if(!isDarwing){
        return;
    }
    //用于调节 擦除半径或者其他
    changeSizeByType(TYPE_POINT,event,drawPointRadius,POINT_PER_CHANGE);
    changeSizeByType(TYPE_LINE,event,penWidth,LINE_PER_CHANGE);
    changeSizeByType(TYPE_ERASER,event,eraseRadius,ERASE_PER_CHANGE);


}

bool GraphicsScene::getIsDarwing() const
{
    return isDarwing;
}

void GraphicsScene::setIsDarwing(bool value)
{
    isDarwing = value;
}

void GraphicsScene::initOthersDarwing(QJsonObject &json)
{
    //优化空间比较大
//    qDebug()<<json;
    //根据客户端发过来的绘图信息绘图
    int penType=json["penType"].toInt();
    //获取当前color
    color=QColor(json["color"].toObject()["r"].toInt(),
            json["color"].toObject()["g"].toInt(),
            json["color"].toObject()["b"].toInt());
    int penSytle=json["pen"].toObject()["penStyle"].toInt();
    penWidth=json["pen"].toObject()["penWidth"].toInt();
    QJsonObject rectJson=json["rect"].toObject();
    QRect rect(rectJson["x"].toInt(),
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
    darwSomething(penType,rect);
}

void GraphicsScene::darwSomething(int penType,QRect &rect)
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

void GraphicsScene::setPenStyle(int value)
{
    penStyle = value;
}

void GraphicsScene::initAndSendJson()
{
    QJsonObject json;

    QJsonObject penJson;  // 笔的参数
    penJson["penStyle"]=penStyle;  //用于判断是 line画笔还是普通画笔
    penJson["penWidth"]=penWidth;
    QJsonObject pointJson; // 画点的一些参数
    pointJson["radius"]=drawPointRadius; //画点的半径
    QJsonObject eraserJson;
    eraserJson["radius"]=eraseRadius;
    QJsonObject colorJson;
    colorJson["r"]=color.red();
    colorJson["g"]=color.green();
    colorJson["b"]=color.blue();
    QJsonObject rectJson;
    rectJson["x"]=x;
    rectJson["y"]=y;
    rectJson["w"]=w;
    rectJson["h"]=h;
//    QJsonObject brushJson; //全局的颜色 跟pen和color是一样的,所以不用考虑

//   eraserJson[]

    json["penType"]=curType; //用于判断目前画笔类型 包括橡皮
    json["pen"]=penJson;
    json["eraser"]=eraserJson;
    json["point"]=pointJson;
    json["color"]=colorJson;
    json["rect"]=rectJson;

    drawJson["messageType"]=MESSAGE_TYPE::DRAW;
    drawJson["content"]=json;
    QString j=JsonOp::JsonToQstring(drawJson);
    QTimer::singleShot(10,this,[=](){emit signalsSendDrawingInfo(j); }) ;
}

void GraphicsScene::changeSizeByType(const int type, QGraphicsSceneWheelEvent *event, int &r, const int per)
{

    if(curType==type){
        if(event->delta()>0)
            r+=per;
        else
            if(r>0)
                r-=per;
    }
    else{
        return;
    }
    if(curType==TYPE_LINE){
        linePen.setWidth(r);
    }
    else if(curType==TYPE_ERASER){
        //直接覆盖
        eraser->setRect(-eraseRadius,-eraseRadius,eraseRadius*2,eraseRadius*2);
        eraser->setPos(event->scenePos().x(),event->scenePos().y());
    }
}




void GraphicsScene::setColor(const QColor &value)
{
    color = value;
    pen.setColor(color);
    linePen.setColor(color);
    brush.setColor(color);
}

void GraphicsScene::setCurType(int value)
{
    //在 选择画笔时,重置 drawing
    pendown=0;
    //除了橡皮擦外,都要隐藏
    if(value==TYPE_ERASER){
        eraser=addEllipse(width(),height(),eraseRadius*2,eraseRadius*2,QPen(EREASER_EDGE_COLOR),brush);

        eraser->setZValue(1);
        eraser->show();
    }
    else{
        //被清除了

    }
    if(value==TYPE_LINE){
        penStyle=LINE_PEN;
    }
    else{
        penStyle=POINT_PEN;
    }
    curType = value;


}

//template<typename T>
//const void drawSomething(QPointF start,QPointF end, bool drawing,QGraphicsSceneMouseEvent *event, qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen, T (*draw)(qreal, qreal, qreal, qreal, const QPen &))
//{
//    //在当前选择一个起始点
//    if(!drawing){
//        start=event->scenePos();
//        drawing=1;
//    }
//    else{
//        end=event->scenePos();
//        drawing=0;
//        draw(x1,y1,x2,y2,pen);
//    }
//}
