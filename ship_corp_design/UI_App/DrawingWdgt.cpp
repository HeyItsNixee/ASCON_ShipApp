#include "DrawingWdgt.h"
#include <mesh_polygon.h>

const QPen defaultPen(Qt::blue, 3);
const QPen highlightPen(Qt::red, 12);
const QPen lineToPen(Qt::black, 5);
const QSize defaultSize(650, 700);


DrawingWdgt::DrawingWdgt(QWidget* parent, int new_drawX, int new_drawY) :
	QWidget(parent),
	drawX(new_drawX),
	drawY(new_drawY){
	//------| это пока просто для отображения корректности передачи размеров---
	//------v не финальное сравнение ---
	if (drawX == 250 && drawY == 250)
		qDebug("X and Y successfully imported!");
	this->setCursor(Qt::CrossCursor);

}

DrawingWdgt::~DrawingWdgt() {

}

QPointF DrawingWdgt::calculateCoordinates(QPointF mouse_cords) {
	//712px_x690px = drawXcm_xdrawYcm
	//this->size().width()x_this->size().height() = drawXcm_xdrawYcm
	//mousePos.x() / this->size().width() = ??%
	//drawX*??% = ??.??cm
	return QPointF(drawX * (mouse_cords.x() / this->size().width()), drawY * (mouse_cords.y() / this->size().height()));
}

QPoint DrawingWdgt::calculatePixels(QPointF point){
	//??.??cm / drawX = ??%
	//mousePos.x() = this->size().width() * ??%
	return QPoint((point.x() / drawX) * this->size().width(), (point.y() / drawY) * this->size().height());
}

QVector<QPointF> DrawingWdgt::createQVector(SArray<MbCartPoint>* saray){
	QVector<QPointF> result;
	for (auto it = saray->begin(); it < saray->end(); it++)
		result.push_back(QPointF(it->x, it->y));
	return result;
}

//------------ДЛЯ ОТРИСОВКИ ФАНТОМОВ---------------------
// 
//-----------нужно две точки поставить-------------------
/*for (auto it = pointsArray.begin(); it < pointsArray.end() - 1; it++) {
		p.drawLine(*it, *it++);
	}*/
//-----------будет соединять последнюю точку с новой----------------
/*for (int i = 0; i < pointsArray.size() - 1; i++) {
		p.drawLine(pointsArray[i], pointsArray[i + 1]);
}*/
//-------------------------------------------------------

// проверка: отображает ли виджет отрезок
void DrawingWdgt::paintEvent(QPaintEvent*) {
	QPainter p(this);
	p.setPen(defaultPen);
	if (gotEndPoint && !pointsArray.empty()) {
		p.scale(scale, scale);
		SArray<MbCartPoint> saray(coordinatesArray);
		auto cubCurve = ShipSection::CreateCubicSpline(&saray);
		MbPolygon polygon{};
		cubCurve->CalculatePolygon(Math::visualSag, polygon);
		polygon.GetPoints(saray);
		pointsArray = createQVector(&saray);

		for (int i = 1; i < pointsArray.size(); i++) {
			p.drawLine(calculatePixels(pointsArray[i - 1]), calculatePixels(pointsArray[i]));
		}
		for (int i = 0; i < hotpointsArray.size(); i++) {
			p.setPen(highlightPen);
			p.drawPoint(hotpointsArray[i]);
		}

	}
	else
		if (!pointsArray.empty()) {
			p.setPen(highlightPen);
			p.scale(scale, scale);
			for (int i = 1; i < hotpointsArray.size(); i++) {
				p.setPen(lineToPen);
				p.drawLine(hotpointsArray[i - 1], hotpointsArray[i]);
				p.setPen(highlightPen);
				p.drawPoint(hotpointsArray[i - 1]);
			}
			p.drawPoint(mousePos);
		}
	//update должен отрабатывать каждый раз для корректной отрисовки
	update();
}

void DrawingWdgt::mousePressEvent(QMouseEvent* e) {
	if (e->button() == Qt::LeftButton && gotEndPoint == false) {
		qDebug() << "left click!";
		if (scale != 1) {
			mousePos.setX(e->pos().x() / scale);
			mousePos.setY(e->pos().y() / scale);
		}
		else {
			mousePos.setX(e->pos().x());
			mousePos.setY(e->pos().y());
		}
		pointsArray.push_back(mousePos);
		hotpointsArray.push_back(mousePos);
		QPointF coord = calculateCoordinates(mousePos);
		coordinatesArray.push_back(MbCartPoint(coord.x(), coord.y()));

		std::cout << mousePos.x() << " - x; " << mousePos.y() << " - y" << std::endl;
	}
	else
		if (e->button() == Qt::RightButton && gotEndPoint == false) {
			qDebug() << "right click!";
			qDebug() << "Got last point for Spline. Turning off mouseEvents on this widget";
			gotEndPoint = true;
			return;
		}
		else
			if (e->button() == Qt::MiddleButton && gotEndPoint) {
				qDebug() << "middle click!";
				qDebug() << "Resetting array...";
				gotEndPoint = false;
				coordinatesArray.clear();
				pointsArray.clear();
				hotpointsArray.clear();
				return;
			}

}

void DrawingWdgt::wheelEvent(QWheelEvent* ev) {
	double scale_defender = scale;
	std::cout << "DrawingWdgt.size() = " << this->size().width() << "x" << this->size().height() << std::endl;
	if (ev->angleDelta().y() > 0) {
		qDebug("Wheel+");
		scale += 0.1;
		this->setMinimumSize(QSize(this->size().width() * scale, this->size().height() * scale));
	}
	if (ev->angleDelta().y() < 0) {
		qDebug("Wheel-");
		if ((scale_defender - 0.1) <= 0)
			scale = 0.1;
		else
			scale -= 0.1;
		this->setMinimumSize(QSize(this->size().width() / scale, this->size().height() / scale));
	}
	std::cout << "scale = " << scale << std::endl;
	if (scale == 1)
		this->setMinimumSize(defaultSize);
}
