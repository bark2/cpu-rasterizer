#include <algorithm>
#include <utility>
#include "cg.hpp"
#include "Scene.h"
#include "Poly.h"
#include "Edge.h"
#include "Vertex.h"
#include "Renderer.h"
#include "ZBuffer.h"

Scene& Scene::operator=(Scene&& rhs)
{
	missing_polygon_normals_info = false;
	missing_vertex_normals_info = false;
	recalculated_polygon_normals = false;
	recalculated_vertex_normals = false;
	bbox_min = { DBL_MAX, DBL_MAX, DBL_MAX };
	bbox_max = { -DBL_MAX, -DBL_MAX, -DBL_MAX };
	proj_view_mat = mat4::Identity();
	model_mat = mat4::Identity();
	scene_mat = mat4::Identity();
	view2object_mat = mat4::Identity();
	curr_space = Space::VIEW;
	center_of_mass = { 0, 0, 0 };
	camera_pos = { 0, 0, 2.0f };
	camera_target = { 0, 0, 0 };
	camera_up = { 0, 1, 0 };
	std::swap(objects, rhs.objects);
	polygons.swap(std::vector<Poly*>());
	edges.swap(std::vector<Edge*>());
	vertexes.swap(std::vector<Vertex*>());
	renderer = std::move(rhs.renderer);
	return *this;
}

Scene::~Scene() {
	for (auto& obj : objects) {
		for (auto p : obj.get_polygons()) {
			delete p;
		}
		for (auto e : obj.get_edges()) {
			delete e;
		}
		for (auto v : obj.get_vertices()) {
			delete v;
		}
	}
}

void Scene::set_bbox() {

	bbox_min = vec3(DBL_MAX, DBL_MAX, DBL_MAX);
	bbox_max = vec3(-DBL_MAX, -DBL_MAX, -DBL_MAX);


	for (const auto& obj : objects)
	{
		vec4 obj_bbox_min = obj.get_model_mat() * obj.get_bbox_min();
		vec4 obj_bbox_max = obj.get_model_mat() * obj.get_bbox_max();

		bbox_min.x = min(bbox_min.x, obj_bbox_min.x);
		bbox_min.y = min(bbox_min.y, obj_bbox_min.y);
		bbox_min.z = min(bbox_min.z, obj_bbox_min.z);
		bbox_max.x = max(bbox_max.x, obj_bbox_max.x);
		bbox_max.y = max(bbox_max.y, obj_bbox_max.y);
		bbox_max.z = max(bbox_max.z, obj_bbox_max.z);
	}
	center_of_mass = (bbox_min + bbox_max) / 2;
}

void Scene::push_back(const Object& obj) {
	if (obj.missing_polygon_normals_info)
		missing_polygon_normals_info = true;
	if (obj.missing_vertex_normals_info)
		missing_vertex_normals_info = true;
	objects.push_back(obj);
}

void Scene::set_ortho_proj() {
	// fav_v and near_v values are treated as negetives in the projection.
	proj_view_mat = mat4::Ortho(1, 1, 0.3333f, 6.0f) * view_mat;
}

void Scene::set_persp_proj(double near_v, double far_v) {
	// fav_v and near_v values are treated as negetives in the projection.
	auto normalize = [](double z) { return (z - 12.5f) / 7.5; };
	double mid = camera_pos.z + (normalize(far_v)+normalize(near_v)) / 2.0f;
	proj_view_mat = mat4::Scale(mid, mid, 1) * mat4::Persp_d(0.3333f, 6.0f) * view_mat;
	/*
	vec4 tt1 = (proj_view_mat * vec4(10, 1, 0.5)).HomogenicDivision();
	vec4 tt2 = (proj_view_mat * vec4(-10, -1, -0.5)).HomogenicDivision();
	vec4 tt3 = (proj_view_mat * vec4(0, 0, 1)).HomogenicDivision();
	vec4 tt4 = (proj_view_mat * vec4(0, 0, -1)).HomogenicDivision();
	vec4 tt5 = (proj_view_mat * vec4(0.5, -0.5, 0.5)).HomogenicDivision();
	vec4 tt6 = (proj_view_mat * vec4(0.5, 0.5, 0.5)).HomogenicDivision();
	*/
}

