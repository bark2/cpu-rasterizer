#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "vec3.hpp"
#include "Object.h"
#include <vector>
#include <utility>

struct Renderer;

class Scene
{
	std::vector<Object> objects;
	vec3 bbox_min, bbox_max;
	mat4 model_mat;
	mat4 scene_mat;
	mat4 view_mat;
	mat4 view2object_mat;
	vec3 center_of_mass;
	bool missing_polygon_normals_info = false;
	bool missing_vertex_normals_info = false;
	bool recalculated_polygon_normals = false;
	bool recalculated_vertex_normals = false;
	Renderer renderer;

	void init_view_mat();
	
public:
	struct {
		std::vector<Poly*> polygons;
		std::vector<Object*> objects_of_selected_polygons;
	} selected;
	mat4 proj_view_mat;
	vec3 camera_pos;
	vec3 camera_target;
	vec3 camera_up;

	int fineness = 20;
	Object* selected_obj = nullptr;

	bool bf_culling = false;

	Scene() :
		bbox_min(DBL_MAX, DBL_MAX, DBL_MAX),
		bbox_max(-DBL_MAX, -DBL_MAX, -DBL_MAX),
		proj_view_mat(mat4::Identity()),
		model_mat(mat4::Identity()),
		scene_mat(mat4::Identity()),
		curr_space(Space::VIEW),
		center_of_mass({ 0, 0, 0 }),
		camera_pos({ 0, 0, 2.0f }),
		camera_target({ 0, 0, 0 }),
		camera_up({ 0, 1, 0 }),
		renderer()
	{
		init_view_mat();
	}

	~Scene();
	Scene& operator=(Scene&& rhs);

	mat4 get_view2object_mat() const;
	const mat4& get_model_mat() const;
	void transform_scene_mat(const mat4& mat, const mat4& mat_inv);
	void set_model_mat(const mat4& mat);
	void transform_model_mat(const mat4& mat);

	vec3 get_bbox_min() const { return bbox_min; }
	vec3 get_bbox_max() const { return bbox_max; }

	void Scene::set_bbox();

	void push_back(const Object& obj);

	void set_ortho_proj();
	void set_persp_proj(double d, double alpha);
	void norm_model();
	void validate_normals_info();
	void set_edge_color(COLORREF color);
	std::pair<Object*, Poly*> get_nearest_bounding_obj(CRect screen, int x, int y);
	void select_polygon(Poly* p);

	void draw_wireframe(CDC* pDC, CRect rect) const;
	void draw_wireframe_(ZBuffer& zbuffer, CRect screen);
	void draw_polygons(ZBuffer& zbuffer, const CRect& screen, const std::vector<Light>& lights, int specular_exp) const;
	void draw_light_sources(CDC* pDC, ZBuffer& zbuffer, const CRect& screen, const std::vector<Light>& lights, Renderer::Mode mode) const;
	void draw_polygon_normals(CDC* pDC, ZBuffer& zbuffer, CRect screen, COLORREF c, Renderer::Mode mode) const;
	void draw_vertex_normals(CDC* pDC, ZBuffer& zbuffer, CRect screen, COLORREF c, Renderer::Mode mode) const;
	void draw_bbox(CDC* pDC, ZBuffer& zbuffer, CRect screen, COLORREF c, Renderer::Mode mode) const;
	void draw_axises(CDC* pDC, const mat4& aspect_ratio_mat, COLORREF axis_colors[3]) const;
	void draw_selected_polygons(CDC* pDC, CRect rect, COLORREF color) const;

	enum class Space { VIEW, OBJECT };

	Space get_curr_space();
	Pipeline obj_pipeline(CRect screen, const mat4& obj_mat) const;

	static mat4 Scene::get_aspect_ratio_mat(CRect screen);
	Space curr_space;

	vec3 get_center_of_mass() const;

	void insert_polygon(Poly* p) { polygons.emplace_back(p); }
	void insert_edge(Edge* e) { edges.emplace_back(e); }
	void insert_vertex(Vertex* v) {	vertexes.emplace_back(v); }

	void flip_normals(bool flip);
	void change_normal_mode(Renderer::NormalMode mode);

	void set_shading(Renderer::Shading s);
	void draw_silhouette(ZBuffer& zbuffer, CRect screen);

private:
	std::vector<Poly*> polygons;
	std::vector<Edge*> edges;
	std::vector<Vertex*> vertexes;
};

#endif