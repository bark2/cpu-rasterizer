#pragma once

#include "StdAfx.h"
#include "vec3.hpp"
#include "mat4.h"
#include "Edge.h"
#include "Poly.h"

/*
	this is a utility struct wich helps with edge calculatiton
*/

struct Line {
	vec3 p;
	vec3 d;

	Line() = default;
	Line(vec3 p0, vec3 p1) : p(p0), d(p1 - p0) {}
	Line(vec3 v1, vec3 v2, const Pipeline& pipe) : p(pipe(v1)), d(pipe(v2) - pipe(v1)) {}
	Line(Edge* e) : p(e->v1->vec), d(e->v2->vec - e->v1->vec) {}
	Line(Edge* e, const Pipeline& pipe) : p(pipe(e->v1->vec)), d(pipe(e->v2->vec) - pipe(e->v1->vec)) {}
	Line(Edge* e, bool flipped, const Pipeline& pipe) {
		vec3 v1{ (flipped) ? e->v2->vec : e->v1->vec };
		vec3 v2{ (flipped) ? e->v1->vec : e->v2->vec };
		vec3 p1{ pipe(v1) };
		vec3 p2{ pipe(v2) };
		*this = Line(p1, p2);
	}

	bool operator==(const Line& rhs) { return p == rhs.p && d == rhs.d; }

	static double cross(const vec3& v1, const vec3 v2) {
		return v1.x*v2.y - v1.y*v2.x;
	}
};

struct Intersection {
	bool are_intersecting;
	bool are_only_touching;
	vec3 lp, rp;
};

/* 
	returns an Intersection type of which the are_intersection param is set to be true 
	only if the the lines are intersecting in screen space.
	lp and rp are the points on each line that could be with a different z value 
*/
Intersection line_intersection_2d(const Line& l, const Line& r);
Intersection scan_line_intersection_2d(unsigned int y, const Line& line);

/*
	collision detection for line-plane in model space with pipe transformation to screen space
*/
std::pair<bool, vec3> CollideLPlane(const Line& l, const Poly& p, const Pipeline& pipe);