void Scene::norm_model() {
	double len = (bbox_max - bbox_min).Length() * 1.2;
	set_model_mat(mat4::Scale(1 / len, 1 / len, 1 / len) * mat4::Translate(-center_of_mass));
	view2object_mat = view2object_mat * mat4::Translate(center_of_mass) * mat4::Scale(len, len, len);
}

void Scene::init_view_mat() {
	mat4 result;
	vec3 new_z_axis = (camera_pos - camera_target).Normalize();
	vec3 new_y_axis = camera_up.Normalize();
	vec3 new_x_axis = Cross(new_y_axis, new_z_axis).Normalize();
	vec4 v1{ new_x_axis, 0 };
	vec4 v2{ new_y_axis, 0 };
	vec4 v3{ new_z_axis, 0 };
	vec4 v4{ 0.0f, 0.0f, 0.0f, 1.0f };

	view_mat = mat4(v1, v2, v3, v4) * mat4::Translate(-camera_pos);
}

mat4 Scene::get_view2object_mat() const {
	return view2object_mat * mat4::Translate(-camera_pos);
}

mat4 Scene::get_aspect_ratio_mat(CRect screen) {
	int w = screen.Width();
	int h = screen.Height();
	h = w = (double)h / w > 1 ? w : h;

	return
		mat4::Translate(screen.Width() / 2.0, screen.Height() / 2.0, 0) * mat4::Scale(w, -h, 1);
}

const mat4& Scene::get_model_mat() const {
	return model_mat;
}

void Scene::transform_scene_mat(const mat4& mat, const mat4& mat_inv) {
	scene_mat = mat * scene_mat;
	view2object_mat = view2object_mat * mat_inv;
}

void Scene::set_model_mat(const mat4& mat) {
	model_mat = mat;
}

void Scene::transform_model_mat(const mat4& tm) {
	if (selected_obj != nullptr) {
		selected_obj->transform_model_mat(tm);
	}
	else {
		mat4 center_translation = mat4::Translate((vec3)(model_mat *  vec4(center_of_mass, 1)));
		mat4 reversed_center_translation = mat4::Translate((vec3)(model_mat *  vec4(center_of_mass, 1))*-1.0f);
		model_mat = center_translation * tm * reversed_center_translation * model_mat;
	}
	set_bbox();
}

Pipeline Scene::obj_pipeline(CRect screen, const mat4& obj_mat) const {
	return [=](const vec4& v) {
		return pipeline_transform(get_aspect_ratio_mat(screen), proj_view_mat * scene_mat * get_model_mat() * obj_mat, v);
	};
}

void Scene::draw_wireframe(CDC* pDC, CRect screen) const {
	mat4 aspect_ratio_mat = get_aspect_ratio_mat(screen);
	COLORREF axises_colors[3] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };

	for (auto& obj : objects) {
		obj.draw_wireframe(pDC, proj_view_mat * scene_mat * get_model_mat(), aspect_ratio_mat, bf_culling);
	}

	if (curr_space == Space::VIEW)
		return;

	if (selected_obj == nullptr && objects.size() != 0) {
		draw_axises(pDC, aspect_ratio_mat, axises_colors);
	}
	else {
		vec3 obj_size = selected_obj->get_bbox_max() - selected_obj->get_bbox_min();
		double t = max(max(obj_size.x, obj_size.y), obj_size.z);
		mat4 axises_transformation{ proj_view_mat * scene_mat * get_model_mat() };
		selected_obj->draw_axises(pDC, aspect_ratio_mat, axises_transformation, axises_colors, t);
	}
}

