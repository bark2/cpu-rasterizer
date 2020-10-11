#include "Vertex.h"
#include "mat4.h"
#include "vec4.hpp"
#include "Poly.h"

void Vertex::set(double x, double y, double z) { vec = vec3(x, y, z); }
void Vertex::set(const vec3& v) { vec = v; }

void Vertex::insert_edge(Edge *e) {
	for (auto curr_e : edges) {
		if (curr_e == e) return;
	}
	edges.push_back(e);
}
void Vertex::insert_polygon(Poly *p) {
	for (auto curr_p : polygons) {
		if (curr_p == p) return;
	}
	polygons.push_back(p); 
}
const std::vector<Edge*>& Vertex::get_edges() const { return edges; }
const std::vector<Poly*>& Vertex::get_polygons() const { return polygons; }


void Vertex::draw_vertex(CDC * pDC, mat4 lens_mat, mat4 view_mat, COLORREF c) {
	vec4 t_vec = lens_mat * vec;
	t_vec.HomogenicDivision();
	t_vec = view_mat * t_vec;
	pDC->SetPixel((int)t_vec.x, (int)t_vec.y, c);
}

void Vertex::initialize_calc_normal()
{
	vec3 normal(0, 0, 0);
	for (auto p : get_polygons()) {
		normal += p->calc_normal;
	}

	calc_normal = UnitVector(normal / get_polygons().size());
}

vec3 Vertex::get_normal() const {
	switch (normal_mode) {
	case Renderer::PARS_NORMALS:
		return pars_normal * (flip_normals ? -1 : 1);
	case(Renderer::CALC_NORMALS):
		return calc_normal * (flip_normals ? -1 : 1);
	}
}

vec3 Vertex::get_normal(const Pipeline& pipe) const
{
	vec3 pos = pipe(vec);
	vec3 normpos;

	switch (normal_mode) {
	case Renderer::PARS_NORMALS:
		normpos = pipe((vec3)vec + pars_normal);
		break;
	case(Renderer::CALC_NORMALS):
		normpos = pipe((vec3)vec + calc_normal);
		break;
	}
	return (normpos - pos).Normalize() * (flip_normals ? -1 : 1);
}