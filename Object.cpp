#include "Object.h"
#include "cg.hpp"
#include "Poly.h"
#include "Edge.h"
#include "Vertex.h"
#include "Line.h"

void Object::draw_vertices(CDC *pDC, mat4 lens_mat, mat4 view_mat) const {
	for (auto v : vertices) {
		v->draw_vertex(pDC, lens_mat, view_mat, color);
	}
}

void Object::draw_wireframe(CDC *pDC, const mat4& transform_mat, const mat4& aspect_ratio_mat, bool backface_culling) const {
	const Pipeline& pipe = [=](const vec4& v) { return pipeline_transform(aspect_ratio_mat, transform_mat * get_model_mat(), v); };
	for (const auto p : polygons) {
		if (backface_culling && p->face_indicator(pipe) < 0)
			continue;
		for (const auto e : p->get_edges()) {
			e->draw_edge(pDC, transform_mat * get_model_mat(), aspect_ratio_mat, color);
		}
	}
}

void Object::draw_polygon_normals(CDC *pDC, ZBuffer& zbuffer, const mat4& transform_mat, const mat4& aspect_ratio_mat, COLORREF c, bool bf_culling, Renderer::Mode mode) const
{
	assert(!missing_polygon_normals_info);
	const Pipeline& pipe = [=](const vec4& v) { return pipeline_transform(aspect_ratio_mat, transform_mat * get_model_mat(), v); };

	for (auto p : polygons)
	{
		if (bf_culling && p->face_indicator(pipe) < 0)
			continue;

		vec3 start = pipe(p->center);
		vec3 end = pipe(p->center + p->get_normal() * 0.5);

		switch (mode) {
		case Renderer::WIREFRAME_MODE:
			Helpers::setLine(pDC, (int)start.x, (int)start.y, (int)end.x, (int)end.y, c);
			break;
		case Renderer::SCREEN_MODE:
			Helpers::setLine_z(zbuffer, start, end, c);
			break;
		}
	}
}

std::vector<Poly*> Object::bounding_polygons(const mat4& tm, const mat4& aspect_ratio_mat, vec3 point) {
	std::vector<Poly*> result;
	for (auto p : polygons) {
		std::vector<Line> ps{p->get_edges().cbegin(), p->get_edges().cend()};
		for (auto& l : ps) {
			vec3 w = (vec3)(aspect_ratio_mat*((tm * get_model_mat() * vec4( l.p-l.d, 1)).HomogenicDivision()));
			l.p = (vec3)(aspect_ratio_mat*((tm * get_model_mat() * vec4( l.p,     1 )).HomogenicDivision()));
			l.d = l.p - w;
		}

		Line lx{ vec3(0, point.y, 0), vec3(2.0f*aspect_ratio_mat.v4.x, point.y, 0) };
		Line ly{ vec3(point.x, 0, 0), vec3(point.x, 2.0f*aspect_ratio_mat.v4.y, 0) };
		int collision_count = 0;
		for (auto& l : ps) {
			if (line_intersection_2d(l, lx).are_intersecting)
				collision_count++;
			if (line_intersection_2d(l, ly).are_intersecting)
				collision_count++;
		}
		if (collision_count >= 4)
			result.push_back(p);
	}
	return result;
}

void Object::draw_vertex_normals(CDC *pDC, ZBuffer& zbuffer, const mat4& transform_mat, const mat4& aspect_ratio_mat, COLORREF c, bool bf_culling, Renderer::Mode mode) const
{
	assert(!missing_vertex_normals_info);
	const Pipeline& pipe = [=](const vec4& v) { return pipeline_transform(aspect_ratio_mat, transform_mat * get_model_mat(), v); };

	for (auto v : vertices)
	{
		vec3 start = pipe(v->vec);
		vec3 end = pipe((vec3)v->vec + v->get_normal() * 0.5);

		switch (mode) {
		case Renderer::WIREFRAME_MODE:
			Helpers::setLine(pDC, (int)start.x, (int)start.y, (int)end.x, (int)end.y, c);
			break;
		case Renderer::SCREEN_MODE:
			Helpers::setLine_z(zbuffer, start, end, c);
			break;
		}
	}
}


void Object::set_bbox() {
	double x_min, y_min, z_min;
	double x_max, y_max, z_max;
	x_min = y_min = z_min = DBL_MAX;
	x_max = y_max = z_max = -DBL_MAX;

	for (auto& v : vertices) {
		x_min = min(x_min, v->vec.x);
		y_min = min(y_min, v->vec.y);
		z_min = min(z_min, v->vec.z);
		x_max = max(x_max, v->vec.x);
		y_max = max(y_max, v->vec.y);
		z_max = max(z_max, v->vec.z);
	}

	bbox_min = vec3(x_min, y_min, z_min);
	bbox_max = vec3(x_max, y_max, z_max);

	center_of_mass = (bbox_max + bbox_min) / 2.0f;
}


