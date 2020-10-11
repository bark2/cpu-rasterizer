// CGWorkView.cpp : implementation of the CCGWorkView class
//

#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"


// For Status Bar access
#include "MainFrm.h"


// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_FILE_RESET, OnResetView)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_VIEW_BBOX, OnViewBBox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BBOX, OnUpdateViewBBox)
	ON_COMMAND(ID_VIEW_SILHOUETTE, OnViewSilhouette)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SILHOUETTE, OnUpdateViewSilhouette)
	ON_COMMAND(ID_VIEW_LIGHT_SOURCES, OnViewLightSources)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHT_SOURCES, OnUpdateViewLightSources)
	ON_COMMAND(ID_VIEW_V_NORMALS, OnViewVertexNormals)
	ON_UPDATE_COMMAND_UI(ID_VIEW_V_NORMALS, OnUpdateViewVertexNormals)
	ON_COMMAND(ID_VIEW_P_NORMALS, OnViewPolygonNormals)
	ON_COMMAND(ID_VIEW_CHANGE_COLORS, OnViewChangeColors)
	ON_COMMAND(ID_VIEW_CHANGE_FINENESS, OnViewChangeFineness)
	ON_UPDATE_COMMAND_UI(ID_VIEW_P_NORMALS, OnUpdateViewPolygonNormals)
	ON_COMMAND(ID_VIEW_FLIP_NORMALS, OnViewFlipNormals)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FLIP_NORMALS, OnUpdateViewFlipNormals)
	ON_COMMAND(ID_VIEW_BF_CULLING, OnViewBFCulling)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BF_CULLING, OnUpdateViewBFCulling)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_ACTION_VIEW, OnActionView)
	ON_UPDATE_COMMAND_UI(ID_ACTION_VIEW, OnUpdateActionView)
	ON_COMMAND(ID_ACTION_OBJECT, OnActionObject)
	ON_UPDATE_COMMAND_UI(ID_ACTION_OBJECT, OnUpdateActionObject)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_AXIS_XY, OnAxisXY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_XY, OnUpdateAxisXY)
	ON_COMMAND(ID_OPTIONS_MOUSESENSITIVITY, OnSensitivityDialog)
	ON_COMMAND(ID_OPTIONS_PERSPECTIVECONTROL, OnPerspectiveDialog)
	ON_COMMAND(ID_NORMALS_CALC_NORMALS, OnCalcNormals)
	ON_UPDATE_COMMAND_UI(ID_NORMALS_CALC_NORMALS, OnUpdateCalcNormals)
	ON_COMMAND(ID_RENDERING_WIREFRAME, OnRenderingWireframe)
	ON_UPDATE_COMMAND_UI(ID_RENDERING_WIREFRAME, OnUpdateRenderingWireframe)
	ON_COMMAND(ID_RENDERING_TO_SCREEN, OnRendering2Screen)
	ON_UPDATE_COMMAND_UI(ID_RENDERING_TO_SCREEN, OnUpdateRendering2Screen)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_SHADING_PHONG, OnLightShadingPhong)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_PHONG, OnUpdateLightShadingPhong)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluCylinder(quad, radius, 0.0, height, 20, 20);
        gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

void CCGWorkView::set_defaults() {
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	should_draw_vertex_normals = false;
	should_draw_polygon_normals = false;
	should_draw_bbox = false;
	should_draw_silhouette = false;
	should_draw_light_sources = false;
	flip_normals = false;
	normal_mode = Renderer::PARS_NORMALS;
	bf_culling = false;
	is_LButtonDown = false;

	render_mode = Renderer::WIREFRAME_MODE;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	init_lights();
	m_pDbBitMap = NULL;
	m_pDbDC = NULL;
}