void Scene::draw_wireframe_(ZBuffer& zbuffer, CRect screen) {
	mat4 aspect_ratio_mat = get_aspect_ratio_mat(screen);
	mat4 proj_view_model = proj_view_mat * scene_mat * get_model_mat();
	if (!edges.size())
		return;
	for (auto& obj : objects) {
		auto obj_pipe = [=](const vec4& v) { return pipeline_transform(proj_view_model * obj.get_model_mat(), v); };
		//renderer.DrawWireframes_NonLinear(zbuffer, obj.get_edges(), edges, polygons, screen.Width(), screen.Height(), obj_pipe, {});
//		renderer.DrawWireframes_(zbuffer, obj.get_edges(), edges, polygons, screen.Width(), screen.Height(), obj_pipe, {});
	}
}

void Scene::draw_polygons(ZBuffer& zbuffer, const CRect& screen, const std::vector<Light>& lights, int specular_exp) const {
	mat4 aspect_ratio_mat = get_aspect_ratio_mat(screen);
	mat4 view_model = scene_mat * get_model_mat();
	mat4 proj_view_model = proj_view_mat * view_model;

	for (const auto& obj : objects) {
		const Pipeline& screen_pipe = [=](const vec4& v) { return pipeline_transform(aspect_ratio_mat, proj_view_model * obj.get_model_mat(), v); };
		const Pipeline& obj_pipe = [=](const vec4& v) { return pipeline_transform(mat4::Identity(), get_model_mat() * obj.get_model_mat(), v);  };
		renderer.DrawPolygons(zbuffer, screen_pipe, obj_pipe, obj, (vec3)(get_view2object_mat() * camera_pos), lights, bf_culling, specular_exp);
	}
}

void Scene::draw_light_sources(CDC* pDC, ZBuffer& zbuffer, const CRect& screen, const std::vector<Light>& lights, Renderer::Mode mode) const
{
	mat4 m = proj_view_mat * scene_mat;

	for (const Light& light : lights) {
		vec3 light_d, light_p;
		switch (light.type) {
		case LIGHT_TYPE_AMBIENT:
			continue;
		case LIGHT_TYPE_DIRECTIONAL:
			light_p = UnitVector(-light.dir) / 2;
			light_d = light.dir; break;
		case LIGHT_TYPE_POINT:
			light_p = light.pos;
			light_d = { 0,0,1 }; break;
		case LIGHT_TYPE_SPOT:
			light_p = light.pos;
			light_d = light.dir; break;
		}

		vec3 new_z_axis = light_d.Length() > 0 ? UnitVector(light_d) : vec3{ 0, 0, 1 };
		vec3 new_y_axis;
		if (abs(vec3(0, 0, 1) * new_z_axis) == 1) {
			new_y_axis = vec3{ 0, 1, 0 };
		}
		else {
			new_y_axis = Cross(vec3(0, 0, 1), new_z_axis).Normalize();
		}
		vec3 new_x_axis = Cross(new_y_axis, new_z_axis).Normalize();
		vec4 v1{ new_x_axis, 0 };
		vec4 v2{ new_y_axis, 0 };
		vec4 v3{ new_z_axis, 0 };
		vec4 v4{ 0.0f, 0.0f, 0.0f, 1.0f };
		mat4 rotation = mat4(v1, v2, v3, v4);

		if (light.space == LIGHT_SPACE_LOCAL) {
			double len = (bbox_max - bbox_min).Length() * 1.2;
			mat4 normal_model = mat4::Translate(center_of_mass) * mat4::Scale(len, len, len);
			m = m * get_model_mat() * normal_model * mat4::Translate(light_p) * rotation;
		}
		else {
			m = m * mat4::Translate(light_p) * rotation;
		}

		Pipeline pipe = [=](const vec4& v) { return pipeline_transform(get_aspect_ratio_mat(screen), m, v); };
		vec3 start = pipe({ 0, 0, 0 });
		vec3 end = pipe({ 0, 0, 0.1 });

		COLORREF c = light.color.get_rgb();

		switch (mode) {
		case Renderer::WIREFRAME_MODE:
			Helpers::draw_box(pDC, pipe, c, { -0.01, -0.01, 0.01 }, { 0.01, 0.01, -0.01 });
			if (light.type != LIGHT_TYPE_POINT) Helpers::setLine(pDC, start.x, start.y, end.x, end.y, c);
			break;
		case Renderer::SCREEN_MODE:
			Helpers::draw_box_z(zbuffer, pipe, c, { -0.01, -0.01, 0.01 }, { 0.01, 0.01, -0.01 });
			if (light.type != LIGHT_TYPE_POINT) Helpers::setLine_z(zbuffer, start, end, c);
			break;
		}
	}
}