void Object::transform_model_mat(const mat4& tm) {
	mat4 center_translation = mat4::Translate((vec3)(model_mat *  vec4(center_of_mass, 1)));
	mat4 reversed_center_translation = mat4::Translate((vec3)(model_mat *  vec4(center_of_mass, 1))*-1.0f);
	model_mat = center_translation * tm * reversed_center_translation * model_mat;
}

mat4 Object::get_model_mat() const {
	return model_mat;
}

void Object::draw_axises(CDC* pDC, const mat4& aspect_ratio_mat, const mat4& transform_mat, COLORREF colors[3], double scene_bbox_size) const
{
	double t = scene_bbox_size * 0.1;
	vec4 axis_directions[3] = { vec4(t, 0.0f, 0.0f, 0.0f),
								vec4(0.0f, t, 0.0f, 0.0f),
								vec4(0.0f, 0.0f, t, 0.0f) };

	mat4 tm = transform_mat * get_model_mat();

	for (int i = 0; i < 3; i++) {
		vec4 start{ center_of_mass,  1 };
		vec4 end = aspect_ratio_mat * (tm * (start + axis_directions[i])).HomogenicDivision();
		start = aspect_ratio_mat * (tm * start).HomogenicDivision();

		Helpers::setLine(pDC, (int)start.x, (int)start.y, (int)end.x, (int)end.y, colors[i]);
	}
}

void Object::draw_bbox(CDC* pDC, ZBuffer& zbuffer, const mat4& tm, const mat4& aspect_ratio_mat, COLORREF c, Renderer::Mode mode) const
{
	Pipeline pipe = [=](const vec4& v) { return pipeline_transform(aspect_ratio_mat, tm * get_model_mat(), v); };
	switch (mode) {
	case Renderer::WIREFRAME_MODE:
		Helpers::draw_box(pDC, pipe, c, get_bbox_min(), get_bbox_max());
		break;
	case Renderer::SCREEN_MODE:
		Helpers::draw_box_z(zbuffer, pipe, c, get_bbox_min(), get_bbox_max());
		break;
	}
}

VKey Object::get_vkey(vec3 vec) {
	vec *= 100000;
	long int key_x = static_cast<long int>(vec.x);
	long int key_y = static_cast<long int>(vec.y);
	long int key_z = static_cast<long int>(vec.z);
	return VKey(key_x, key_y, key_z);
}

void Object::hash_vertex(Vertex* v) {
	if (fetch_vertex(vec3(v->vec)) != nullptr)
		return;

	v_map[get_vkey((vec3)(v->vec))] = v;
}

void Object::insert_vertex(Vertex* v) {
	if (v == nullptr)
		return;
	hash_vertex(v);
	vertices.push_back(v);
}


Vertex* Object::insert_vertex(vec3 vec, bool& is_new) {
	Vertex* v = fetch_vertex(vec);
	if (v == nullptr) {
		is_new = true;
		v = new Vertex(vec);
		insert_vertex(v);
	}
	else
		is_new = false;
	return v;
}

Vertex* Object::fetch_vertex(vec3 vec) {
	auto v_it = v_map.find(get_vkey(vec));
	if (v_it != v_map.end()) {
		return v_it->second;
	}
	return nullptr;
}

void Object::hash_edge(Edge* e) {
	if (fetch_edge(*e) != nullptr)
		return;

	e_map[EKey(get_vkey((vec3)e->v1->vec),
		get_vkey((vec3)e->v2->vec))] = e;
}

Edge* Object::fetch_edge(const Edge& e) {
	vec3 v3_1 = (vec3)e.v1->vec;
	vec3 v3_2 = (vec3)e.v2->vec;

	auto e_it1 = e_map.find(EKey(get_vkey(v3_1), get_vkey(v3_2)));
	auto e_it2 = e_map.find(EKey(get_vkey(v3_2), get_vkey(v3_1)));
	if (e_it1 != e_map.end()) {
		return e_it1->second;
	}
	if (e_it2 != e_map.end()) {
		return e_it2->second;
	}
	return nullptr;
}

void Object::insert_edge(Edge* e) {
	if (e == nullptr)
		return;
	hash_edge(e);
	edges.push_back(e);
	e->id = edges.size() - 1;
}


Edge* Object::insert_edge(Vertex* v1, Vertex* v2, bool& is_new) {
	Edge* e = fetch_edge(Edge(v1, v2));
	if (e == nullptr) {
		e = new Edge(v1, v2);
		insert_edge(e);
		is_new = true;
	} else
		is_new = false;
	return e;
}

void Object::flip_normals(bool flip) {
	for (auto& p : polygons) {
		p->flip_normals = flip;
	}
	for (auto& v : vertices) {
		v->flip_normals = flip;
	}
}

void Object::calc_vertex_normals() {
	for (auto v : get_vertices()) {
		v->initialize_calc_normal();
	}
}