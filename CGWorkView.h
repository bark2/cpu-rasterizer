// CGWorkView.h : interface of the CCGWorkView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
#define AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "gl\gl.h"    // Include the standard CGWork  headers
#include "gl\glu.h"   // Add the utility library


#include "Light.h"
#include "vec3.hpp"
#include "SensitivityDlg.h"
#include "PerspectiveDlg.h"
#include "ColorDlg.h"
#include "FinenessDlg.h"
#include "Renderer.h"
#include "LightDialog.h"

class CCGWorkView : public CView
{
protected: // create from serialization only
	CCGWorkView();
	DECLARE_DYNCREATE(CCGWorkView)

// Attributes
public:
	CCGWorkDoc* GetDocument();

// Operations
public:

private:
	int m_nAxis;				// Axis of Action, X Y or Z
	int m_nAction;				// Rotate, Translate, Scale
	int m_nView;				// Orthographic, perspective
	bool m_bIsPerspective;			// is the view perspective
	bool should_draw_vertex_normals;
	bool should_draw_polygon_normals;
	bool should_draw_bbox;
	bool should_draw_light_sources;
	bool should_draw_silhouette;
	bool flip_normals;
	bool bf_culling;
	Renderer::Mode render_mode;
	Renderer::NormalMode normal_mode;
	PngWrapper* bg_image = nullptr;

	CString m_strItdFileName;		// file name of IRIT data

	int m_nLightShading;			// shading: Flat, Gouraud.

	double m_lMaterialAmbient;		// The Ambient in the scene
	double m_lMaterialDiffuse;		// The Diffuse in the scene
	double m_lMaterialSpecular;		// The Specular in the scene
	int m_nMaterialCosineFactor;		// The cosine factor for the specular

	Light m_lights[MAX_LIGHT];		//configurable lights array
	Light m_ambientLight;			//ambient light (only RGB is used)
	std::vector<Light> lights;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCGWorkView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CCGWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL InitializeCGWork();
	BOOL SetupViewingFrustum(void);
	BOOL SetupViewingOrthoConstAspect(void);
	void OnMouseDragTransofrmation(int delta_x, int delta_y);
	bool set_bg_image(const char* name, int sw, int sh);

	virtual void RenderScene();


	HGLRC    m_hRC;			// holds the Rendering Context
	CDC*     m_pDC;			// holds the Device Context
	int m_WindowWidth;		// hold the windows width
	int m_WindowHeight;		// hold the windows height
	double m_AspectRatio;		// hold the fixed Aspect Ration

	HBITMAP m_pDbBitMap;
	CDC* m_pDbDC;

	CPoint point, mouse_start_drag_p, prev_mouse_drag_pos;
	clock_t drag_time;
	bool is_LButtonDown;
	CSensitivityDlg sens_dlg;
	CPerspectiveDlg persp_dlg;
	ColorDlg color_dlg;
	FinenessDlg fineness_dlg;
	CLightDialog light_dlg;

// Generated message map functions
protected:
	//{{AFX_MSG(CCGWorkView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLoad();
	afx_msg void OnViewOrthographic();
	afx_msg void OnUpdateViewOrthographic(CCmdUI* pCmdUI);
	afx_msg void OnViewPerspective();
	afx_msg void OnUpdateViewPerspective(CCmdUI* pCmdUI);
	afx_msg void OnViewBBox();
	afx_msg void OnUpdateViewBBox(CCmdUI* pCmdUI);
	afx_msg void OnViewLightSources();
	afx_msg void OnUpdateViewLightSources(CCmdUI* pCmdUI);
	afx_msg void OnViewVertexNormals();
	afx_msg void OnUpdateViewVertexNormals(CCmdUI* pCmdUI);
	afx_msg void OnViewPolygonNormals();
	afx_msg void OnViewFlipNormals();
	afx_msg void OnUpdateViewFlipNormals(CCmdUI* pCmdUI);
	afx_msg void OnViewBFCulling();
	afx_msg void OnUpdateViewBFCulling(CCmdUI* pCmdUI);
	afx_msg void OnViewChangeColors();
	afx_msg void OnUpdateViewPolygonNormals(CCmdUI* pCmdUI);
	afx_msg void OnActionRotate();
	afx_msg void OnUpdateActionRotate(CCmdUI* pCmdUI);
	afx_msg void OnActionScale();
	afx_msg void OnUpdateActionScale(CCmdUI* pCmdUI);
	afx_msg void OnActionTranslate();
	afx_msg void OnUpdateActionTranslate(CCmdUI* pCmdUI);
	afx_msg void OnActionView();
	afx_msg void OnUpdateActionView(CCmdUI* pCmdUI);
	afx_msg void OnActionObject();
	afx_msg void OnUpdateActionObject(CCmdUI* pCmdUI);
	afx_msg void OnAxisX();
	afx_msg void OnUpdateAxisX(CCmdUI* pCmdUI);
	afx_msg void OnAxisY();
	afx_msg void OnUpdateAxisY(CCmdUI* pCmdUI);
	afx_msg void OnAxisZ();
	afx_msg void OnUpdateAxisZ(CCmdUI* pCmdUI);
	afx_msg void OnAxisXY();
	afx_msg void OnUpdateAxisXY(CCmdUI* pCmdUI);
	afx_msg void OnRenderingWireframe();
	afx_msg void OnUpdateRenderingWireframe(CCmdUI* pCmdUI);
	afx_msg void OnRendering2Screen();
	afx_msg void OnUpdateRendering2Screen(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingFlat();
	afx_msg void OnUpdateLightShadingFlat(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingGouraud();
	afx_msg void OnUpdateLightShadingGouraud(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingPhong();
	afx_msg void OnUpdateLightShadingPhong(CCmdUI* pCmdUI);
	afx_msg void OnLightConstants();
	afx_msg void OnSensitivityDialog();
	afx_msg void OnPerspectiveDialog();
	afx_msg void OnViewChangeFineness();
	afx_msg void OnResetView();
	afx_msg void OnCalcNormals();
	afx_msg void OnUpdateCalcNormals(CCmdUI* pCmdUI);
	afx_msg void OnViewSilhouette();
	afx_msg void OnUpdateViewSilhouette(CCmdUI* pCmdUI);

	std::vector<Light> get_lights() const;
	void init_lights();
	void set_defaults();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // debug version in CGWorkView.cpp
inline CCGWorkDoc* CCGWorkView::GetDocument()
   { return (CCGWorkDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
