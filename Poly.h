#pragma once

#include "StdAfx.h"
#include <vector>
#include <utility>

#include "vec3.hpp"
#include "mat4.h"
#include "Vertex.h"
#include "Edge.h"
#include "Renderer.h"

struct Line;

class Poly
{
	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
	std::vector<bool> flipped;

public:
	Renderer::NormalMode normal_mode = Renderer::PARS_NORMALS;
	vec3 pars_normal; // normal provided by the model
	vec3 calc_normal; // calculated normal
	vec3 center;

	bool flip_normals = false;

	Poly() = default;
	~Poly() = default;
	void insert_vertex(Vertex * v);
	void insert_edge(Edge * e, bool is_new);

	const std::vector<Vertex*>& get_vertices() const;
	const std::vector<Edge*>& get_edges() const;

	double get_z_plane_line_collision(const mat4& screen, const mat4& tm, double x, double y, double z, vec3 l) const;
	double face_indicator(const Pipeline& pipe) const;
	double face_indicator(const Pipeline& pipe, vec3 from) const;
	std::vector<Line> Poly::transform(const Pipeline& pipe) const;

	void initialize_calc_normal(); // should be used only at parsing
	vec3 get_normal() const;
	vec3 get_normal(const Pipeline& pipe) const;

	double get_z_at_point(double x, double y, const Pipeline& pipe) const;
	bool point_inside_projected_polygon(const Pipeline& pipe, double x, double y);

	const Vertex* min_y_vertex(const Pipeline& pipe) const;
	const Vertex* max_y_vertex(const Pipeline& pipe) const;
	const Vertex* min_x_vertex(const Pipeline& pipe) const;
	const Vertex* max_x_vertex(const Pipeline& pipe) const;
};

