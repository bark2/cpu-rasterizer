#include "stdafx.h"
#include "iritSkel.h"

#include "cg.hpp"
#include "Scene.h"
#include "Object.h"
#include "Poly.h"
#include "Edge.h"
#include "Vertex.h"

/*****************************************************************************
 * Skeleton for an interface to a parser to read IRIT data files.			 *
 ******************************************************************************
 * (C) Gershon Elber, Technion, Israel Institute of Technology                *
 ******************************************************************************
 * Written by:  Gershon Elber				Ver 1.0, Feb 2002				 *
 * Minimal changes made by Amit Mano			November 2008					 *
 ******************************************************************************/

IPFreeformConvStateStruct CGSkelFFCState = {
	FALSE,          /* Talkative */
	FALSE,          /* DumpObjsAsPolylines */
	TRUE,           /* DrawFFGeom */
	FALSE,          /* DrawFFMesh */
	{ 10, 10, 10 }, /* 10 isocurves peru/v/w direction. */
	100,            /* 100 point samples along a curve. */
	SYMB_CRV_APPROX_UNIFORM,  /* CrvApproxMethod */
	FALSE,   /* ShowIntrnal */
	FALSE,   /* CubicCrvsAprox */
	20,      /* Polygonal FineNess */
	FALSE,   /* ComputeUV */
	TRUE,    /* ComputeNrml */
	FALSE,   /* FourPerFlat */
	0,       /* OptimalPolygons */
	FALSE,   /* BBoxGrid */
	TRUE,    /* LinearOnePolyFlag */
	FALSE
};

//CGSkelProcessIritDataFiles(argv + 1, argc - 1);


/*****************************************************************************
 * DESCRIPTION:                                                               *
 * Main module of skeleton - Read command line and do what is needed...	     *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   FileNames:  Files to open and read, as a vector of strings.              *
 *   NumFiles:   Length of the FileNames vector.								 *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   bool:		false - fail, true - success.                                *
 *****************************************************************************/
bool CGSkelProcessIritDataFiles(CString &FileNames, int NumFiles)
{
	IPObjectStruct *PObjects;
	IrtHmgnMatType CrntViewMat;

	/* Get the data files: */
	IPSetFlattenObjects(FALSE);
	CStringA CStr(FileNames);
	if ((PObjects = IPGetDataFiles((const char* const *)&CStr, 1/*NumFiles*/, TRUE, FALSE)) == NULL)
		return false;
	PObjects = IPResolveInstances(PObjects);

	if (IPWasPrspMat)
		MatMultTwo4by4(CrntViewMat, IPViewMat, IPPrspMat);
	else
		IRIT_GEN_COPY(CrntViewMat, IPViewMat, sizeof(IrtHmgnMatType));

	/* Here some useful parameters to play with in tesselating freeforms: */
	CGSkelFFCState.FineNess = cg_scene.fineness;   /* Res. of tesselation, larger is finer. */
	CGSkelFFCState.ComputeUV = TRUE;   /* Wants UV coordinates for textures. */
	CGSkelFFCState.FourPerFlat = TRUE;/* 4 poly per ~flat patch, 2 otherwise.*/
	CGSkelFFCState.LinearOnePolyFlag = TRUE;    /* Linear srf gen. one poly. */

	/* Traverse ALL the parsed data, recursively. */
	IPTraverseObjListHierarchy(PObjects, CrntViewMat,
                               CGSkelDumpOneTraversedObject);
	return true;
}

/*****************************************************************************
 * DESCRIPTION:                                                               *
 *   Call back function of IPTraverseObjListHierarchy. Called on every non    *
 * list object found in hierarchy.                                            *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   PObj:       Non list object to handle.                                   *
 *   Mat:        Transformation matrix to apply to this object.               *
 *   Data:       Additional data.                                             *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   void									                                 *
 *****************************************************************************/
void CGSkelDumpOneTraversedObject(IPObjectStruct *PObj,
                                  IrtHmgnMatType Mat,
                                  void *Data)
{
	IPObjectStruct *PObjs;

	if (IP_IS_FFGEOM_OBJ(PObj))
		PObjs = IPConvertFreeForm(PObj, &CGSkelFFCState);
	else
		PObjs = PObj;

	for (PObj = PObjs; PObj != NULL; PObj = PObj -> Pnext)
		if (!CGSkelStoreData(PObj)) 
			exit(1);
}


/*****************************************************************************
 * DESCRIPTION:                                                               *
 *   Prints the data from given geometry object.								 *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   PObj:       Object to print.                                             *
 *   Indent:     Column of indentation.                                       *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   bool:		false - fail, true - success.                                *
 *****************************************************************************/