void CCGWorkView::init_lights() {
	for (int i = LIGHT_ID_1; i < MAX_LIGHT; i++) {
		Light& light = m_lights[i];
		light.enabled = false;
		light.color = vec3{ 1.0, 1.0, 1.0 }; // white
		light.pos = { 0, 0, 0 };
		light.dir = { 0, 0, 0 };
		light.type = LIGHT_TYPE_DIRECTIONAL;
		light.space = LIGHT_SPACE_VIEW;
		light.diffuse_coeff = 0.8;
		light.ambient_coeff = 0.0;
		light.specular_coeff = 1;
		light.cone_angle = 10.0;
	}

	Light& light = m_ambientLight;
	light.enabled = true;
	light.color = vec3{ 1.0, 1.0, 1.0 }; // white
	light.pos = { 0, 0, 0 };
	light.dir = { 0, 0, 0 };
	light.type = LIGHT_TYPE_AMBIENT;
	light.space = LIGHT_SPACE_VIEW;
	light.diffuse_coeff = 0.0;
	light.ambient_coeff = 0.2;
	light.specular_coeff = 0.0;
	light.cone_angle = 0.0;

	m_lights[LIGHT_ID_1].enabled = true;
	m_lights[LIGHT_ID_1].pos = { 0, 0, 0.5 };
	m_lights[LIGHT_ID_1].dir = { 0, 0, -0.5 };
	
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
	{
		light_dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	light_dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);

	light_dlg.specular_exp = 32;

	lights = get_lights();
}

CCGWorkView::CCGWorkView()
{
	set_defaults();
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();

	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);
	
	if ( NULL == m_pDC ) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	CRect r;
	GetClientRect(&r);
	m_pDbDC = new CDC();
	m_pDbDC->CreateCompatibleDC(m_pDC);
	SetTimer(1, 1, NULL);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);	
	m_pDbDC->SelectObject(m_pDbBitMap);


	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy ) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;
	/*
	CRect r;
	GetClientRect(&r);
	DeleteObject(m_pDbBitMap);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);	
	m_pDbDC->SelectObject(m_pDbBitMap);
	*/
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}



BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC) 
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	
	return true;
}


bool CCGWorkView::set_bg_image(const char* name, int sw, int sh)
{
	bg_image = new PngWrapper( name , sw, sh );
	return bg_image->ReadPng();
}

