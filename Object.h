#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "StdAfx.h"
#include <vector>
#include <map>
#include <tuple>
#include <memory>
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.h"
#include "Poly.h"
#include "ZBuffer.h"
#include "Renderer.h"

class Edge;
class Vertex;

typedef std::tuple<long int, long int, long int> VKey;
typedef std::map<VKey, Vertex*> VMap;
typedef std::tuple<VKey, VKey> EKey;
typedef std::map<EKey, Edge*> EMap;

class Object
{
	std::vector<Poly*> polygons;
	std::vector<Edge*> edges;
	std::vector<Vertex*> vertices;
	
	COLORREF color;
	vec3 bbox_min, bbox_max;

public:
	mat4 model_mat;
	mat4 model_to_scene;
	vec3 center_of_mass = { 0, 0, 0 };
	bool missing_polygon_normals_info = false;
	bool missing_vertex_normals_info = false;

	std::vector<Poly*> get_polygons() const { return polygons; }
	std::vector<Edge*> get_edges() const { return edges; }
	std::vector<Vertex*> get_vertices() const { return vertices; }
	COLORREF get_color() const { return color; }
	vec3 get_bbox_min() const { return bbox_min; }
	vec3 get_bbox_max() const { return bbox_max; }
	mat4 get_model_mat() const;

	void insert_polygon(Poly* p) { polygons.emplace_back(p); }
	void hash_vertex(Vertex* v);
	void insert_vertex(Vertex* v);
	Vertex* insert_vertex(vec3 vec, bool& is_new);
	Vertex* fetch_vertex(vec3 vec);
	Edge* fetch_edge(const Edge& e);
	void insert_edge(Edge* e);
	Edge* insert_edge(Vertex* v1, Vertex* v2, bool& is_new);
	VMap v_map;
	EMap e_map;
	void hash_edge(Edge* e);
	static VKey get_vkey(vec3 vec);

	void set_color(COLORREF c) { color = c; }

	void draw_vertices(CDC *pDC, mat4 lens_mat, mat4 view_mat) const;

	void Object::draw_wireframe(CDC *pDC, const mat4& transform_mat, const mat4& aspect_ratio_mat, bool backface_culling) const;
	void draw_polygon_normals(CDC *pDC, ZBuffer& zbuffer, const mat4& transform_mat, const mat4& aspect_ratio_mat, COLORREF c, bool bf_culling, Renderer::Mode mode) const;
	void draw_vertex_normals(CDC *pDC, ZBuffer& zbuffer, const mat4& transform_mat, const mat4& aspect_ratio_mat, COLORREF c, bool bf_culling, Renderer::Mode mode) const;
	void draw_bbox(CDC* pDC, ZBuffer& zbuffer, const mat4& tm, const mat4& aspect_ratio_mat, COLORREF c, Renderer::Mode mode) const;

	std::vector<Poly*> bounding_polygons(const mat4& tm, const mat4& aspect_ratio_mat, vec3 point);
	void set_bbox();
	void transform_model_mat(const mat4& tm);

	void draw_axises(CDC* pDC, const mat4& aspect_ratio_mat, const mat4& transform_mat, COLORREF colors[3], double scene_bbox_size) const;

	void flip_normals(bool flip);
	void calc_vertex_normals();
};

#endif