bool CGSkelStoreData(IPObjectStruct *PObj)
{
	const char *Str;
	double RGB_C[3], Transp;
	const IPAttributeStruct *Attrs =
        AttrTraceAttributes(PObj -> Attr, PObj -> Attr);

	if (PObj -> ObjType != IP_OBJ_POLY) {
//		AfxMessageBox(_T("Non polygonal object detected and ignored"));
		return true;
	}

	Object curr_obj;
	bool missing_polygon_normals_info = false;
	bool missing_vertex_normals_info = false;

	/* You can use IP_IS_POLYGON_OBJ(PObj) and IP_IS_POINTLIST_OBJ(PObj) 
	   to identify the type of the object*/

	if (CGSkelGetObjectColor(PObj, RGB_C))
	{
		curr_obj.set_color(RGB(RGB_C[0] * 255, RGB_C[1] * 255, RGB_C[2] * 255));
	}
	if (CGSkelGetObjectTransp(PObj, &Transp))
	{
		/* transparency code */
	}
	if ((Str = CGSkelGetObjectTexture(PObj)) != NULL)
	{
		/* volumetric texture code */
	}
	if ((Str = CGSkelGetObjectPTexture(PObj)) != NULL)
	{
		/* parametric texture code */
	}
	if (Attrs != NULL) 
	{
		printf("[OBJECT\n");
		while (Attrs) {
			/* attributes code */
			Attrs = AttrTraceAttributes(Attrs, NULL);
		}
	}
	
	for (IPPolygonStruct *PPolygon = PObj->U.Pl; PPolygon != NULL; PPolygon = PPolygon->Pnext)
	{
		if (PPolygon->PVertex == NULL) {
			AfxMessageBox(_T("Dump: Attempt to dump empty polygon"));
			return false;
		}

		/* use if(IP_HAS_PLANE_POLY(PPolygon)) to know whether a normal is defined for the polygon
		access the normal by the first 3 components of PPolygon->Plane */

		IPVertexStruct *PPREV_V = PPolygon->PVertex;

		bool is_new;
		vec3 curr_vec{ PPREV_V->Coord[0], PPREV_V->Coord[1], PPREV_V->Coord[2] };
		Vertex* first_v = curr_obj.insert_vertex(curr_vec, is_new);
		if (is_new)
			cg_scene.insert_vertex(first_v);
		Poly* curr_p = new Poly();
		Edge* curr_e;
		Vertex* prev_v = first_v;
		Vertex* curr_v;


		first_v->insert_polygon(curr_p);
		curr_p->insert_vertex(first_v);

		if (IP_HAS_PLANE_POLY(PPolygon))
			curr_p->pars_normal = UnitVector({ PPolygon->Plane[0], PPolygon->Plane[1], PPolygon->Plane[2] });
		else
			missing_polygon_normals_info = true;
		if (IP_HAS_NORMAL_VRTX(PPREV_V))
			first_v->pars_normal = UnitVector({ PPREV_V->Normal[0], PPREV_V->Normal[1], PPREV_V->Normal[2] });
		else
			missing_vertex_normals_info = true;

		for (IPVertexStruct *PV = PPREV_V->Pnext;
			PV != nullptr && PV != PPolygon->PVertex;
			PPREV_V = PV, PV = PV->Pnext, prev_v = curr_v) {

			curr_vec = vec3{ PV->Coord[0], PV->Coord[1], PV->Coord[2] };
			curr_v = curr_obj.insert_vertex(curr_vec, is_new);
			if (is_new)
				cg_scene.insert_vertex(curr_v);
			curr_e = curr_obj.insert_edge(prev_v, curr_v, is_new);
			if (is_new)
				cg_scene.insert_edge(curr_e);
			curr_v->insert_polygon(curr_p);
			curr_v->insert_edge(curr_e);
			curr_p->insert_edge(curr_e, is_new);
			curr_p->insert_vertex(curr_v);
			curr_e->set_polygon(curr_p);

			if (IP_HAS_NORMAL_VRTX(PV))
				curr_v->pars_normal = UnitVector({ PV->Normal[0], PV->Normal[1], PV->Normal[2] });
			else
				missing_vertex_normals_info = true;
		}

		curr_e = curr_obj.insert_edge(curr_v, first_v, is_new);
		if (is_new)
			cg_scene.insert_edge(curr_e);
		curr_p->insert_edge(curr_e, is_new);
		curr_v->insert_edge(curr_e);
		curr_e->set_polygon(curr_p);

		vec3 poly_center{ 0,0,0 };
		for (auto& v : curr_p->get_vertices())
			poly_center += (vec3)(v->vec);
		curr_p->center = poly_center / curr_p->get_vertices().size();

		curr_p->initialize_calc_normal();
		if (missing_polygon_normals_info) {
			curr_p->pars_normal = curr_p->calc_normal;
		}

		cg_scene.insert_polygon(curr_p);
		curr_obj.insert_polygon(curr_p);
	}

	curr_obj.calc_vertex_normals();
	if (missing_vertex_normals_info) {
		for (auto v : curr_obj.get_vertices())
			v->pars_normal = v->calc_normal;
	}
	curr_obj.set_bbox();
	curr_obj.missing_polygon_normals_info = missing_polygon_normals_info;
	curr_obj.missing_vertex_normals_info = missing_vertex_normals_info;
	cg_scene.push_back(curr_obj);

	/* Close the object. */
	return true;
}

