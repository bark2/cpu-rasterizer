#pragma once

#ifndef VERTEX_H
#define VERTEX_H

#include "stdafx.h"
#include <vector>
#include "vec4.hpp"
#include "vec3.hpp"
#include "mat4.h"
#include "Renderer.h"

class Poly;
class Edge;

class Vertex
{
	std::vector<Edge*> edges; // edges sharing the vertex
	std::vector<Poly*> polygons; // polygons sharing the vertex

public:
	vec4 vec;
	Renderer::NormalMode normal_mode = Renderer::PARS_NORMALS;
	vec3 pars_normal = { 0, 0, 0 }; // normal provided by the model
	vec3 calc_normal; // calculated normal

	bool flip_normals = false;

	Vertex(double x, double y, double z) : vec(x, y, z, 1) {}
	Vertex(const vec3& v) : vec(v) {}

	void set(double x, double y, double z);
	void set(const vec3& v);

	void insert_edge(Edge *e);
	void insert_polygon(Poly *p);
	const std::vector<Edge*>& get_edges() const;
	const std::vector<Poly*>& get_polygons() const;

	void draw_vertex(CDC * pDC, mat4 transform, mat4 screen, COLORREF c);

	void initialize_calc_normal(); // should be used only at parsing
	vec3 get_normal() const;
	vec3 Vertex::get_normal(const Pipeline& pipe) const;
};

#endif