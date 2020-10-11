#include "Renderer.h"
#include <algorithm>
#include <utility>
#include <cmath>
#include "Poly.h"
#include "Object.h"
#include "Vertex.h"
#include "vec4.hpp"
#include "Edge.h"
#include "Line.h"
#include "cg.hpp"
#include <unordered_map>

static int calc_point_qv(vec3 v, Edge* e, const std::vector<Poly*> polygons, unsigned int sw, unsigned int sh, const Pipeline& pipe)
{
	int result = 0;
	for (auto p : polygons) {
		if (e->p1 == p || e->p2 == p)
			continue;
		if (p->face_indicator(pipe) < 0)
			continue;

		double polygon_z = p->get_z_at_point(v.x, v.y, pipe);
		if (polygon_z <= v.z)
			continue;
		if (p->point_inside_projected_polygon(pipe, v.x, v.y))
			return ++result;
	}
	return result;
}

#if 0
void Renderer::DrawWireframes(CDC* pDC, const std::vector<Edge*>& edges, const std::vector<Poly*> polygons, 
	unsigned int sw, unsigned int sh, const Pipeline& pipe, const std::vector<LightParams>& light_sources) 
{
	std::vector<std::pair<Line, int>> lines;
	lines.reserve(edges.size());
	for (unsigned int i = 0; i < edges.size(); i++)
		lines.emplace_back(std::make_pair(Line(edges[i], pipe), -1));

	// should be recalculating the normals
	std::vector<Line> active;
	vec3 light_position = { 0.0f, 0.0f, 2.0f };
	for (unsigned int i = 0; i < edges.size(); i++) {
		Edge* e = edges[i];
		if (e->p2 == nullptr) {
			active.emplace_back(Line(e, pipe));
		}
		else {
			double is_boundery = e->p1->face_indicator(light_position, pipe) *
				                 e->p2->face_indicator(light_position, pipe);
			if (is_boundery == 0.0f)
				continue; // TODO
			else if (is_boundery < -1)
				active.emplace_back(Line(e, pipe));
			}
	}

	std::vector<bool> visited(edges.size(), false);
	std::vector<unsigned int> line_indexes = { 0 };
	while (line_indexes.size())
	{
		unsigned int li = line_indexes.back();
		line_indexes.pop_back();
		visited[li] = true;

		std::pair<Line, int>& l{ lines[li] };
		const vec3& vertex{ l.first.p };
		Edge* e = edges[li];

		if (l.second == -1) // vertex QI should be initialized
		{
			int vertex_QI = 0;
			for (auto p : polygons) {
				std::vector<Line> lp = p->transform(pipe);
				double polygon_z = p->get_z_at_point(vertex.x, vertex.y, pipe);
				if (polygon_z <= vertex.z)
					continue;
				// initial vertex QI calculation
				// the calculation is wrong, not checking for z 
				Line lx{ vec3(0, vertex.y, 0), vec3(sw, vertex.y, 0) };
				Line ly{ vec3(vertex.x, 0, 0), vec3(vertex.x, sh, 0) };
				int collision_count = 0;
				for (auto& sl : lp) {
					//if (l.first == sl)
					//	continue;
					if (line_intersection_2d(sl, lx).are_intersecting)
						collision_count++;
					if (line_intersection_2d(sl, ly).are_intersecting)
						collision_count++;
				}
				if (collision_count >= 4)
					vertex_QI++;
			}
			l.second = vertex_QI;
		}

		// should be pushing the unknown ones at the end, use a list or seperate to functions
		for (unsigned int i = 0; i < e->v1->get_edges().size(); i++) {
			if (e->v1->get_edges()[i] == e)
				continue;
			unsigned int near_line_id = (e->v1->get_edges()[i])->id;
			if (visited[near_line_id] == false)
				line_indexes.push_back(near_line_id);
		}

		// is this search dependent on the QI value ? 
		std::vector<std::pair<vec3, bool>> intersections; // second value stands for - did the QI decreased
		for (auto& al : active) {
			Intersection intersection{ line_intersection_2d(l.first, al) };
			if (intersection.are_intersecting) {
				intersections.emplace_back(std::move(std::pair<vec3, bool>(intersection.lp, intersection.lp.z >= intersection.rp.z)));
			}
		}

		std::vector<int> QVs(intersections.size() + 2);
		QVs[0] = l.second;
		if (intersections.size())
		{
			// not performent since we could sort the vector in the insertions
			if (l.first.p.x < l.first.p.x + l.first.d.x) {
				std::sort(intersections.begin(), intersections.end(),
					[](const std::pair<vec3, bool>& l, const std::pair<vec3, bool>& r) {
					return l.first.x < r.first.x; });
			}
			else {
				std::sort(intersections.begin(), intersections.end(),
					[](const std::pair<vec3, bool>& l, const std::pair<vec3, bool>& r) {
					return l.first.x > r.first.x; });
			}

			unsigned int i;
			for (i = 1; i < QVs.size() - 1; i++) {
				if (intersections[i - 1].second)
					QVs[i] = QVs[i - 1] - 1;
				else
					QVs[i] = QVs[i - 1] + 1;
			}
			QVs[i] = QVs[i - 1]; // special cases?
		}
		
		const int next_vertex_QV = QVs.back(); 
		const Vertex* next_vertex = e->v2;

		// can the edges be upside down, then the QV isnt right?
		for (unsigned int i = 0; i < next_vertex->get_edges().size(); i++) {
			if (next_vertex->get_edges()[i] == e)
				continue;
			unsigned int next_line_id = (next_vertex->get_edges()[i])->id;
			if (visited[next_line_id] == false) {
				lines[next_line_id].second = next_vertex_QV;
				line_indexes.push_back(next_line_id);
			}
		}

		if (intersections.size() == 0) {
			Helpers::setLine(pDC, l.first.p.x, l.first.p.y, l.first.p.x + l.first.d.x, l.first.p.y + l.first.d.y, RGB(255, 255, 255));
		}
		else {
			unsigned int i;
			for (i = 0; i < QVs.size() - 2; i++) {

				const vec3& v1 = (i == 0) ? l.first.p : intersections[i-1].first;
				const vec3& v2 = intersections[i].first;
				if (QVs[i] == 0) 
					Helpers::setLine(pDC, v1.x, v1.y, v2.x, v2.y, RGB(255, 255, 255));
			}
			if (QVs[i] == 0)
				Helpers::setLine(pDC, intersections.back().first.x, intersections.back().first.y, l.first.p.x + l.first.d.x, l.first.p.y + l.first.d.y, RGB(255, 255, 255));
		}
	}
}
#endif