/*****************************************************************************
 * DESCRIPTION:                                                               *
 *   Returns the color of an object.                                          *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   PObj:   Object to get its color.                                         *
 *   RGB:    as 3 floats in the domain [0, 1].                                *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   int:    TRUE if object has color, FALSE otherwise.                       *
 *****************************************************************************/
int CGSkelGetObjectColor(IPObjectStruct *PObj, double RGB[3])
{
	static int TransColorTable[][4] = {
		{ /* BLACK	*/   0,    0,   0,   0 },
		{ /* BLUE	*/   1,    0,   0, 255 },
		{ /* GREEN	*/   2,    0, 255,   0 },
		{ /* CYAN	*/   3,    0, 255, 255 },
		{ /* RED	*/   4,  255,   0,   0 },
		{ /* MAGENTA 	*/   5,  255,   0, 255 },
		{ /* BROWN	*/   6,   50,   0,   0 },
		{ /* LIGHTGRAY	*/   7,  127, 127, 127 },
		{ /* DARKGRAY	*/   8,   63,  63,  63 },
		{ /* LIGHTBLUE	*/   9,    0,   0, 255 },
		{ /* LIGHTGREEN	*/   10,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   11,   0, 255, 255 },
		{ /* LIGHTRED	*/   12, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 13, 255,   0, 255 },
		{ /* YELLOW	*/   14, 255, 255,   0 },
		{ /* WHITE	*/   15, 255, 255, 255 },
		{ /* BROWN	*/   20,  50,   0,   0 },
		{ /* DARKGRAY	*/   56,  63,  63,  63 },
		{ /* LIGHTBLUE	*/   57,   0,   0, 255 },
		{ /* LIGHTGREEN	*/   58,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   59,   0, 255, 255 },
		{ /* LIGHTRED	*/   60, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 61, 255,   0, 255 },
		{ /* YELLOW	*/   62, 255, 255,   0 },
		{ /* WHITE	*/   63, 255, 255, 255 },
		{		     -1,   0,   0,   0 }
	};
	int i, j, Color, RGBIColor[3];

	if (AttrGetObjectRGBColor(PObj,
                              &RGBIColor[0], &RGBIColor[1], &RGBIColor[2])) {
        for (i = 0; i < 3; i++)
            RGB[i] = RGBIColor[i] / 255.0;

        return TRUE;
	}
	else if ((Color = AttrGetObjectColor(PObj)) != IP_ATTR_NO_COLOR) {
		for (i = 0; TransColorTable[i][0] >= 0; i++) {
			if (TransColorTable[i][0] == Color) {
				for (j = 0; j < 3; j++)
					RGB[j] = TransColorTable[i][j+1] / 255.0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
 * DESCRIPTION:                                                               *
 *   Returns the volumetric texture of an object, if any.                     *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   PObj:   Object to get its volumetric texture.                            *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   char *:    Name of volumetric texture map to apply, NULL if none.        *
 *****************************************************************************/
const char *CGSkelGetObjectTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "texture");
}

/*****************************************************************************
 * DESCRIPTION:                                                               *
 *   Returns the parametric texture of an object, if any.                     *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   PObj:   Object to get its parametric texture.                            *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   char *:    Name of parametric texture map to apply, NULL if none.        *
 *****************************************************************************/
const char *CGSkelGetObjectPTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "ptexture");
}

/*****************************************************************************
 * DESCRIPTION:                                                               *
 *   Returns the transparency level of an object, if any.                     *
 *                                                                            *
 * PARAMETERS:                                                                *
 *   PObj:   Object to get its volumetric texture.                            *
 *   Transp: Transparency level between zero and one.                         *
 *                                                                            *
 * RETURN VALUE:                                                              *
 *   int:    TRUE if object has transparency, FALSE otherwise.                *
 *****************************************************************************/
int CGSkelGetObjectTransp(IPObjectStruct *PObj, double *Transp)
{
	*Transp = AttrGetObjectRealAttrib(PObj, "transp");

	return !IP_ATTR_IS_BAD_REAL(*Transp);
}