void Scene::draw_polygon_normals(CDC* pDC, ZBuffer& zbuffer, CRect screen, COLORREF c, Renderer::Mode mode) const {
	mat4 aspect_ratio_mat = get_aspect_ratio_mat(screen);
	for (auto& obj : objects) {
		obj.draw_polygon_normals(pDC, zbuffer, proj_view_mat * scene_mat * get_model_mat(), aspect_ratio_mat, c, bf_culling, mode);
	}
}

void Scene::draw_vertex_normals(CDC* pDC, ZBuffer& zbuffer, CRect screen, COLORREF c, Renderer::Mode mode) const {
	mat4 aspect_ratio_mat = get_aspect_ratio_mat(screen);
	for (auto& obj : objects) {
		obj.draw_vertex_normals(pDC, zbuffer, proj_view_mat * scene_mat * get_model_mat(), aspect_ratio_mat, c, bf_culling, mode);
	}
}

void Scene::draw_axises(CDC* pDC, const mat4& aspect_ratio_mat, COLORREF colors[3]) const
{
	mat4 transform_mat = proj_view_mat * scene_mat * get_model_mat();

	vec3 bbox_size = bbox_max - bbox_min;
	double t = max(max(bbox_max.x - bbox_min.x, bbox_max.y - bbox_min.y), bbox_max.z - bbox_min.z);

	vec4 axis_directions[3] = { vec4(0.1 * t, 0.0f, 0.0f, 0.0f),
									vec4(0.0f, 0.1 * t, 0.0f, 0.0f),
									vec4(0.0f, 0.0f, 0.1 * t, 0.0f) };
	for (int i = 0; i < 3; i++) {
		vec4 start{ center_of_mass,  1 };
		vec4 end = aspect_ratio_mat * (transform_mat * (start + axis_directions[i])).HomogenicDivision();
		start = aspect_ratio_mat * (transform_mat * start).HomogenicDivision();

		Helpers::setLine(pDC, (int)start.x, (int)start.y, (int)end.x, (int)end.y, colors[i]);
	}
}

void Scene::draw_bbox(CDC* pDC, ZBuffer& zbuffer, CRect screen, COLORREF c, Renderer::Mode mode) const
{
	mat4 aspect_ratio_mat = get_aspect_ratio_mat(screen);
	mat4 m = proj_view_mat * scene_mat * get_model_mat();

	Pipeline pipe = [=](const vec4& v) { return pipeline_transform(aspect_ratio_mat, m, v); };

	if (selected_obj == nullptr) {
		switch (mode) {
		case Renderer::WIREFRAME_MODE:
			Helpers::draw_box(pDC, pipe, c, get_bbox_min(), get_bbox_max());
			break;
		case Renderer::SCREEN_MODE:
			Helpers::draw_box_z(zbuffer, pipe, c, get_bbox_min(), get_bbox_max());
			break;
		}
	}
	else if (selected_obj != nullptr && curr_space == Space::OBJECT){
		selected_obj->draw_bbox(pDC, zbuffer, m, aspect_ratio_mat, c, mode);
	}
}

/* ERROR: selected polygons arent being colored. */
void Scene::draw_selected_polygons(CDC* pDC, CRect screen, COLORREF color) const {
	//for (auto p : selected_polygons) {
	for (unsigned int i = 0; i < selected.polygons.size(); i++) {
		for (auto e : selected.polygons[i]->get_edges()) {
			mat4 transform_mat = proj_view_mat * scene_mat * get_model_mat() * selected.objects_of_selected_polygons[i]->get_model_mat();
			e->draw_edge(pDC, transform_mat, get_aspect_ratio_mat(screen), color);
		}
	}
}

void Scene::select_polygon(Poly* p) {
	selected.polygons.push_back(p);
}

