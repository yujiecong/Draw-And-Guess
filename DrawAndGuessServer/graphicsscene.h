#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QJsonObject>
#include <QObject>
#include <QWidget>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsScene(QObject *parent = Q_NULLPTR);
public slots:
    void slotsInitData(QJsonObject json);
    void slotsDrawSomething();
protected:

private:
    QColor color;
    QRect rect;
    QPen pen;
    QPen linePen;
    QBrush brush;
    QPen bgPen;
    QBrush bgBrush;
    int penSytle;
    int penType;

    int penWidth;
    int eraserRadius;
    int pointRadius;

};

#endif // GRAPHICSSCENE_H
