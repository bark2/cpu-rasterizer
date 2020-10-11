#include "cg.hpp"
#include "Vertex.h"
#include <algorithm>
#include "vec3.hpp"
#include "vec4.hpp"
#include "Scene.h"

using std::swap;

Scene cg_scene;

int Helpers::sign(int num) {
	return (num > 0) - (num < 0);
}

void Helpers::setLine(CDC* pDC, int x1, int y1, int x2, int y2, COLORREF c) {
	double m;
	if (x2 == x1) {
		m = INT_MAX;
	}
	else {
		m = (double)(y2 - y1) / (x2 - x1);
	}

	bool xy_swapped = false;

	if (m < -1 || m > 1) {
		swap(x1, y1);
		swap(x2, y2);
		xy_swapped = true;
	}
	if (x1 > x2) {
		swap(x1, x2);
		swap(y1, y2);
	}

	if (x1 == x2) {
		return;
	}

	m = (double)(y2 - y1) / (x2 - x1);

	int dec = y1 < y2 ? 1 : -1;
	int curr_x = x1;
	int curr_y = y1;

	double err = dec * m - 0.5;
	while (curr_x < x2) {
		curr_x++;
		if (err > 0) {
			err += dec * m - 1;
			curr_y += dec;
		}
		else {
			err += dec * m;
		}
		if (!xy_swapped)
			pDC->SetPixel(curr_x, curr_y, c);
		else
			pDC->SetPixel(curr_y, curr_x, c);
	}
}

void Helpers::setLine_z(ZBuffer& zbuffer, vec3 v1, vec3 v2, COLORREF c) {
	int x1 = (int)v1.x, y1 = (int)v1.y;
	int x2 = (int)v2.x, y2 = (int)v2.y;
	double z1 = v1.z, z2 = v2.z;

	double m;

	if (x2 == x1) {
		m = INT_MAX;
	} else {
		m = (double)(y2 - y1) / (x2 - x1);
	}
	bool xy_swapped = false;

	if (m < -1 || m > 1) {
		swap(x1, y1);
		swap(x2, y2);
		xy_swapped = true;
	}
	if (x1 > x2) {
		swap(x1, x2);
		swap(y1, y2);
		swap(z1, z2);
	}

	if (x1 == x2) {
		return;
	}

	m = (double)(y2 - y1) / (x2 - x1);

	int dec = y1 < y2 ? 1 : -1;
	int curr_x = x1;
	int curr_y = y1;
	double curr_z = z1;

	double err = dec * m - 0.5;
	double dz = (z2 - z1) / (x2 - x1);

	while (curr_x < x2) {
		curr_x++;
		if (err > 0) {
			err += dec * m - 1;
			curr_y += dec;
		}
		else {
			err += dec * m;
		}

		if (!xy_swapped) {
			zbuffer.put_pixel(curr_x, curr_y, curr_z, c);
		} else {
			zbuffer.put_pixel(curr_y, curr_x, curr_z, c);
		}
		curr_z += dz;
	}
}


vec3 Helpers::median(const std::vector<vec3>& points)
{
	size_t size = points.size();

	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> z;

	for (const auto p : points) {
		x.push_back(p.x);
		y.push_back(p.y);
		z.push_back(p.z);
	}

	if (size == 0)
		return { 0, 0, 0 };
	else {
		sort(x.begin(), x.end());
		sort(y.begin(), y.end());
		sort(z.begin(), z.end());
		if (size % 2 == 0) {
			return {	(x[size / 2 - 1] + x[size / 2]) / 2,
						(y[size / 2 - 1] + y[size / 2]) / 2,
						(z[size / 2 - 1] + z[size / 2]) / 2		};
		} else {
			return { x[size / 2], y[size / 2], z[size / 2] };
		}
	}
}

