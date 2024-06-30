#include "MapWdgt.h"

const QRect frame(3, 3, 175, 136);
const QPen defaultPen(Qt::black, 3);
const int interRectW = 35;
const int interRectH = 30;
const QRect defaultInterRect(0, 25, 35, 30);
const QPen interPen(Qt::red, 3);
bool firstDraw = true;

MapWdgt::MapWdgt(QWidget* parent) :
	QWidget(parent){

}

MapWdgt::~MapWdgt()
{

}


void MapWdgt::paintEvent(QPaintEvent* e){
	QPainter p(this);
	QPixmap pixmap(":/new/prefix1/Icons/MapIMG.png");
	p.setPen(defaultPen);
	p.drawRect(frame);
	p.drawPixmap(frame, pixmap);
	p.setPen(interPen);
	if (firstDraw)
		p.drawRect(defaultInterRect);
	else
		p.drawRect(mousePos.x() - 15, mousePos.y() - 15, interRectW, interRectH);
	this->update();
}

void MapWdgt::mouseMoveEvent(QMouseEvent* e) {
	mousePos = e->pos();
}

void MapWdgt::mousePressEvent(QMouseEvent* e) {
	if (e->button() == Qt::LeftButton) {
		qDebug("LeftMousePress on MapWdgt. Turning ON mouseTracking.");
		this->setMouseTracking(true);
		firstDraw = false;
	} else
		if (e->button() == Qt::RightButton) {
			qDebug("RightMousePress on MapWdgt. Turning OFF mouseTracking");
			mousePos = e->pos();
			this->setMouseTracking(false);
		}
}