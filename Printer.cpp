#include "Printer.h"

void Printer::setProjection(ProjectionMode mode) {
	const vec3& box_min = cg_bounding_box_min;
	const vec3& box_max = cg_bounding_box_max;
	switch (mode) {
	case ORTHOGRAPHIC: {
		projection = mat4::Ortho(box_min.x, box_max.x,
								 box_min.y, box_max.y,
								 box_max.z, box_min.z);
	} break;
		/*	case PERSPECTIVE: {
				projection = ;
			} break; */
	}
}

void Printer::drawObjVertices(QuickBMP& q_bmp, const CG_Object& obj) {
	mat4 screen_proj = projection * view * model * obj.model;
	for (const auto& v : obj.vertices)
	{
		vec4 projected_pos = screen_proj * vec4(v, 1.0f);
		if (isInBox(projected_pos)) {
			Pixel p = screenCoord(q_bmp.rect, projected_pos);
			q_bmp.setPixel(p.x, p.y, obj.color);
		}
	}
	q_bmp.print();
}

void Printer::drawObjWireframe(QuickBMP& q_bmp, const CG_Object& obj) {

	for (const auto& v : obj.vertices)
	{
		vec4 projected_pos = projection * view * model * obj.model * vec4(v, 1.0f);
		if (projected_pos.x <= 1.0f && projected_pos.x >= -1.0f &&
			projected_pos.y <= 1.0f && projected_pos.y >= -1.0f &&
			projected_pos.z <= 1.0f && projected_pos.z >= -1.0f)
		{
			int screen_pos_x = lround(q_bmp.rect.Width()*(projected_pos.x + 1.0f) / 2.0f);
			int screen_pos_y = lround(q_bmp.rect.Height()*(projected_pos.y + 1.0f) / 2.0f);
			q_bmp.setPixel(screen_pos_x, q_bmp.rect.Height() - screen_pos_y, obj.color);
		}
	}
	q_bmp.print();
}