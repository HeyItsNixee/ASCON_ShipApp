#ifndef __MAPWDGT_H
#define __MAPWDGT_H

#include <QtWidgets/QWidget>
#include <QPainter>
#include <qevent.h>
#include <iostream>

class MapWdgt : public QWidget
{
    Q_OBJECT

public:
    MapWdgt(QWidget* parent = nullptr);
    ~MapWdgt();

protected:
    void paintEvent(QPaintEvent* e);

private slots:
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
private:
    //scale гарантирует не быть <= 0;
    double scale = 1.;
    QPoint mousePos;
};


#endif