CCGWorkView::~CCGWorkView()
{
	delete bg_image;
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////
#include "cg.hpp"
#include "mat4.h"
#include "vec3.hpp"
#include "ZBuffer.h"
#include "Scene.h"

void CCGWorkView::OnDraw(CDC* pDC)
{
	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC memDC;
	CBitmap bmp;
	CRect rect;
	GetClientRect(&rect);

	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(&rect, color_dlg.get_bg_c());

	ZBuffer zbuffer(rect.Width(), rect.Height(), color_dlg.get_bg_c());
	/*
	if (render_mode == Renderer::SCREEN_MODE &&  set_bg_image)
	{
		if (bg_image == nullptr)
			set_bg_image("bg.png", rect.Width(), rect.Height());
		bool png_read_result = zbuffer.set_bg_image(*bg_image);
	}
	*/

	switch (render_mode) {
	case Renderer::WIREFRAME_MODE:
		cg_scene.draw_wireframe(&memDC, rect); break;
	case Renderer::SCREEN_MODE:
		cg_scene.draw_polygons(zbuffer, rect, lights, light_dlg.specular_exp); break;
	}

//   cg_scene.draw_wireframe_(&memDC, rect);

	if (should_draw_vertex_normals)
		cg_scene.draw_vertex_normals(&memDC, zbuffer, rect, color_dlg.get_normals_c(), render_mode);

	if (should_draw_polygon_normals)
		cg_scene.draw_polygon_normals(&memDC, zbuffer, rect, color_dlg.get_normals_c(), render_mode);

	if (should_draw_bbox)
		cg_scene.draw_bbox(&memDC, zbuffer, rect, RGB(255, 0, 0), render_mode);

	if (should_draw_light_sources)
		cg_scene.draw_light_sources(&memDC, zbuffer, rect, lights, render_mode);

	if (should_draw_silhouette)
		cg_scene.draw_silhouette(zbuffer, rect);

	//	cg_scene.draw_selected_polygons(&memDC, rect, RGB(0, 255, 255));

	zbuffer.draw2screen(&memDC);
	//zbuffer.export_png("image.png", rect.Width(), rect.Height());

	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, rect.left, rect.top, SRCCOPY);
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy() 
{
	CView::OnDestroy();

	// delete the DC
	if ( m_pDC ) {
		delete m_pDC;
	}

	if (m_pDbDC) {
		delete m_pDbDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}


void CCGWorkView::OnFileLoad() 
{
	TCHAR szFilters[] = _T ("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,szFilters);

	if (dlg.DoModal () == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		set_defaults();
		cg_scene = std::move(Scene());
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);

		cg_scene.set_bbox();
		cg_scene.validate_normals_info();
		cg_scene.norm_model();
		cg_scene.set_ortho_proj();

		// Open the file and read it.
		// Your code here...

		Invalidate();	// force a WM_PAINT for drawing.
	}
}


void CCGWorkView::OnResetView()
{
	set_defaults();
	cg_scene = std::move(Scene());
	cg_scene.fineness = fineness_dlg.get_fineness();
	CGSkelProcessIritDataFiles(m_strItdFileName, 1);
	cg_scene.set_bbox();
	cg_scene.validate_normals_info();
	cg_scene.norm_model();
	cg_scene.set_ortho_proj();

	Invalidate();	// force a WM_PAINT for drawing.
}


// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic() 
{
	if (!m_bIsPerspective) return;
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	cg_scene.set_ortho_proj();
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective() 
{
	if (m_bIsPerspective) return;
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	cg_scene.set_persp_proj(persp_dlg.get_n(), persp_dlg.get_f());
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}

void CCGWorkView::OnViewBBox()
{
	should_draw_bbox = !should_draw_bbox;
	Invalidate();
}

void CCGWorkView::OnUpdateViewBBox(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(should_draw_bbox);
}

void CCGWorkView::OnViewSilhouette()
{
	should_draw_silhouette = !should_draw_silhouette;
	Invalidate();
}

void CCGWorkView::OnUpdateViewSilhouette(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(should_draw_silhouette);
}

void CCGWorkView::OnViewLightSources()
{
	should_draw_light_sources = !should_draw_light_sources;
	Invalidate();
}

void CCGWorkView::OnUpdateViewLightSources(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(should_draw_light_sources);
}

void CCGWorkView::OnViewVertexNormals()
{
	should_draw_vertex_normals = !should_draw_vertex_normals;
	Invalidate();
}

void CCGWorkView::OnUpdateViewVertexNormals(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(should_draw_vertex_normals);
}


void CCGWorkView::OnViewPolygonNormals()
{
	should_draw_polygon_normals = !should_draw_polygon_normals;
	Invalidate();
}

void CCGWorkView::OnUpdateViewPolygonNormals(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(should_draw_polygon_normals);
}

void CCGWorkView::OnViewFlipNormals()
{
	flip_normals = !flip_normals;
	cg_scene.flip_normals(flip_normals);
	Invalidate();
}

void CCGWorkView::OnUpdateViewFlipNormals(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(flip_normals);
}

void CCGWorkView::OnViewBFCulling()
{
	bf_culling = !bf_culling;
	cg_scene.bf_culling = bf_culling;
	Invalidate();
}

void CCGWorkView::OnUpdateViewBFCulling(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(bf_culling);
}

void CCGWorkView::OnCalcNormals()
{
	switch (normal_mode) {
	case Renderer::PARS_NORMALS:
		normal_mode = Renderer::CALC_NORMALS; break;
	case Renderer::CALC_NORMALS:
		normal_mode = Renderer::PARS_NORMALS; break;
	}

	cg_scene.change_normal_mode(normal_mode);
	Invalidate();
}

void CCGWorkView::OnUpdateCalcNormals(CCmdUI* pCmdUI)
{
	switch (normal_mode) {
	case Renderer::PARS_NORMALS:
		pCmdUI->SetCheck(false); break;
	case Renderer::CALC_NORMALS:
		pCmdUI->SetCheck(true); break;
	}
}

void CCGWorkView::OnViewChangeColors()
{
	if (color_dlg.DoModal() == IDOK && color_dlg.should_invalidate) {
		cg_scene.set_edge_color(color_dlg.get_wireframe_c());
		Invalidate();
		color_dlg.should_invalidate = false;
	}
}

// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate() 
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate() 
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale() 
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}

void CCGWorkView::OnActionView()
{
	cg_scene.curr_space = Scene::Space::VIEW;
	cg_scene.selected_obj = nullptr;
	Invalidate();
}

void CCGWorkView::OnUpdateActionView(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cg_scene.get_curr_space() == Scene::Space::VIEW);
}

void CCGWorkView::OnActionObject()
{
	cg_scene.curr_space = Scene::Space::OBJECT;
	Invalidate();
}

void CCGWorkView::OnUpdateActionObject(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cg_scene.get_curr_space() == Scene::Space::OBJECT);
}



// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX() 
{
	m_nAxis = ID_AXIS_X;
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY() 
{
	m_nAxis = ID_AXIS_Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ() 
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
}

void CCGWorkView::OnAxisXY()
{
	m_nAxis = ID_AXIS_XY;
}

void CCGWorkView::OnUpdateAxisXY(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_XY);
}

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*
	switch (MOUSE_ACTION) {
	case MOUSE_TRANSFORM_ACTION: */
		mouse_start_drag_p = point;
		prev_mouse_drag_pos = point;
		is_LButtonDown = true;
		drag_time = clock();
	//	break;
	//case MOUSE_SELECTION_ACTION:
		CRect screen;
		GetClientRect(&screen);
		if (cg_scene.curr_space == Scene::Space::OBJECT) {
			std::pair<Object*, Poly*> selected = cg_scene.get_nearest_bounding_obj(screen, point.x, point.y);
			cg_scene.selected_obj = selected.first;
			if (selected.first) {
				cg_scene.selected.polygons.push_back(selected.second);
				cg_scene.selected.objects_of_selected_polygons.push_back(selected.first);
			}
		}

		Invalidate();
	//	break;
//	}
}

void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point) {
	is_LButtonDown = false;
}

inline double SLength(const CPoint& p1, const CPoint& p2)
{
	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

#include <ctime>

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (is_LButtonDown) {
		float timediff_sec = ((float)(clock() - drag_time)) / CLOCKS_PER_SEC;
		float refresh_rate_sec = 0;
		switch (render_mode) {
		case Renderer::WIREFRAME_MODE:
			refresh_rate_sec = 0.03; break;
		case Renderer::SCREEN_MODE:
			refresh_rate_sec = 0.1; break;
		}

		if (timediff_sec >= refresh_rate_sec) {
			drag_time = clock();
			OnMouseDragTransofrmation(point.x - prev_mouse_drag_pos.x, point.y - prev_mouse_drag_pos.y);
			prev_mouse_drag_pos = point;
		}
	}
}