/*
void Renderer::DrawWireframes_NonLinear(ZBuffer& zbuffer, const std::vector<Edge*>& edges_to_draw, const std::vector<Edge*>& edges, const std::vector<Poly*> polygons,
unsigned int sw, unsigned int sh, const Pipeline& pipe, const std::vector<Light>& light_sources)
{
	std::vector<std::pair<Line, Edge*>> active;
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		Edge* e = edges[i];
		if (e->p2 == nullptr || e->is_silhouette(pipe))
			active.emplace_back(Line(e, pipe), e);
	}

	std::vector<Line> divided_lines;
	std::vector<Edge*> dle;
	unsigned int edge_n = -1;
	for (auto e : edges_to_draw)
	{
		edge_n++;
		Line l{ Line(e, pipe) };
		std::vector<vec3> intersections {
			l.p,
			l.p + l.d
		};

		unsigned int active_n = -1;
		for (auto& a : active) {
			active_n++;
			if (l == a.first)
				continue;
			Intersection intersection{ line_intersection_2d(l, a.first) };
			// if the edges are neighboors they will always intersect.
			if (intersection.are_intersecting && !intersection.are_only_touching) {
				intersections.emplace_back(intersection.lp);
			}
		}

		{
			std::function<bool(const vec3&, const vec3&)> cmp;
			if (l.p.x < l.p.x + l.d.x)
				cmp = [](const vec3& l, const vec3& r) { return l.x < r.x; };
			else if (l.p.x > l.p.x + l.d.x)
				cmp = [](const vec3& l, const vec3& r) { return l.x > r.x; };
			else if (l.p.y < l.p.y + l.d.y)
				cmp = [](const vec3& l, const vec3& r) { return l.y < r.y; };
			else if (l.p.y > l.p.y + l.d.y)
				cmp = [](const vec3& l, const vec3& r) { return l.y > r.y; };
			else // only the z value is different.
				continue;
			std::sort(intersections.begin(), intersections.end(), cmp);
		}

		divided_lines.reserve(divided_lines.size() + intersections.size());
		for (unsigned int i = 1; i < intersections.size(); i++) {
			divided_lines.emplace_back(intersections[i - 1], intersections[i]);
			dle.push_back(e);
		}
	}

	std::vector<Line> front_lines;
	std::vector<Edge*> fle;
	front_lines.reserve(active.size());
	for (int i = 0; i < divided_lines.size(); i++)
	{
		vec3 v{ divided_lines[i].p + (divided_lines[i].d / 2.0f) };
		int v_qv = calc_point_qv(v, dle[i], polygons, sw, sh, pipe);
		if (v_qv == 0) {
			front_lines.push_back(divided_lines[i]);
			fle.emplace_back(dle[i]);
		}
	}

	for (auto& l : front_lines) {
		l.to_screen(sw, sh);
		ZDrawLine(zbuffer, sw, sh, l.p.x, l.p.y, l.p.z, l.p.x + l.d.x, l.p.y + l.d.y, l.p.z + l.d.z, RGB(255, 255, 255));
	}
}
*/
/* scan-line z-buffer algorithm */
/*
void Renderer::DrawPolygons(unsigned int screen_width, unsigned int screen_height, const std::function<vec3(const vec3&)>& pipe,
					  const std::vector<LightParams>& ls, COLORREF* const screen) {
	std::vector<std::pair<Poly, double>> polygons_ymin, polygons_ymax;
	polygons_ymin.reserve(polygons.size());
	polygons_ymax.reserve(polygons.size());

	auto polygon_pipe = [&](const Poly& p) {
		Poly result{ p.color, p.normal, p.vertexes.size() };
		std::transform(p.vertexes.cbegin(), p.vertexes.cend(), result.vertexes.begin(), pipe);
		return std::move(result);
	};

	std::transform(polygons.cbegin(), polygons.cend(), polygons_ymin.begin(),
		[&](const Poly& p) {return std::move(std::make_pair(polygon_pipe(p), Polygon::miny_vertex(p))); });
	std::transform(polygons.cbegin(), polygons.cend(), polygons_ymax.begin(),
		[&](const Poly& p) {return std::move(std::make_pair(polygon_pipe(p), Polygon::maxy_vertex(p))); });
	// TODO: handle clipped polygons?
	// minimal y first for both
	using piter = std::pair<Poly, double>;
	std::sort(polygons_ymin.begin(), polygons_ymin.end(),
		[](const piter& l, const piter& r) { return l.second < r.second; });
	std::sort(polygons_ymin.begin(), polygons_ymax.end(),
		[](const piter& l, const piter& r) { return l.second < r.second; });

	std::vector<Poly> active;
	for (unsigned int y = 0; y < screen_height; y++) {
		std::fill(screen + y * screen_width, screen + (y+1) * screen_width, -DBL_MAX);
		active.emplace_back(std::upper_bound(polygons_ymin.cbegin(), polygons_ymin.cend(),// { Poly(), y },
			[=](const piter& lp, const piter& rp) { return lp.second < rp.second; }),
			polygons_ymin.cend());

		for (auto& p : active) {
			// get scanline of p -> vector of x pairs?
		}
	}
}
*/

