#pragma once

#include "StdAfx.h"
#include <algorithm>
#include <memory>
#include <vector>
#include <functional>
#include <utility>
#include "Light.h"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.h"
#include "ZBuffer.h"

class Edge;
class Poly;
class Object;
struct Line;

/*
	utility Rendering class which would encapsolate the rendering methods from the geomtery and models information.
*/

struct Renderer
{
	enum Shading { FLAT, GOURAUD, PHONG } shading;
	enum Mode { WIREFRAME_MODE, SCREEN_MODE, FILE_MODE } mode;
	enum NormalMode { PARS_NORMALS, CALC_NORMALS } normal_mode;

	struct LightCalcParams {
		LightColor obj_color;
		vec3 cam_pos;
		Pipeline obj_pipe;
		Pipeline screen_pipe;
		int specular_exp;
	};

	void DrawWireframes_NonLinear(ZBuffer&, const std::vector<Edge*>& edges_to_draw, const std::vector<Edge*>& edges, const std::vector<Poly*> polygons,
	unsigned int sw, unsigned int sh, const Pipeline& pipe, const std::vector<Light>& light_sources);
	void DrawLine(CDC* pDC, unsigned int sw, unsigned int sh, double dx1, double dy1, double dx2, double dy2, COLORREF c);
	void ZDrawLine(ZBuffer& zbuffer, unsigned int sw, unsigned int sh, int x1, int  y1, int z1, int x2, int y2, int z2, COLORREF c) const;
	void DrawPixel(CDC* pDC, unsigned int sw, unsigned int sh, unsigned int x, unsigned int y, COLORREF c);

	void ForceDrawEdge(ZBuffer& zbuffer, Edge* e, const Pipeline& pipe, const CRect& screen, COLORREF c) const;

	void DrawPolygons(ZBuffer& zbuffer, const Pipeline& screen_pipe, const Pipeline& obj_pipe,
		const Object& obj, const vec3& cam_pos_obj, const std::vector<Light>& lights, bool bf_culling,
		int specular_exp) const;

private:

	using PointOnEdge = std::pair<vec3, const Edge*>;
	using ScanLineFunc = std::function<void(ZBuffer& zbuffer, const Poly* poly, unsigned int y, const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params)>;

	static void scan_polygon(ZBuffer& zbuffer, const Poly* poly, const std::vector<Light>& lights, const ScanLineFunc& scan_line, const LightCalcParams& params);
	static std::vector<PointOnEdge> get_scan_line_intersections(const Poly* poly, const Pipeline& pipe, unsigned int y, double sw);
	static void scan_line_flat(ZBuffer& zbuffer, const Poly* poly, unsigned int y, const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params);
	static void scan_line_gouraud(ZBuffer& zbuffer, const Poly* poly, unsigned int y, const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params);
	static void Renderer::scan_line_phong(ZBuffer& zbuffer, const Poly* poly, unsigned int y, const std::vector<PointOnEdge>& intersections, const std::vector<Light>& lights, const LightCalcParams& params);
	static LightColor apply_lights(const std::vector<Light>& lights, const vec3& surface_normal, const vec3& surface_pos, const LightCalcParams& params);
};


