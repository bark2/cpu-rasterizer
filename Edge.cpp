#include "Edge.h"
#include "Vertex.h"
#include "cg.hpp"

void Edge::draw_edge(CDC *pDC, const mat4&  transform_mat, const mat4& aspect_ratio_mat, COLORREF c) {
	vec4 t_vec1 = vec4(aspect_ratio_mat * (transform_mat * v1->vec).HomogenicDivision());
	vec4 t_vec2 = vec4(aspect_ratio_mat * (transform_mat * v2->vec).HomogenicDivision());

	if (!(t_vec1 == vec4(0, 0, 0, 0) || t_vec2 == vec4(0, 0, 0, 0)))
		Helpers::setLine(pDC, (int)t_vec1.x, (int)t_vec1.y, (int)t_vec2.x, (int)t_vec2.y, c);
}

void Edge::draw_edge(CDC *pDC, const Pipeline& pipe, COLORREF c) {
	vec4 t_vec1 = pipe(v1->vec);
	vec4 t_vec2 = pipe(v2->vec);
	if (!(t_vec1 == vec4(0, 0, 0, 0) || t_vec2 == vec4(0, 0, 0, 0)))
		Helpers::setLine(pDC, (int)t_vec1.x, (int)t_vec1.y, (int)t_vec2.x, (int)t_vec2.y, c);
}

bool Edge::is_silhouette(const Pipeline& pipe) const
{
	if (p1 && p2) {
		double indi1 = p1->face_indicator(pipe);
		double indi2 = p2->face_indicator(pipe);

		return
			indi1 * indi2 < 0 ||
			indi1 > 0 && indi2 == 0 ||
			indi2 > 0 && indi1 == 0;
	}
	return true;
}

void Edge::set_polygon(Poly* p) {
	if (p1 == nullptr) {
		p1 = p;
	}
	else if (p2 == nullptr) {
		p2 = p;
	}
}