std::pair<Object*,Poly*> Scene::get_nearest_bounding_obj(CRect screen, int x, int y) {
	Object* result = nullptr;
	Poly* min_polygon = nullptr;
	mat4 tm = proj_view_mat * scene_mat * get_model_mat();
	mat4 screen_mat = get_aspect_ratio_mat(screen);

	for (auto& obj : objects) {

		auto polygons = obj.bounding_polygons(tm, screen_mat, vec3(x, y, 0));
		if (polygons.empty())
			continue;

		Poly* curr_min_p = polygons.front();
		for (unsigned int i = 0; i < polygons.size(); i++) {
			if (polygons[i]->get_z_plane_line_collision(tm * obj.get_model_mat(), screen_mat, x, y, -2, vec3(0.0f, 0.0f, 1.0f)) <
				curr_min_p->get_z_plane_line_collision(tm * obj.get_model_mat(),  screen_mat, x, y, -2, vec3(0.0f, 0.0f, 1.0f)))

			{
				curr_min_p = polygons[i];
			}
		}
		if (min_polygon == nullptr) 
		{
			min_polygon = curr_min_p;
			result = &obj;
		}
		else if (curr_min_p->get_z_plane_line_collision(tm * obj.get_model_mat(), screen_mat, x, y, -2, vec3(0.0f, 0.0f, 1.0f)) <
			min_polygon->get_z_plane_line_collision(tm * obj.get_model_mat(),     screen_mat, x, y, -2, vec3(0.0f, 0.0f, 1.0f)))
		{
			min_polygon = curr_min_p;
			result = &obj;
		}
	}

	return std::make_pair(result, min_polygon); // could be (nullptr,nullptr)
}

void Scene::set_edge_color(COLORREF color) {
	for (auto& obj : objects) {
		obj.set_color(color);
	}
}

Scene::Space Scene::get_curr_space() {
	return curr_space;
}

vec3 Scene::get_center_of_mass() const {
	return center_of_mass;
}

void Scene::validate_normals_info() {
	if (missing_polygon_normals_info) {
		for (auto& obj : objects) {
			if (obj.missing_polygon_normals_info) {
				recalculated_polygon_normals = true;
			}
		}
	}
		
	if (missing_vertex_normals_info) {
		for (auto& obj : objects) {
			if (obj.missing_vertex_normals_info) {
				recalculated_vertex_normals = true;
			}
		}
	}
}

void Scene::flip_normals(bool flip) {
	for (auto& obj : objects) {
		obj.flip_normals(flip);
	}
}

void Scene::change_normal_mode(Renderer::NormalMode mode) {
	for (auto& obj : objects) {
		for (auto p : obj.get_polygons()) {
			p->normal_mode = mode;
		}
		for (auto v : obj.get_vertices()) {
			v->normal_mode = mode;
		}
	}
}

void Scene::set_shading(Renderer::Shading s) {
	renderer.shading = s;
}

void Scene::draw_silhouette(ZBuffer& zbuffer, CRect screen) {
	for (auto& obj : objects) {
		for (auto e : obj.get_edges()) {
			auto obj_pipe = obj_pipeline(screen, obj.get_model_mat());
			if (e->is_silhouette(obj_pipe)) {
				vec3 v1 = obj_pipe(e->v1->vec);
				vec3 v2 = obj_pipe(e->v2->vec);
				v1.z = 1000; v2.z = 1000;
				for (int i = -1; i <= 1; ++i) {
					vec3 curr_v1 = v1;
					vec3 curr_v2 = v2;
					if (v1.x != v2.x) {
						double grad = (v2.y - v1.y) / (v2.x - v1.x);
						if (abs(grad) > 1) {
							curr_v1.x += i;
							curr_v2.x += i;
						}
						else {
							curr_v1.y += i;
							curr_v2.y += i;
						}
					}
					else {
						curr_v1.y += i;
						curr_v2.y += i;
					}
					Helpers::setLine_z(zbuffer, curr_v1, curr_v2, RGB(255, 0, 255)); // TODO:: change to inverse of bg color
				}
			}
		}
	}
}