void Helpers::draw_box(CDC* pDC, const Pipeline& pipe, COLORREF c, vec3 bbox_min, vec3 bbox_max)
{
	// l = left, r = right, b = bottom, t = top, n = near, f = far
	vec3 lbn = pipe(bbox_min);
	vec3 rbn = pipe({ bbox_max.x, bbox_min.y, bbox_min.z });
	vec3 ltn = pipe({ bbox_min.x, bbox_max.y, bbox_min.z });
	vec3 rtn = pipe({ bbox_max.x, bbox_max.y, bbox_min.z });

	vec3 lbf = pipe({ bbox_min.x, bbox_min.y, bbox_max.z });
	vec3 rbf = pipe({ bbox_max.x, bbox_min.y, bbox_max.z });
	vec3 ltf = pipe({ bbox_min.x, bbox_max.y, bbox_max.z });
	vec3 rtf = pipe(bbox_max);

	Helpers::setLine(pDC, (int)lbn.x, (int)lbn.y, (int)rbn.x, (int)rbn.y, c);
	Helpers::setLine(pDC, (int)ltn.x, (int)ltn.y, (int)rtn.x, (int)rtn.y, c);
	Helpers::setLine(pDC, (int)lbn.x, (int)lbn.y, (int)ltn.x, (int)ltn.y, c);
	Helpers::setLine(pDC, (int)rbn.x, (int)rbn.y, (int)rtn.x, (int)rtn.y, c);

	Helpers::setLine(pDC, (int)lbf.x, (int)lbf.y, (int)rbf.x, (int)rbf.y, c);
	Helpers::setLine(pDC, (int)ltf.x, (int)ltf.y, (int)rtf.x, (int)rtf.y, c);
	Helpers::setLine(pDC, (int)lbf.x, (int)lbf.y, (int)ltf.x, (int)ltf.y, c);
	Helpers::setLine(pDC, (int)rbf.x, (int)rbf.y, (int)rtf.x, (int)rtf.y, c);

	Helpers::setLine(pDC, (int)lbn.x, (int)lbn.y, (int)lbf.x, (int)lbf.y, c);
	Helpers::setLine(pDC, (int)ltn.x, (int)ltn.y, (int)ltf.x, (int)ltf.y, c);
	Helpers::setLine(pDC, (int)rbn.x, (int)rbn.y, (int)rbf.x, (int)rbf.y, c);
	Helpers::setLine(pDC, (int)rtn.x, (int)rtn.y, (int)rtf.x, (int)rtf.y, c);
}

void Helpers::draw_box_z(ZBuffer& zbuffer, const Pipeline& pipe, COLORREF c, vec3 bbox_min, vec3 bbox_max)
{
	// l = left, r = right, b = bottom, t = top, n = near, f = far
	vec3 lbn = pipe(bbox_min);
	vec3 rbn = pipe({ bbox_max.x, bbox_min.y, bbox_min.z });
	vec3 ltn = pipe({ bbox_min.x, bbox_max.y, bbox_min.z });
	vec3 rtn = pipe({ bbox_max.x, bbox_max.y, bbox_min.z });

	vec3 lbf = pipe({ bbox_min.x, bbox_min.y, bbox_max.z });
	vec3 rbf = pipe({ bbox_max.x, bbox_min.y, bbox_max.z });
	vec3 ltf = pipe({ bbox_min.x, bbox_max.y, bbox_max.z });
	vec3 rtf = pipe(bbox_max);

	Helpers::setLine_z(zbuffer, lbn, rbn, c);
	Helpers::setLine_z(zbuffer, ltn, rtn, c);
	Helpers::setLine_z(zbuffer, lbn, ltn, c);
	Helpers::setLine_z(zbuffer, rbn, rtn, c);

	Helpers::setLine_z(zbuffer, lbf, rbf, c);
	Helpers::setLine_z(zbuffer, ltf, rtf, c);
	Helpers::setLine_z(zbuffer, lbf, ltf, c);
	Helpers::setLine_z(zbuffer, rbf, rtf, c);

	Helpers::setLine_z(zbuffer, lbn, lbf, c);
	Helpers::setLine_z(zbuffer, ltn, ltf, c);
	Helpers::setLine_z(zbuffer, rbn, rbf, c);
	Helpers::setLine_z(zbuffer, rtn, rtf, c);
}