void Renderer::ZDrawLine(ZBuffer& zbuffer, unsigned int sw, unsigned int sh, int x1, int  y1, int z1, int x2, int y2, int z2, COLORREF c) const {
	double m = (double)(y2 - y1) / (x2 - x1);
	bool xy_swapped = false;

	if (m < -1 || m > 1) {
		std::swap(x1, y1);
		std::swap(x2, y2);
		xy_swapped = true;
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
		std::swap(z1, z2);
	}

	m = (double)(y2 - y1) / (x2 - x1);
	double mz = (double)(z2 - z1) / (x2 - x1);
	int dec = y1 < y2 ? 1 : -1;
	int curr_x = x1;
	int curr_y = y1;
	int curr_z = z1;
	double err =   dec  * m  - 0.5;
	while (curr_x < x2) {
		if (!xy_swapped)
			zbuffer.put_pixel(curr_x, curr_y, curr_z, c);
		else
			zbuffer.put_pixel(curr_y, curr_x, curr_z, c);

		curr_x++;
		if (err > 0) {
			err += dec * m - 1;
			curr_y += dec;
		}
		else {
			err += dec * m;
		}
		curr_z += mz;
	}
}

void Renderer::DrawLine(CDC* pDC, unsigned int sw, unsigned int sh, double dx1, double dy1, double dx2, double dy2, COLORREF c) {
	unsigned int ss = min(sw, sh);
	int x1 = ((sw / 2.0f) + (ss * dx1));
	int x2 = ((sw / 2.0f) + (ss * dx2));
	int y1 = ((sh / 2.0f) + (-(int)ss * dy1));
	int y2 = ((sh / 2.0f) + (-(int)ss * dy2));

	double m = (double)(y2 - y1) / (x2 - x1);
	bool xy_swapped = false;

	if (m < -1 || m > 1) {
		std::swap(x1, y1);
		std::swap(x2, y2);
		xy_swapped = true;
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	m = (double)(y2 - y1) / (x2 - x1);
	int dec = y1 < y2 ? 1 : -1;
	int curr_x = x1;
	int curr_y = y1;

	double err = dec * m - 0.5;
	while (curr_x < x2) {
		if (!xy_swapped)
			DrawPixel(pDC, sw, sh, curr_x, curr_y, c);
		else
			DrawPixel(pDC, sw, sh, curr_y, curr_x, c);

		curr_x++;
		if (err > 0) {
			err += dec * m - 1;
			curr_y += dec;
		}
		else {
			err += dec * m;
		}
	}
}

void Renderer::ForceDrawEdge(ZBuffer& zbuffer, Edge* e, const Pipeline& pipe, const CRect& screen, COLORREF c) const
{
	Line l{ e, pipe };
	ZDrawLine(zbuffer, screen.Width(), screen.Height(), l.p.x, l.p.y, INT_MAX, l.p.x + l.d.x, l.p.y + l.d.y, INT_MAX, c);
}

void Renderer::DrawPixel(CDC* pDC, unsigned int sw, unsigned int sh, unsigned int x, unsigned int y, COLORREF c) {
	pDC->SetPixel(x, y, c);
}


void Renderer::DrawPolygons(ZBuffer& zbuffer, const Pipeline& screen_pipe, const Pipeline& obj_pipe,
	const Object& obj, const vec3& cam_pos_obj, const std::vector<Light>& lights, bool bf_culling,
	int specular_exp) const
{
	LightCalcParams params;
	params.obj_color = obj.get_color();
	params.cam_pos = cam_pos_obj;
	params.specular_exp = specular_exp;
	params.obj_pipe = obj_pipe;
	params.screen_pipe = screen_pipe;

	for (const Poly* p : obj.get_polygons())
	{
		if (bf_culling && p->face_indicator(screen_pipe) < 0) {
			continue; // don't draw back faces	
		}

		ScanLineFunc scan_line;

		if (shading == FLAT) {
			scan_line = scan_line_flat;
		} else if (shading == GOURAUD) {
			scan_line = scan_line_gouraud;
		} else if (shading == PHONG) {
			scan_line = scan_line_phong;
		}

		scan_polygon(zbuffer, p, lights, scan_line, params);
	}
}

void Renderer::scan_polygon(ZBuffer& zbuffer, const Poly* poly, const std::vector<Light>& lights,
	const ScanLineFunc& scan_line, const LightCalcParams& params)
{
	double min_y = max(0, params.screen_pipe(poly->min_y_vertex(params.screen_pipe)->vec).y);
	double max_y = min(zbuffer.sh - 1, params.screen_pipe(poly->max_y_vertex(params.screen_pipe)->vec).y);

	for (unsigned int y = min_y; y <= max_y; y++)
	{
		std::vector<PointOnEdge> intersections = get_scan_line_intersections(poly, params.screen_pipe, y, zbuffer.sw);
		if (intersections.size() == 0) continue;

		scan_line(zbuffer, poly, y, intersections, lights, params);
	}
}

std::vector<Renderer::PointOnEdge> Renderer::get_scan_line_intersections(const Poly* poly,
	const Pipeline& pipe, unsigned int y,double sw)
{
	std::vector<Line> poly_lines = poly->transform(pipe);
	const auto& edges = poly->get_edges();

	std::vector<PointOnEdge> intersections;
	intersections.reserve(edges.size());

	for (int i = 0; i < poly_lines.size(); ++i) {
		Intersection curr_i = scan_line_intersection_2d(y, poly_lines[i]);
		if (!curr_i.are_only_touching && curr_i.are_intersecting) {
			intersections.emplace_back(curr_i.rp, edges[i]);
		}
	}

	auto cmp_by_x = [](const PointOnEdge& l, const PointOnEdge& r) { return l.first.x < r.first.x; };
	std::sort(intersections.begin(), intersections.end(), cmp_by_x);

	return intersections;
}

void Renderer::scan_line_flat(ZBuffer& zbuffer, const Poly* poly, unsigned int y,
	const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params)
{
	vec3 normal = poly->get_normal(params.obj_pipe);
	vec3 pos = params.obj_pipe(poly->center);
	COLORREF color = apply_lights(lights, normal, pos, params).get_rgb();

	for (int i = 0; i < intersections.size() - 1; i += 2)
	{
		double x1 = intersections[i].first.x, x2 = intersections[i + 1].first.x;
		double z1 = intersections[i].first.z, z2 = intersections[i + 1].first.z;

		if (x1 == x2) continue;

		// for calculating depth of points between p1 & p2
		double dz = (z2 - z1) / (x2 - x1);

		for (double x = max(x1, 0), z = z1; x <= min(x2, zbuffer.sw - 1); ++x, z += dz)
			zbuffer.put_pixel(x, y, z, color);
	}
}

double get_linear2d_screen_ratio(const vec3& v1, const vec3& v2, double x, double y) {
	double dx = abs(x - v1.x);
	double x_len = abs(v1.x - v2.x);
	double ratio;
	if (dx < 0.1 || x_len < 0.1) {
		ratio = abs((y - v1.y) / (v1.y - v2.y));
	}
	else {
		ratio = dx / x_len;
	}
	return ratio;
};

void Renderer::scan_line_gouraud(ZBuffer& zbuffer, const Poly* poly, unsigned int y,
	const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params)
{
	std::unordered_map<const Vertex*, LightColor> color;
	std::unordered_map<const Vertex*, vec3> pos;

	for (const Vertex* v : poly->get_vertices())
	{
		LightColor vertex_color = apply_lights(lights, v->get_normal(params.obj_pipe), params.obj_pipe(v->vec), params);
		color.emplace(v, vertex_color);
		
		vec3 v_pos = params.screen_pipe(v->vec);
		pos.emplace(v, v_pos);
	}

	for (int i = 0; i < intersections.size() - 1; i += 2)
	{
		double x1 = intersections[i].first.x, x2 = intersections[i + 1].first.x;
		double z1 = intersections[i].first.z, z2 = intersections[i + 1].first.z;

		if (x1 == x2) continue;

		const Edge* left_e = intersections[i].second;
		const Edge* right_e = intersections[i+1].second;

		double lr = get_linear2d_screen_ratio(pos[left_e->v1], pos[left_e->v2], x1, y);
		double rr = get_linear2d_screen_ratio(pos[right_e->v1], pos[right_e->v2], x2, y);

		auto interpolate_color = [](const LightColor& c1, const LightColor& c2, double ratio)->LightColor {
			return ((1 - ratio) * c1) + (ratio * c2);
		};

		LightColor x1_color = interpolate_color(color[left_e->v1], color[left_e->v2], lr);
		LightColor x2_color = interpolate_color(color[right_e->v1], color[right_e->v2], rr);

		// for calculating depth of points between p1 & p2
		double dz = (z2 - z1) / (x2 - x1);

		for (double x = max(x1, 0), z = z1; x <= min(x2, zbuffer.sw - 1); ++x, z += dz) {
			double r = abs((x - x1) / (x1 - x2));
			LightColor x_color = interpolate_color(x1_color, x2_color, r);
			zbuffer.put_pixel(x, y, z, x_color.get_rgb());
		}
	}
}


void Renderer::scan_line_phong(ZBuffer& zbuffer, const Poly* poly, unsigned int y,
	const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params)
{
	std::unordered_map<const Vertex*, vec3> normal;
	std::unordered_map<const Vertex*, vec3> screen_pos;
	std::unordered_map<const Vertex*, vec3> obj_pos;

	for (const Vertex* v : poly->get_vertices())
	{
		normal.emplace(v, v->get_normal(params.obj_pipe));
		screen_pos.emplace(v, params.screen_pipe(v->vec));
		obj_pos.emplace(v, params.obj_pipe(v->vec));
	}

	for (int i = 0; i < intersections.size() - 1; i += 2)
	{
		double x1 = intersections[i].first.x, x2 = intersections[i + 1].first.x;
		double z1 = intersections[i].first.z, z2 = intersections[i + 1].first.z;

		if (x1 == x2) continue;

		const Edge* left_e = intersections[i].second;
		const Edge* right_e = intersections[i + 1].second;

		double lr = get_linear2d_screen_ratio(screen_pos[left_e->v1], screen_pos[left_e->v2], x1, y);
		double rr = get_linear2d_screen_ratio(screen_pos[right_e->v1], screen_pos[right_e->v2], x2, y);

		auto interpolate = [](const vec3& v1, const vec3& v2, double ratio)->vec3 {
			return v1 + ratio * (v2 - v1);
		};

		vec3 x1_normal = interpolate(normal[left_e->v1], normal[left_e->v2], lr).Normalize();
		vec3 x2_normal = interpolate(normal[right_e->v1], normal[right_e->v2], rr).Normalize();

		vec3 x1_pos = interpolate(obj_pos[left_e->v1], obj_pos[left_e->v2], lr);
		vec3 x2_pos = interpolate(obj_pos[right_e->v1], obj_pos[right_e->v2], rr);

		// for calculating depth of points between p1 & p2
		double dz = (z2 - z1) / (x2 - x1);

		for (double x = max(x1, 0), z = z1; x <= min(x2, zbuffer.sw - 1); ++x, z += dz) {
			double r = abs((x - x1) / (x1 - x2));
			vec3 x_normal = interpolate(x1_normal, x2_normal, r).Normalize();
			vec3 x_pos = interpolate(x1_pos, x2_pos, r);
			LightColor x_color = apply_lights(lights, x_normal, x_pos, params);
			zbuffer.put_pixel(x, y, z, x_color.get_rgb());
		}
	}
}


LightColor Renderer::apply_lights(const std::vector<Light>& lights, const vec3& surface_normal, const vec3& surface_pos, const LightCalcParams& params)
{
	// ambient
	LightColor final_c;

	for (const Light& light : lights)
	{
		if (!light.enabled)
			continue;

		LightColor reflected_c = LightColor::get_reflected_color(light.color, params.obj_color);

		if (light.type == LIGHT_TYPE_AMBIENT) {
			final_c += light.ambient_coeff * reflected_c;
			continue;
		}

		vec3 light_pos = light.pos, light_dir = light.dir;

		if (light.space == LIGHT_SPACE_LOCAL) {
			light_pos = params.obj_pipe(light.pos);
			vec3 end = params.obj_pipe(light_pos + light_dir);
			light_dir = end - light_pos;
		}

		double attenuation = 1.0;

		if (light.type == LIGHT_TYPE_DIRECTIONAL) {
			light_dir = UnitVector(-light_dir);
		}
		else if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT) {
			light_dir = light_pos - surface_pos;
			attenuation = 1.0 / (0.000001 + light_dir.Length());
			light_dir.Normalize();
		}

		if (light.type == LIGHT_TYPE_SPOT) {
			double light2surface_angle = degrees(acos((-light_dir) * UnitVector(light.dir)));
			if (light2surface_angle > light.cone_angle) {
				attenuation = 0.0;
			}
		}

		// diffuse
		double cos_theta = max(0, light_dir * surface_normal); // for diffuse light
		LightColor diffuse = reflected_c * light.diffuse_coeff * cos_theta;

		// specular
		vec3 reflection_d = Reflect(light_dir, surface_normal);
		vec3 cam_dir = (params.cam_pos - surface_pos).Normalize();
		double cos_alpha = max(0, cam_dir * reflection_d); // for specular reflection
		LightColor specular = light.color * light.specular_coeff * pow(cos_alpha, params.specular_exp);

		final_c += attenuation * (diffuse + specular);
	}

	return final_c;
}