#pragma once

#include "StdAfx.h"
#include "vec3.hpp"
#include "mat4.h"
#include "Vertex.h"
#include "Poly.h"

class Edge
{
public:
	Vertex *v1, *v2;
	Poly *p1, *p2;
	unsigned int id;

	Edge(Vertex* v1, Vertex* v2) : v1(v1), v2(v2), p1(nullptr), p2(nullptr) {}
	Edge(Vertex* v1, Vertex* v2, Poly* p1, Poly* p2) :
		v1(v1), v2(v2), p1(p1), p2(p2) {}

	vec3 direction() const { return (vec3)v2->vec - (vec3)v2->vec; }
	bool is_silhouette(const Pipeline& pipe) const;
	void set_polygon(Poly* p);
	void draw_edge(CDC *pDC, const mat4& proj_view_model_mat, const mat4& aspect_ratio_mat, COLORREF c);
	void draw_edge(CDC *pDC, const Pipeline& pipe, COLORREF c);

};
