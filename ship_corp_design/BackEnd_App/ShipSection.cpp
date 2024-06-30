#include "ShipSection.h"

ShipSection::ShipSection(SArray<MbCartPoint>* points, MbePlaneType type) {

	MbCurve* linePointer{ nullptr };
	switch (type) {
		//Кубический сплайн
		case(MbePlaneType::pt_CubicSpline) :
				SplineCurve(*points, false, MbePlaneType::pt_CubicSpline, linePointer);

		//Ломаная
		case(MbePlaneType::pt_Polyline) :
				SplineCurve(*points, false, MbePlaneType::pt_Polyline, linePointer);

		//Сплайн Эрмита
		case(MbePlaneType::pt_Hermit) :
			SplineCurve(*points, false, MbePlaneType::pt_Hermit, linePointer);

		//NURBS
		case(MbePlaneType::pt_Nurbs) : 
			SplineCurve(*points, false, MbePlaneType::pt_Nurbs, linePointer);
	}
	
	line = linePointer;
}


void ShipSection::CalculatePolygon(double sag, MbPolygon& poligon) {
	line->CalculatePolygon(sag, poligon);
}

void ShipSection::CalculateNormals(std::vector<MbDirection>& dir, std::vector<MbCartPoint>& points) {
	MbCartPoint point;
	MbDirection elem;
	for (double a = line->GetTMin(); a < line->GetTMax(); a += line->Step(a, Math::visualSag)) {
		line->PointOn(a, point);
		points.push_back(point);
		line->Normal(a, elem);
		dir.push_back(elem);
	}
		
}

std::shared_ptr<ShipSection> ShipSection::CreatePolyline(SArray<MbCartPoint>* points) {
	if ( (points != nullptr) && (points->size() > 3) )  {
		ShipSection ship(points, MbePlaneType::pt_Polyline);
		return std::make_shared<ShipSection>(ship);
	}
	else
		return nullptr;
}

std::shared_ptr<ShipSection> ShipSection::CreateCubicSpline(SArray<MbCartPoint>* points) {
	if (points->size() >= 2 && points != nullptr) {
		ShipSection ship(points, MbePlaneType::pt_CubicSpline);
		return std::make_shared<ShipSection>(ship);
	}
	else
		return nullptr;
}

std::shared_ptr<ShipSection> ShipSection::CreateHermit(SArray<MbCartPoint>* points) {
	if (points->size() >= 2 && points != nullptr) {
		ShipSection ship(points, MbePlaneType::pt_Hermit);
		return std::make_shared<ShipSection>(ship);
	}
	else
		return nullptr;
}

std::shared_ptr<ShipSection> ShipSection::CreateNURBS(SArray<MbCartPoint>* points) {
	if (points->size() >= 2) {
		ShipSection ship(points, MbePlaneType::pt_Nurbs);
		return std::make_shared<ShipSection>(ship);
	}
	else
		return nullptr;
}