void CCGWorkView::OnMouseDragTransofrmation(int delta_x, int delta_y)
{
	CRect screen;
	GetClientRect(&screen);

	double mouse_move_x = (double)delta_x / screen.Width();
	double mouse_move_y = -(double)delta_y / screen.Height();

	mat4 transformation_mat;
	mat4 transformation_mat_inv;

	double dt = mouse_move_x * sens_dlg.get_translation_sensitivity();
	double ds = mouse_move_x * sens_dlg.get_scaling_sensitivity();
	double dr = mouse_move_x * sens_dlg.get_rotation_sensitivity();

	double dty = mouse_move_y * sens_dlg.get_translation_sensitivity();
	double dsy = mouse_move_y * sens_dlg.get_scaling_sensitivity();
	double dry = mouse_move_y * sens_dlg.get_rotation_sensitivity();

	switch (m_nAction) {
	case ID_ACTION_TRANSLATE:
		switch (m_nAxis) {
		case ID_AXIS_X:
			transformation_mat = mat4::Translate(dt, 0, 0);
			transformation_mat_inv = mat4::Translate(-dt, 0, 0);
			break;
		case ID_AXIS_Y:
			transformation_mat = mat4::Translate(0, dty, 0);
			transformation_mat_inv = mat4::Translate(0, -dty, 0);
			break;
		case ID_AXIS_Z:
			transformation_mat = mat4::Translate(0, 0, dt);
			transformation_mat_inv = mat4::Translate(0, 0, -dt);
			break;
		case ID_AXIS_XY:
			transformation_mat = mat4::Translate(dt, dty, 0);
			transformation_mat_inv = mat4::Translate(-dt, -dty, 0);
			break;
		}
		break;
	case ID_ACTION_SCALE:
		switch (m_nAxis) {
		case ID_AXIS_X:
			transformation_mat = mat4::Scale(1 + ds, 1, 1);
			transformation_mat_inv = mat4::Scale(1/(1 + ds), 1, 1);
			break;
		case ID_AXIS_Y:
			transformation_mat = mat4::Scale(1, 1 + ds, 1);
			transformation_mat_inv = mat4::Scale(1, 1 / (1 + dsy), 1);
			break;
		case ID_AXIS_Z:
			transformation_mat = mat4::Scale(1, 1, 1 + ds);
			transformation_mat_inv = mat4::Scale(1, 1, 1 / (1 + ds));
			break;
		case ID_AXIS_XY:
			double curr_ds = max(ds, dsy);
			transformation_mat = mat4::Scale(1 + curr_ds, 1 + curr_ds, 1 + curr_ds);
			transformation_mat_inv = mat4::Scale(1/(1 + curr_ds), 1/(1 + curr_ds), 1/(1 + curr_ds));
			break;
		}
		break;
	case ID_ACTION_ROTATE:
		switch (m_nAxis) {
		case ID_AXIS_X:
			transformation_mat = mat4::Rotate(mat4::Axis::X, 180 * -dry);
			break;
		case ID_AXIS_Y:
			transformation_mat = mat4::Rotate(mat4::Axis::Y, 180 * dr);
			break;
		case ID_AXIS_Z:
			transformation_mat = mat4::Rotate(mat4::Axis::Z, 180 * dr);
			break;
		case ID_AXIS_XY:
			transformation_mat = mat4::Rotate(mat4::Axis::X, 180 * -dry) * mat4::Rotate(mat4::Axis::Y, 180 * dr);
			break;
		}
		transformation_mat_inv = Transpose(transformation_mat);
		break;
	}
	
	switch (cg_scene.get_curr_space()) {
	case Scene::Space::OBJECT:
		cg_scene.transform_model_mat(transformation_mat);
		break;
	case Scene::Space::VIEW:
		cg_scene.transform_scene_mat(transformation_mat, transformation_mat_inv);
		break;
	}

	Invalidate();
}


// RENDERING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnRenderingWireframe()
{
	render_mode = Renderer::WIREFRAME_MODE;
	Invalidate();
}

void CCGWorkView::OnUpdateRenderingWireframe(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(render_mode == Renderer::WIREFRAME_MODE);
}

void CCGWorkView::OnRendering2Screen()
{
	render_mode = Renderer::SCREEN_MODE;
	Invalidate();
}

void CCGWorkView::OnUpdateRendering2Screen(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(render_mode == Renderer::SCREEN_MODE);
}



// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat() 
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
	cg_scene.set_shading(Renderer::FLAT);
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud() 
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
	cg_scene.set_shading(Renderer::GOURAUD);
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

void CCGWorkView::OnLightShadingPhong()
{
	m_nLightShading = ID_LIGHT_SHADING_PHONG;
	cg_scene.set_shading(Renderer::PHONG);
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingPhong(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_PHONG);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants() 
{
	for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	{	    
		light_dlg.SetDialogData((LightID)id,m_lights[id]);
	}
	light_dlg.SetDialogData(LIGHT_ID_AMBIENT,m_ambientLight);

	if (light_dlg.DoModal() == IDOK)
	{
	    for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	    {
			m_lights[id] = light_dlg.GetDialogData((LightID)id);
	    }
	    m_ambientLight = light_dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}

	lights = get_lights();

	Invalidate();
}

std::vector<Light> CCGWorkView::get_lights() const
{
	std::vector<Light> lights;
	lights.reserve(MAX_LIGHT);

	lights.emplace_back(m_ambientLight);

	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
	{
		lights.emplace_back(m_lights[id]);
	}

	return lights;
}

void CCGWorkView::OnSensitivityDialog()
{
	sens_dlg.DoModal();
}

void CCGWorkView::OnPerspectiveDialog()
{
	if (persp_dlg.DoModal() == IDOK && m_bIsPerspective) {
		cg_scene.set_persp_proj(persp_dlg.get_n(), persp_dlg.get_f());
		Invalidate();
	}
}

void CCGWorkView::OnViewChangeFineness()
{
	if (fineness_dlg.DoModal() == IDOK) {
		OnResetView();
	}
}