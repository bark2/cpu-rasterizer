#include <algorithm>
#include "Poly.h"
#include "Vertex.h"
#include "Line.h"

void Poly::insert_vertex(Vertex * v) {
	if (!v) return;
	
	for (auto curr_v : vertices) {
		if (curr_v == v) return;
	}

	vertices.push_back(v);
}
void Poly::insert_edge(Edge * e, bool is_new) {
	if (!e)
		return;
	for (auto curr_e : edges) {
		if (curr_e == e) return;
	}
	edges.push_back(e);
	flipped.push_back(!is_new);
}

double Poly::get_z_plane_line_collision(const mat4& screen, const mat4& tm, double x, double y, double z, vec3 l) const {
	double result;
	Edge* e1 = get_edges()[0];
	vec3 v1 = (vec3)pipeline_transform(screen, tm, e1->v1->vec);
	vec3 n =  (vec3)pipeline_transform(screen, tm, get_normal());
	vec3 l0{ x, y, z };
	if (l*n == 0)
		result =  DBL_MAX;
	else 
		result = ((v1 - l0) * n) / (l * n);

	return result;
}

const Vertex* Poly::min_y_vertex(const Pipeline& pipe) const {
	const Vertex* result = nullptr;
	double min_y = DBL_MAX;
	for (const auto v : vertices) {
		double curr_y = pipe(v->vec).y;
		if (curr_y < min_y) {
			min_y = curr_y;
			result = v;
		}
	}
	return result;
}

const Vertex* Poly::max_y_vertex(const Pipeline& pipe) const {
	const Vertex* result = nullptr;
	double max_y = -DBL_MAX;
	for (const auto v : vertices) {
		double curr_y = pipe(v->vec).y;
		if (curr_y > max_y) {
			max_y = curr_y;
			result = v;
		}
	}
	return result;
}

const Vertex* Poly::min_x_vertex(const Pipeline& pipe) const {
	const Vertex* result = nullptr;
	double min_x = DBL_MAX;
	for (const auto v : vertices) {
		double curr_x = pipe(v->vec).x;
		if (curr_x < min_x) {
			min_x = curr_x;
			result = v;
		}
	}
	return result;
}

const Vertex* Poly::max_x_vertex(const Pipeline& pipe) const {
	const Vertex* result = nullptr;
	double max_x = -DBL_MAX;
	for (const auto v : vertices) {
		double curr_x = pipe(v->vec).x;
		if (curr_x > max_x) {
			max_x = curr_x;
			result = v;
		}
	}
	return result;
}

double Poly::face_indicator(const Pipeline& pipe) const {
	return get_normal(pipe).z;
}

double Poly::face_indicator(const Pipeline& pipe, vec3 from) const {
	double result;
	vec3 normal{ get_normal(pipe) };
	double d = pipe(vertices[0]->vec) * normal;
	// vec3 from = pipe(from);
	result = normal * from + d;
	return result;
}


std::vector<Line> Poly::transform(const Pipeline& pipe) const {
	std::vector<Line> result;
	result.reserve(edges.size());
	
	for (unsigned int i = 0; i < edges.size(); i++)
		result.emplace_back(edges[i], flipped[i], pipe);

	return result;
}


double Poly::get_z_at_point(double x, double y, const Pipeline& pipe) const {
	double result;
	vec3 snormal = get_normal(pipe);
	double d = snormal * pipe(vertices.front()->vec);
	result = (snormal.x * x + snormal.y * y + d) / (-1 * snormal.z);
	return result;
}

bool Poly::point_inside_projected_polygon(const Pipeline& pipe, double x, double y)
{
	std::vector<Line> polygon_lines = transform(pipe);
	Line lx{ vec3(x, y, 0), vec3(10.0f, y, 0) }; // Convex Polygon
	int collision_count = 0;
	for (auto& l : polygon_lines) {
		auto intersection = line_intersection_2d(l, lx);
		if (intersection.are_intersecting)
			collision_count++;
	}
	return collision_count == 1;
}

const std::vector<Vertex*>& Poly::get_vertices() const { return vertices; }
const std::vector<Edge*>& Poly::get_edges() const { return edges; }


void Poly::initialize_calc_normal()
{
	if (vertices.size() < 3) return;
	vec3 v1 = (vec3)vertices[0]->vec;
	vec3 v2 = (vec3)vertices[1]->vec;
	vec3 v3 = (vec3)vertices[2]->vec;

	Line e1{ v1, v2 };
	Line e2{ v1, v3 };

	calc_normal = UnitVector(Cross(e1.d, e2.d));
}

vec3 Poly::get_normal() const {
	switch (normal_mode) {
	case Renderer::PARS_NORMALS:
		return pars_normal * (flip_normals ? -1 : 1);
	case(Renderer::CALC_NORMALS):
		return calc_normal * (flip_normals ? -1 : 1);
	}
}

vec3 Poly::get_normal(const Pipeline& pipe) const
{
	vec3 pos = pipe(center);
	vec3 normpos;

	switch (normal_mode) {
	case Renderer::PARS_NORMALS:
		normpos = pipe(center + pars_normal);
		break;
	case(Renderer::CALC_NORMALS):
		normpos = pipe(center + calc_normal);
		break;
	}
	return (normpos - pos).Normalize() * (flip_normals ? -1 : 1);
}
