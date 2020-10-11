#include "Line.h"
#include "Poly.h"
#include "mat4.h"
#include "vec3.hpp"
#include <utility>


// TODO: seperate to two function for performance
Intersection line_intersection_2d(const Line& l, const Line& r) {
	Intersection result;
	vec3 a = r.p - l.p;
	double b = Line::cross(l.d, r.d);
	double t = Line::cross(a, r.d) / b;
	double u = Line::cross(a, l.d) / b;
	result.are_intersecting = b != 0 && t >= 0 && t <= 1 && u >= 0 && u <= 1;
	result.are_only_touching = (t == 0 || t == 1) || (u == 0 || u == 1);
	result.lp = l.p + l.d * t;
	result.rp = r.p + r.d * u;
	return result;
}

Intersection scan_line_intersection_2d(unsigned int y, const Line& line) {
	Line scan_line({ -1000000.0, (double)y, 0.0 }, { 1000000.0, (double)y, 0.0 });
	return line_intersection_2d(scan_line, line);
}

std::pair<bool, vec3> CollideLPlane(const Line& l, const Poly& p, const Pipeline& pipe) {
	std::pair<bool, vec3> result;
	Edge* e1 = p.get_edges().front();
	vec3 v1 = pipe(e1->v1->vec);
	vec3 n = p.get_normal(pipe);
	if (l.d*n == 0)
		result = { false, {} };
	else
		result = { true, l.p + l.d * (((v1 - l.p) * n) / (l.d * n)) };

	return result;
}
