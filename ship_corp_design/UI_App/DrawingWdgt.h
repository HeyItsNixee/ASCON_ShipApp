#ifndef __DRAWINGWDGT_H
#define __DRAWINGWDGT_H

#include "ShipSection.h"
#include <QtWidgets/QWidget>
#include <QPainter>
#include <qpushbutton>
#include <qevent.h>
#include <qvector>
#include <QWheelEvent>
#include <iostream>

class DrawingWdgt : public QWidget
{
    Q_OBJECT

public:
    DrawingWdgt(QWidget* parent = nullptr, int new_drawX = 150, int new_drawY = 150);
    ~DrawingWdgt();
    std::vector<MbCartPoint> coordinatesArray; //будет использовать в себе calculateCoordinates(pointsArray[i])

protected:
    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent *ev);
    QPointF calculateCoordinates(QPointF mouse_cords);
    QPoint calculatePixels(QPointF point);
    QVector<QPointF> createQVector(SArray<MbCartPoint>* saray);
private slots:
    void mousePressEvent(QMouseEvent* e);
private:
    QPointF mousePos = QPointF();
    QVector<QPointF> pointsArray;
    QVector<QPointF> hotpointsArray;
    //scale гарантирует не быть <= 0
    double scale = 1.;
    int drawX;
    int drawY;
    bool gotEndPoint = false;
};

#endif