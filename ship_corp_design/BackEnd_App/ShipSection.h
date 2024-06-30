#ifndef __SHIP_SECTION_H
#define __SHIP_SECTION_H

#include "templ_sptr.h"
#include "curve.h"
#include "action_curve.h"
#include "templ_s_array.h"


class ShipSection {

private:

	SPtr<MbCurve> line{ nullptr };
	ShipSection(SArray<MbCartPoint>* points, MbePlaneType type);

public:

	ShipSection() = delete;
	ShipSection(const ShipSection& other) = default;

	void CalculatePolygon(double sag, MbPolygon& poligon);
	void CalculateNormals(std::vector < MbDirection>& dir, std::vector<MbCartPoint>& points);

	static std::shared_ptr<ShipSection> CreatePolyline(SArray<MbCartPoint>* points);
	static std::shared_ptr<ShipSection> CreateCubicSpline(SArray<MbCartPoint>* points);
	static std::shared_ptr<ShipSection> CreateHermit(SArray<MbCartPoint>* points);
	static std::shared_ptr<ShipSection> CreateNURBS(SArray<MbCartPoint>* points);
};

#endif // __SHIP_SECTION_H
