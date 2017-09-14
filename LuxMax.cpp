/***************************************************************************
* Copyright 1998-2015 by authors (see AUTHORS.txt)                        *
*                                                                         *
*   This file is part of LuxRender.                                       *
*                                                                         *
* Licensed under the Apache License, Version 2.0 (the "License");         *
* you may not use this file except in compliance with the License.        *
* You may obtain a copy of the License at                                 *
*                                                                         *
*     http://www.apache.org/licenses/LICENSE-2.0                          *
*                                                                         *
* Unless required by applicable law or agreed to in writing, software     *
* distributed under the License is distributed on an "AS IS" BASIS,       *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
* See the License for the specific language governing permissions and     *
* limitations under the License.                                          *
***************************************************************************/

#define CAMERAHELPER_CLASSID Class_ID(4128,0)
#define MAX2016_PHYSICAL_CAMERA Class_ID(1181315608,686293133)

#define OMNI_CLASSID Class_ID(4113, 0)
#define SPOTLIGHT_CLASSID Class_ID(4114,0)

#define SKYLIGHT_CLASSID Class_ID(2079724664, 1378764549)
#define DIRLIGHT_CLASSID Class_ID(4115, 0) // free directional light and sun light classid

#include <limits>
#include <limits.h>
#include "LuxMaxpch.h"
#include "resource.h"
#include "LuxMax.h"

namespace luxcore
{
//#include <luxcore/luxcore.h>
//#include <luxcoreimpl.h>
#include <luxcore\luxcore.h>
}


#include "LuxMaxCamera.h"
#include "LuxMaxUtils.h"
#include "LuxMaxMaterials.h"
#include "LuxMaxLights.h"
#include "LuxMaxMesh.h"

#include <maxscript\maxscript.h>
#include <render.h>
#include <point3.h>
#include <MeshNormalSpec.h>
#include <Path.h>
#include <bitmap.h>
#include <GraphicsWindow.h>
#include <IColorCorrectionMgr.h>
#include <IGame\IGame.h>
#include <VertexNormal.h>
#include <string>
#include <string.h>
#include <iostream>
#include <IMaterialBrowserEntryInfo.h>
#include <units.h>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <mesh.h>
#include <locale>
#include <sstream>

LuxMaxCamera lxmCamera;
LuxMaxLights lxmLights;
LuxMaxMaterials lxmMaterials;
LuxMaxUtils lxmUtils;
LuxMaxMesh lxmMesh;

#pragma warning (push)
#pragma warning( disable:4002)
#pragma warning (pop)

using namespace std;
using namespace luxcore;
using namespace luxrays;

extern BOOL FileExists(const TCHAR *filename);
float* pixels;
Scene *materialPreviewScene = NULL;
bool defaultlightset = true;
int rendertype = 4;
int renderWidth = 0;
int renderHeight = 0;
bool renderingMaterialPreview = false;
int vfbRefreshRateInt = 1;
//luxcore::Scene *materialPreviewScene;// = new Scene();

int filterIndex;
bool enableFileSaverOutput;

class LuxMaxClassDesc :public ClassDesc2 {
public:
	virtual int 			IsPublic() { return 1; }
	virtual void *			Create(BOOL loading) { UNREFERENCED_PARAMETER(loading); return new LuxMax; }
	virtual const TCHAR *	ClassName() { return GetString(IDS_VRENDTITLE); }
	virtual SClass_ID		SuperClassID() { return RENDERER_CLASS_ID; }
	virtual Class_ID 		ClassID() { return REND_CLASS_ID; }
	virtual const TCHAR* 	Category() { return _T(""); }
	virtual void			ResetClassParams(BOOL fileReset) { UNREFERENCED_PARAMETER(fileReset); }
};

ClassDesc2* GetRendDesc() {
	static LuxMaxClassDesc srendCD;
	return &srendCD;
}

/*enum { lens_params };
enum { lensradius_spin };

/*const int MIN_SPIN = 0.0f;
const int MAX_SPIN = 100.0f;*/
/*

static ParamBlockDesc2 DepthOfFieldblk(lens_params, _T("Simple Parameters"), 0, GetRendDesc(), P_AUTO_CONSTRUCT + P_AUTO_UI, 1,
//rollout
IDD_DEPTH, IDS_DEPTH, 0, 0, NULL,
// params
lensradius_spin,		_T("spin"),			TYPE_FLOAT,		P_ANIMATABLE,	IDS_SPIN,
p_default,			0.1f,
p_range,			0.0f, 1000.0f,
p_ui,				TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_LENSRADIUS, IDC_LENSRADIUS_SPIN, 0.01f,
p_end,
p_end
);*/

class UV {
public:
	UV(float _u = 0.f, float _v = 0.f)
		: u(_u), v(_v) {
	}

	UV(const float v[2]) : u(v[0]), v(v[1]) {
	}

	float u, v;
};

class Point {
public:
	Point(float _x = 0.f, float _y = 0.f, float _z = 0.f)
		: x(_x), y(_y), z(_z) {
	}

	Point(const float v[3]) : x(v[0]), y(v[1]), z(v[2]) {
	}

	float x, y, z;
};

class Triangle {
public:
	Triangle() { }
	Triangle(const unsigned int v0, const unsigned int v1, const unsigned int v2) {
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
	}

	unsigned int v[3];
};

RefResult LuxMax::NotifyRefChanged(const Interval &changeInt, RefTargetHandle hTarget, PartID &partID,
	RefMessage message, BOOL propagate)
{
	UNREFERENCED_PARAMETER(propagate);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(partID);
	UNREFERENCED_PARAMETER(hTarget);
	UNREFERENCED_PARAMETER(changeInt);
	/*switch (message)
	{
	case REFMSG_CHANGE:
	{
	if (hTarget == pblock)
	{
	ParamID changing_param = pblock->LastNotifyParamID();
	DepthOfFieldblk.InvalidateUI(changing_param);
	}
	}
	break;
	}*/
	return REF_SUCCEED;
}



class BBox {
public:
	// BBox Public Methods

	BBox() {
		pMin = ::Point(numeric_limits<float>::infinity(),
			numeric_limits<float>::infinity(),
			numeric_limits<float>::infinity());
		pMax = ::Point(-numeric_limits<float>::infinity(),
			-numeric_limits<float>::infinity(),
			-numeric_limits<float>::infinity());
	}

	BBox(const ::Point &p1, const ::Point &p2) {
		pMin = p1;
		pMax = p2;
	}

	Point pMin, pMax;
};

static void CreateBox(Scene *scene, const string &objName, const string &meshName,
	const string &matName, const bool enableUV, const BBox &bbox) {
	Point *p = (Point *)Scene::AllocVerticesBuffer(24);
	// Bottom face
	p[0] = Point(bbox.pMin.x, bbox.pMin.y, bbox.pMin.z);
	p[1] = Point(bbox.pMin.x, bbox.pMax.y, bbox.pMin.z);
	p[2] = Point(bbox.pMax.x, bbox.pMax.y, bbox.pMin.z);
	p[3] = Point(bbox.pMax.x, bbox.pMin.y, bbox.pMin.z);
	// Top face
	p[4] = Point(bbox.pMin.x, bbox.pMin.y, bbox.pMax.z);
	p[5] = Point(bbox.pMax.x, bbox.pMin.y, bbox.pMax.z);
	p[6] = Point(bbox.pMax.x, bbox.pMax.y, bbox.pMax.z);
	p[7] = Point(bbox.pMin.x, bbox.pMax.y, bbox.pMax.z);
	// Side left
	p[8] = Point(bbox.pMin.x, bbox.pMin.y, bbox.pMin.z);
	p[9] = Point(bbox.pMin.x, bbox.pMin.y, bbox.pMax.z);
	p[10] = Point(bbox.pMin.x, bbox.pMax.y, bbox.pMax.z);
	p[11] = Point(bbox.pMin.x, bbox.pMax.y, bbox.pMin.z);
	// Side right
	p[12] = Point(bbox.pMax.x, bbox.pMin.y, bbox.pMin.z);
	p[13] = Point(bbox.pMax.x, bbox.pMax.y, bbox.pMin.z);
	p[14] = Point(bbox.pMax.x, bbox.pMax.y, bbox.pMax.z);
	p[15] = Point(bbox.pMax.x, bbox.pMin.y, bbox.pMax.z);
	// Side back
	p[16] = Point(bbox.pMin.x, bbox.pMin.y, bbox.pMin.z);
	p[17] = Point(bbox.pMax.x, bbox.pMin.y, bbox.pMin.z);
	p[18] = Point(bbox.pMax.x, bbox.pMin.y, bbox.pMax.z);
	p[19] = Point(bbox.pMin.x, bbox.pMin.y, bbox.pMax.z);
	// Side front
	p[20] = Point(bbox.pMin.x, bbox.pMax.y, bbox.pMin.z);
	p[21] = Point(bbox.pMin.x, bbox.pMax.y, bbox.pMax.z);
	p[22] = Point(bbox.pMax.x, bbox.pMax.y, bbox.pMax.z);
	p[23] = Point(bbox.pMax.x, bbox.pMax.y, bbox.pMin.z);

	Triangle *vi = (Triangle *)Scene::AllocTrianglesBuffer(12);
	// Bottom face
	vi[0] = Triangle(0, 1, 2);
	vi[1] = Triangle(2, 3, 0);
	// Top face
	vi[2] = Triangle(4, 5, 6);
	vi[3] = Triangle(6, 7, 4);
	// Side left
	vi[4] = Triangle(8, 9, 10);
	vi[5] = Triangle(10, 11, 8);
	// Side right
	vi[6] = Triangle(12, 13, 14);
	vi[7] = Triangle(14, 15, 12);
	// Side back
	vi[8] = Triangle(16, 17, 18);
	vi[9] = Triangle(18, 19, 16);
	// Side back
	vi[10] = Triangle(20, 21, 22);
	vi[11] = Triangle(22, 23, 20);

	// Define the Mesh
	if (!enableUV) {
		// Define the object
		scene->DefineMesh(meshName, 24, 12, (float *)p, (unsigned int *)vi, NULL, NULL, NULL, NULL);
	}
	else {
		UV *uv = new UV[24];
		// Bottom face
		uv[0] = UV(0.f, 0.f);
		uv[1] = UV(1.f, 0.f);
		uv[2] = UV(1.f, 1.f);
		uv[3] = UV(0.f, 1.f);
		// Top face
		uv[4] = UV(0.f, 0.f);
		uv[5] = UV(1.f, 0.f);
		uv[6] = UV(1.f, 1.f);
		uv[7] = UV(0.f, 1.f);
		// Side left
		uv[8] = UV(0.f, 0.f);
		uv[9] = UV(1.f, 0.f);
		uv[10] = UV(1.f, 1.f);
		uv[11] = UV(0.f, 1.f);
		// Side right
		uv[12] = UV(0.f, 0.f);
		uv[13] = UV(1.f, 0.f);
		uv[14] = UV(1.f, 1.f);
		uv[15] = UV(0.f, 1.f);
		// Side back
		uv[16] = UV(0.f, 0.f);
		uv[17] = UV(1.f, 0.f);
		uv[18] = UV(1.f, 1.f);
		uv[19] = UV(0.f, 1.f);
		// Side front
		uv[20] = UV(0.f, 0.f);
		uv[21] = UV(1.f, 0.f);
		uv[22] = UV(1.f, 1.f);
		uv[23] = UV(0.f, 1.f);

		// Define the object
		scene->DefineMesh(meshName, 24, 12, (float *)p, (unsigned int *)vi, NULL, (float *)uv, NULL, NULL);
	}
	// Add the object to the scene
	Properties props;
	props.SetFromString(
		"scene.objects." + objName + ".shape = " + meshName + "\n"
		"scene.objects." + objName + ".material = " + matName + "\n"
		);
	scene->Parse(props);
}

Mtl * matPrevNodesEnum(INode * inode)
{
	//for (int c = 0; c < inode->NumberOfChildren(); c++)
	//{
	//	Mtl * mat = matPrevNodesEnum(inode->GetChildNode(c));
	//	if (mat)
	//	{
	//		//mprintf(_T("\nMaterial Name: %s\n", mat->GetName()));
	//		lxmMaterials.exportMaterial(mat, *materialPreviewScene);
	//		return mat;
	//	}
	//}

	ObjectState ostate = inode->EvalWorldState(GetCOREInterface()->GetTime());
	if (ostate.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
		if (ostate.obj->CanConvertToType(triObjectClassID))
		{
			Object * obj = ostate.obj;
			if (!obj)
				return NULL;
			TriObject * tobj = (TriObject *)obj->ConvertToType(0, triObjectClassID);
			if (!tobj)
				return NULL;
			::Mesh * cmesh = &(tobj->mesh);

			lxmMaterials.exportMaterial(inode->GetMtl(), *materialPreviewScene);
			if (!cmesh)
				return NULL;

			return inode->GetMtl();
		}
	return NULL;
}

::Matrix3 camPos;

int LuxMax::Open(
	INode *scene,     	// root node of scene to render
	INode *vnode,     	// view node (camera or light), or NULL
	ViewParams *viewPar,// view params for rendering ortho or user viewport
	RendParams &rp,  	// common renderer parameters
	HWND hwnd, 				// owner window, for messages
	DefaultLight* defaultLights, // Array of default lights if none in scene
	int numDefLights,	// number of lights in defaultLights array
	RendProgressCallback* prog
	)
{
	UNREFERENCED_PARAMETER(prog);
	UNREFERENCED_PARAMETER(numDefLights);
	UNREFERENCED_PARAMETER(defaultLights);
	UNREFERENCED_PARAMETER(hwnd);
	
	viewNode = vnode;
	camPos = viewPar->affineTM;

	if (rp.inMtlEdit)
	{
		renderingMaterialPreview = true;
		//Scene* materialPreviewScene = NULL;// *= new Scene();
		//Scene *materialPreviewScene = Scene::Create();
		materialPreviewScene = Scene::Create();
		lxmMesh.createMesh(scene, *materialPreviewScene,GetCOREInterface()->GetTime());
	}
	else
	{
		renderingMaterialPreview = false;
	}

	return 1;
}

static void DoRendering(RenderSession *session, RendProgressCallback *prog, Bitmap *tobm) {
	const unsigned int haltTime = session->GetRenderConfig().GetProperties().Get(Property("batch.halttime")(0)).Get<unsigned int>();
	const unsigned int haltSpp = session->GetRenderConfig().GetProperties().Get(Property("batch.haltspp")(0)).Get<unsigned int>();
	const float haltThreshold = session->GetRenderConfig().GetProperties().Get(Property("batch.haltthreshold")(-1.f)).Get<float>();
	const wchar_t *state = NULL;
	
	//char buf[512];
	const Properties &stats = session->GetStats();
	
	for (;;) {
		//boost::this_thread::sleep(boost::posix_time::millisec(10000));

		session->UpdateStats();
		const double elapsedTime = stats.Get("stats.renderengine.time").Get<double>();
		if ((haltTime > 0) && (elapsedTime >= haltTime))
			break;

		const unsigned int pass = stats.Get("stats.renderengine.pass").Get<unsigned int>();
		if ((haltSpp > 0) && (pass >= haltSpp))
			break;

		// Convergence test is update inside UpdateFilm()
		const float convergence = stats.Get("stats.renderengine.convergence").Get<unsigned int>();
		if ((haltThreshold >= 0.f) && (1.f - convergence <= haltThreshold))
			break;

		// Print some information about the rendering progress
		//sprintf(buf, "[Elapsed time: %3d/%dsec][Samples %4d/%d][Convergence %f%%][Avg. samples/sec % 3.2fM on %.1fK tris]",
		//			int(elapsedTime), int(haltTime), pass, haltSpp, 100.f * convergence,
		//		stats.Get("stats.renderengine.total.samplesec").Get<double>() / 1000000.0,
		//	stats.Get("stats.dataset.trianglecount").Get<double>() / 1000.0);
		//mprintf(_T("Elapsed time %i\n"), int(elapsedTime));

		wchar_t passWstr[16];
		wsprintf(passWstr, L"Rendering pass:%d", pass);
		prog->SetTitle(passWstr);
		
		renderWidth = tobm->Width();
		renderHeight = tobm->Height();
		int pixelArraySize = renderWidth * renderHeight * 3;
		float* pixels = new float[pixelArraySize]();

		bool renderabort = prog->Progress(elapsedTime + 1, haltTime);
		if (renderabort == false)
		{
			delete[] pixels;
			session->Stop();
			break;
		}

		//session->GetFilm().GetOutput(session->GetFilm().OUTPUT_RGB_TONEMAPPED, pixels, 0);
		session->GetFilm().GetOutput<float>(Film::OUTPUT_RGB_IMAGEPIPELINE, pixels);
		int i = 0;

		BMM_Color_64 col64;
		col64.r = 0;
		col64.g = 0;
		col64.b = 0;
		//fill in the pixels
		for (int w = renderHeight; w > 0; w--)
		{
			for (int h = 0; h < renderWidth; h++)
			{
				col64.r = (WORD)floorf(pixels[i] * 65535.f + .5f);
				col64.g = (WORD)floorf(pixels[i + 1] * 65535.f + .5f);
				col64.b = (WORD)floorf(pixels[i + 2] * 65535.f + .5f);
				
				tobm->PutPixels(h, w, 1, &col64);
				
				i += 3;
			}
		}
		tobm->RefreshWindow(NULL);
		
		col64 = NULL;
		delete pixels;
		
		//SLG_LOG(buf);
	}
	
}


void parseObjects(INode *currNode, luxcore::Scene &scene , TimeValue &t)
{
	Object*	obj;
	ObjectState os = currNode->EvalWorldState(t);
	obj = os.obj;
	bool doExport = true;

	for (int a = 0; currNode->NumChildren() > a; a++)
	{
		INode *childNode = currNode->GetChildNode(a);
		if (childNode != NULL)
		{
			parseObjects(childNode, scene,t);
		}
	}

	if (currNode->IsHidden(0, true))
	{
		doExport = false;
	}

	switch (os.obj->SuperClassID())
	{
	case HELPER_CLASS_ID:
	{
		doExport = false;

		//If the helper is a group header we loop through and export all objects inside the group.
		if (currNode->IsGroupHead())
		{
			lxmMesh.createMeshesInGroup(currNode, scene,t);
		}
		break;
	}

	if (doExport)
	{
	case LIGHT_CLASS_ID:
	{
		//Properties props;
		std::string objString;
		//bool lightsupport = false;

		//if (defaultlightchk == true)
		//{
		//	if (defaultlightauto == true)
		//	{
		//		defaultlightset = false;
		//	}
		//}
		//else
		//{
		//	defaultlightset = false;
		//	//mprintf(_T("\n Default Light Deactive Automaticlly %i \n"));
		//	OutputDebugStringW(L"Default Light deactivate automatically.");
		//}

		if (os.obj->ClassID() == OMNI_CLASSID)
		{
			scene.Parse(lxmLights.exportOmni(currNode));

			//lightsupport = true;
		}
		if (os.obj->ClassID() == SPOTLIGHT_CLASSID)
		{
			scene.Parse(lxmLights.exportSpotLight(currNode));
			//lightsupport = true;
		}
		if (os.obj->ClassID() == SKYLIGHT_CLASSID)
		{
			scene.Parse(lxmLights.exportSkyLight(currNode));
			//lightsupport = true;
		}
		if (os.obj->ClassID() == DIRLIGHT_CLASSID)
		{
			scene.Parse(lxmLights.exportDiright(currNode));
			//lightsupport = true;
		}
		//if (lightsupport == false)
		//{
		//	if (defaultlightchk == true)
		//	{
		//		//mprintf(_T("\n There is No Suported light in scene %i \n"));
		//		OutputDebugStringW(L"No supported light in the scene.");
		//		defaultlightset = true;
		//	}
		//}

		break;
	}
	}

	if (os.obj->ClassID() == XREFOBJ_CLASS_ID)
	{
		//mprintf(_T("\n There is a xref node in the scene, will not render yet. please merge scene. \n"));
		OutputDebugStringW(L"Xref is unsupported, merge scene instead.");
		break;
	}

	case GEOMOBJECT_CLASS_ID:
	{
		if (doExport)
		{
			lxmMesh.createMesh(currNode, scene,t);
		}
		break;
	}
	}
}

int LuxMax::Render(
	TimeValue t,   			// frame to render.
	Bitmap *tobm, 			// optional target bitmap
	FrameRendParams &frp,	// Time dependent parameters
	HWND hwnd, 				// owner window
	RendProgressCallback *prog,
	ViewParams *vp
	)
{
	UNREFERENCED_PARAMETER(vp);
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(frp);
	//UNREFERENCED_PARAMETER(t);
	UNREFERENCED_PARAMETER(prog);
	
	using namespace std;
	using namespace luxrays;
	using namespace luxcore;
	const wchar_t *renderProgTitle = NULL;
	defaultlightset = true;

	//mprintf(_T("\nRendering with Luxcore version: %s,%s \n"), LUXCORE_VERSION_MAJOR, LUXCORE_VERSION_MINOR);

	if (renderingMaterialPreview)
	{
		//materialPreviewScene = Scene::Create();

		materialPreviewScene->Parse(
			Property("scene.materials.mat_dummy.type")("matte") <<
			Property("scene.materials.mat_dummy.kd")(1.0f, 1.f, 1.f)
			);
		CreateBox(materialPreviewScene, "dummybox", "dummyboxmesh", "mat_dummy", false, BBox(Point(-.001f, -.001f, .001f), Point(.05f, .05f, 0.07f)));

		defaultlightset = false;
		renderWidth = tobm->Width();
		renderHeight = tobm->Height();

		float previewCameraDistance = 6 / lxmUtils.GetMeterMult();
		
		materialPreviewScene->Parse(
			Property("scene.camera.lookat.orig")(previewCameraDistance, previewCameraDistance, previewCameraDistance) <<
			Property("scene.camera.lookat.target")(0.f, 0.f, 0.f) <<
			Property("scene.camera.fieldofview")(35.f)
			);

		//Property("scene.camera.fieldofview")(35.f)
		//Instead of the preview sky light, we should fetch max's internal lights for material previews.
		lxmLights.exportDefaultSkyLight(materialPreviewScene);

		
		//PATHCPU was default here.. 
	
		std::string tmpRenderConfig;
		
		tmpRenderConfig.append("opencl.cpu.use = 0\n");
		tmpRenderConfig.append("opencl.gpu.use = 0\n");
		tmpRenderConfig.append("batch.halttime = " + std::to_string(3) + "\n");
		tmpRenderConfig.append("film.imagepipeline.0.type = TONEMAP_AUTOLINEAR\n");
		tmpRenderConfig.append("film.opencl.enable = 0\n");
		tmpRenderConfig.append("film.opencl.platform = -1\n");
		tmpRenderConfig.append("film.imagepipeline.1.type = GAMMA_CORRECTION\n");
		tmpRenderConfig.append("film.imagepipeline.1.value = 1.0\n");
		tmpRenderConfig.append("film.height = " + std::to_string(renderHeight) + "\n");
		tmpRenderConfig.append("film.width = " + std::to_string(renderWidth) + "\n");
		tmpRenderConfig.append("renderengine.type = PATHCPU\n");
		//tmpRenderConfig.append("sampler.type = SOBOL\n");
		tmpRenderConfig.append("sampler.type = RANDOM\n");
		//tmpRenderConfig.append("film.filter.type = Box\n");

		luxrays::Properties renderConfigProp;
		renderConfigProp.SetFromString(tmpRenderConfig);
		RenderConfig *config = RenderConfig::Create(renderConfigProp, materialPreviewScene);
		RenderSession *session = RenderSession::Create(config);
		session->Start();
		DoRendering(session, prog, tobm);
		session->Stop();
		delete session;
		return 1;
	}
	else
	{
		
		//Scene *scene = NULL;
		Scene *scene = Scene::Create();
		//In the camera 'export' function we check for supported camera, it returns false if something is not right.
		if (!lxmCamera.exportCamera((float)atof(LensRadiusstr.ToCStr()), *scene,t))
		{
			return false;
		}

		//Export all meshes
		INode* maxscene = GetCOREInterface7()->GetRootNode();
		for (int a = 0; maxscene->NumChildren() > a; a++)
		{
			INode* currNode = maxscene->GetChildNode(a);
			renderProgTitle = (L"Translating object: %s", currNode->GetName());
			prog->SetTitle(renderProgTitle);
			//	mprintf(_T("\n Total Rendering elements number: %i"), maxscene->NumChildren());
			//		mprintf(_T("   ::   Current elements number: %i \n"), a + 1);
			prog->Progress(a + 1, maxscene->NumChildren());

			parseObjects(currNode, *scene,t);
		}
		
		//std::string tmpFilename = FileName.ToCStr();
		int halttime = (int)_wtof(halttimewstr);
		vfbRefreshRateInt = (int)_wtof(vbinterval);
		
		//if (tmpFilename != NULL)
		//{
			//mprintf(_T("\nRendering to: %s \n"), FileName.ToMSTR());
			//OutputDebugStringW(L"\nRendering to: ");
			//OutputDebugStringW(tmpFilename);
		//}

		renderWidth = GetCOREInterface11()->GetRendWidth();
		renderHeight = GetCOREInterface11()->GetRendHeight();

		string tmprendtype = "PATHCPU";
		rendertype = (int)_wtoi(RenderTypeWstr);
		
		switch (rendertype)
		{
			case 0:
			{
				//Crashes with latest build
				//tmprendtype = "BIDIRCPU";
			}

			case 1:
			{
				tmprendtype = "BIDIRVMCPU";
				break;
			}
			case 2:
			{
				tmprendtype = "LIGHTCPU";
				break;
			}
			case 3:
			{
				tmprendtype = "PATHCPU";
				break;
			}
			case 4:
			{
				tmprendtype = "TILEPATHCPU";
				break;
			}
			case 5:
			{
				//tmprendtype = "RTBIASPATHOCL";
				tmprendtype = "TILEPATHOCL"; 
				break;
			}
			case 6:
			{
				//tmprendtype = "RTPATHOCL";
				break;
			}
		}
		
		std::string filterName = "";
		filterIndex = (int)_wtoi(FilterIndexWstr);
		switch (filterIndex)
		{
		case 0:
			filterName = "BLACKMANHARRIS";
			break;
		case 1:
			filterName = "BOX";
			break;
		case 2:
			filterName = "GAUSSIAN";
			break;
		case 3:
			filterName = "MITCHELL";
			break;
		case 4:
			filterName = "MITCHELL_SS";
			break;
		case 5:
			filterName = "NONE";
			break;

			break;
		}

		luxrays::Properties renderConfigProp;
		std::string tmpRenderConfig;
		if ((int)_wtoi(enableFileSaverOutoutWstr))
		{
			tmpRenderConfig.append("renderengine.type = FILESAVER\n");
			std::string filenameString = lxmUtils.ToNarrow(FileName);
			std::replace(filenameString.begin(), filenameString.end(), '\\', '/');
			OutputDebugStringW(L"Filesaver output path:\n");
			OutputDebugStringA(filenameString.c_str());
			tmpRenderConfig.append("filesaver.directory = " + filenameString +"\n");
			
			MessageBox(0, L"Files have been written do disk (Filesaver output).\nPress 'Cancel' in the render dialog to stop, browse to folder to view output files..", L"FileSaver output!", MB_OK);
			
		}
		else
		{
			tmpRenderConfig.append("renderengine.type = " + tmprendtype + "\n");
		}
		
		//GeForce GTX 1060 3GB
		tmpRenderConfig.append("opencl.cpu.use = 0\n");
		tmpRenderConfig.append("opencl.gpu.use = 0\n");
		//tmpRenderConfig.append("opencl.devices.select = 1\n");
		tmpRenderConfig.append("batch.halttime = " + std::to_string(halttime) + "\n");
		tmpRenderConfig.append("film.imagepipeline.0.type = TONEMAP_AUTOLINEAR\n");
		tmpRenderConfig.append("film.opencl.enable = 0\n");
		tmpRenderConfig.append("film.opencl.platform = -1\n");
		//tmpRenderConfig.append("opencl.gpu.workgroup.size = 64\n");
		//tmpRenderConfig.append("opencl.cpu.workgroup.size = 64\n");
		//tmpRenderConfig.append("film.opencl.device = -1\n");
		tmpRenderConfig.append("film.imagepipeline.1.type = GAMMA_CORRECTION\n");
		tmpRenderConfig.append("film.imagepipeline.1.value = 1.0\n");
		//tmpRenderConfig.append("film.imagepipeline.0.table.size = 4096\n"); 
		tmpRenderConfig.append("film.height = " + std::to_string(renderHeight) + "\n");
		tmpRenderConfig.append("film.width = " + std::to_string(renderWidth) + "\n");
		tmpRenderConfig.append("film.filter.type = " + filterName + "\n");
		if (filterIndex != 5)
		{
			tmpRenderConfig.append("film.filter.xwidth = " + lxmUtils.ToNarrow(FilterXWidthWst) + "\n");
			tmpRenderConfig.append("film.filter.ywidth = " + lxmUtils.ToNarrow(FilterYWidthWst) + "\n");
		}
		
		if (filterIndex == 2)
		{
			tmpRenderConfig.append("film.filter.gaussian.alpha =" + lxmUtils.ToNarrow(FilterGuassianAlphaWstr) + "\n");
		}
		if (filterIndex == 3)
		{
			tmpRenderConfig.append("film.filter.mitchell.a" + lxmUtils.ToNarrow(FilterMitchellAWstr) + "\n");
			tmpRenderConfig.append("film.filter.mitchell.b" + lxmUtils.ToNarrow(FilterMitchellBWstr) + "\n");
		}
		if (filterIndex == 4)
		{
			tmpRenderConfig.append("film.filter.mitchellss.b" + lxmUtils.ToNarrow(FilterMitchellAWstr) + "\n");
			tmpRenderConfig.append("film.filter.mitchellss.c" + lxmUtils.ToNarrow(FilterMitchellBWstr) + "\n");
		}
		
		
		
		int lightStrategyIndex = (int)_wtoi(LightStrategyIndexWstr);
		switch (lightStrategyIndex)
		{
			case 0:
				tmpRenderConfig.append("lightstrategy.type = UNIFORM\n");
				break;
			case 1:
				tmpRenderConfig.append("lightstrategy.type = POWER\n");
				break;
			case 2:
				tmpRenderConfig.append("lightstrategy.type = LOG_POWER\n");
				break;
		}

		int samplerindex = (int)_wtoi(SamplerIndexWstr);
		switch (samplerindex)
		{
			case 0:
			{
				tmpRenderConfig.append("sampler.type = METROPOLIS\n");
				tmpRenderConfig.append("sampler.metropolis.largesteprate = " + lxmUtils.ToNarrow(MetropolisLargestEpRateWstr) + "\n");
				tmpRenderConfig.append("sampler.metropolis.maxconsecutivereject = " + lxmUtils.ToNarrow(MetropolisMaxConsecutiveRejectWstr) + "\n");
				tmpRenderConfig.append("sampler.metropolis.imagemutationrate = " + lxmUtils.ToNarrow(MetrolpolisImageMutationRateWstr) + "\n");
				break;
			}
			case 1:
			{
				tmpRenderConfig.append("sampler.type = RANDOM\n");
				break;
			}
			case 2:
			{
				tmpRenderConfig.append("sampler.type = SOBOL\n");
				
				break;
			}	
			case 3:
			{
				tmpRenderConfig.append("sampler.type = TILEPATHSAMPLER\n");
				break;
			}
		}

		renderConfigProp.SetFromString(tmpRenderConfig);
		
		scene->RemoveUnusedImageMaps();
		scene->RemoveUnusedMaterials();
		scene->RemoveUnusedMeshes();
		scene->RemoveUnusedTextures();
		scene->SetDeleteMeshData(true);
		RenderConfig *config = RenderConfig::Create(renderConfigProp, scene);
		RenderSession *session = RenderSession::Create(config);
		session->Start();
		DoRendering(session, prog, tobm);
		session->Stop();
		delete scene;
		delete config;
		delete session;
	}
}

void LuxMax::Close(HWND hwnd, RendProgressCallback* prog) {
	UNREFERENCED_PARAMETER(prog);
	UNREFERENCED_PARAMETER(hwnd);
	if (file)
		delete file;
	file = NULL;
}

RefTargetHandle LuxMax::Clone(RemapDir &remap) {
	LuxMax *newRend = new LuxMax;
	newRend->FileName = FileName;
	newRend->halttimewstr = halttimewstr;
	newRend->LensRadiusstr = LensRadiusstr;
	newRend->FilterIndexWstr = FilterIndexWstr;
	newRend->FilterXWidthWst = FilterXWidthWst;
	newRend->FilterYWidthWst = FilterYWidthWst;
	newRend->RenderTypeWstr = RenderTypeWstr;
	newRend->FilterGuassianAlphaWstr = FilterGuassianAlphaWstr;
	newRend->FilterMitchellAWstr = FilterMitchellAWstr;
	newRend->FilterMitchellBWstr = FilterMitchellBWstr;
	newRend->MetrolpolisImageMutationRateWstr = MetrolpolisImageMutationRateWstr;
	newRend->MetropolisLargestEpRateWstr = MetropolisLargestEpRateWstr;
	newRend->MetropolisMaxConsecutiveRejectWstr = MetropolisMaxConsecutiveRejectWstr;
	newRend->SamplerIndexWstr = SamplerIndexWstr;
	newRend->enableFileSaverOutoutWstr = enableFileSaverOutoutWstr;
	BaseClone(this, newRend, remap);
	return newRend;
}

void LuxMax::ResetParams(){
	FileName.Resize(0);
}

#define FILENAME_CHUNKID 001
#define HALTTIME_CHUNKID 002
#define LENSRADIUS_CHUNKID 003
#define VFBREFRESHRATE_CHUNKID 004
#define FILTERINDEX_CHUNKID 005
#define FILTERXWIDTH_CHUNKID 006
#define FILTERYWIDTH_CHUNKID 007
#define RENDERTYPE_CHUNKID 8
#define FILTERGUASSIANALPHA_CHUNKID 011
#define FILTERMITCHELLA_CHUNKID 012
#define FILTERMITCHELLB_CHUNKID 013
#define LIGHTSTRATEGY_CHUNKID 014
#define METROPOLIS_IMAGE_MUTATION_RATE_CHUNKID 015
#define METROPOLIS_LARGEST_EP_RATE_CHUNKID 016
#define METROPOLIS_MAX_CONSECUTIVE_REJECT 017
#define SAMPLER_INDEX_CHUNKID 19
#define ENABLE_FILE_SAVER_OUTPUT_CHUNKID 20

IOResult LuxMax::Save(ISave *isave) {
	if (_tcslen(FileName) > 0) {
		isave->BeginChunk(FILENAME_CHUNKID);
		isave->WriteWString(FileName);
		isave->EndChunk();
	}

	isave->BeginChunk(HALTTIME_CHUNKID);
	isave->WriteWString(halttimewstr);
	isave->EndChunk();
	isave->BeginChunk(VFBREFRESHRATE_CHUNKID);
	isave->WriteWString(vbinterval);
	isave->EndChunk();
	isave->BeginChunk(LENSRADIUS_CHUNKID);
	isave->WriteWString(LensRadiusstr);
	isave->EndChunk();
	isave->BeginChunk(FILTERINDEX_CHUNKID);
	isave->WriteWString(FilterIndexWstr);
	isave->EndChunk();
	isave->BeginChunk(FILTERXWIDTH_CHUNKID);
	isave->WriteWString(FilterXWidthWst);
	isave->EndChunk();
	isave->BeginChunk(FILTERYWIDTH_CHUNKID);
	isave->WriteWString(FilterYWidthWst);
	isave->EndChunk();
	isave->BeginChunk(RENDERTYPE_CHUNKID);
	isave->WriteWString(RenderTypeWstr);
	isave->EndChunk();
	isave->BeginChunk(FILTERGUASSIANALPHA_CHUNKID);
	isave->WriteWString(FilterGuassianAlphaWstr);
	isave->EndChunk();
	isave->BeginChunk(FILTERMITCHELLA_CHUNKID);
	isave->WriteWString(FilterMitchellAWstr);
	isave->EndChunk();
	isave->BeginChunk(FILTERMITCHELLB_CHUNKID);
	isave->WriteWString(FilterMitchellBWstr);
	isave->EndChunk();
	isave->BeginChunk(LIGHTSTRATEGY_CHUNKID);
	isave->WriteWString(LightStrategyIndexWstr);
	isave->EndChunk();
	isave->BeginChunk(METROPOLIS_IMAGE_MUTATION_RATE_CHUNKID);
	isave->WriteWString(MetrolpolisImageMutationRateWstr);
	isave->EndChunk();
	isave->BeginChunk(METROPOLIS_LARGEST_EP_RATE_CHUNKID);
	isave->WriteWString(MetropolisLargestEpRateWstr);
	isave->EndChunk();
	isave->BeginChunk(METROPOLIS_MAX_CONSECUTIVE_REJECT);
	isave->WriteWString(MetropolisMaxConsecutiveRejectWstr);
	isave->EndChunk();
	isave->BeginChunk(SAMPLER_INDEX_CHUNKID);
	isave->WriteWString(SamplerIndexWstr);
	isave->EndChunk();
	isave->BeginChunk(ENABLE_FILE_SAVER_OUTPUT_CHUNKID);
	isave->WriteWString(enableFileSaverOutoutWstr);
	isave->EndChunk();
		
	return IO_OK;
}

IOResult LuxMax::Load(ILoad *iload) {
	int id;
	TCHAR *buf;
	IOResult res;
	while (IO_OK == (res = iload->OpenChunk())) {
		switch (id = iload->CurChunkID())  {
		case FILENAME_CHUNKID:
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FileName = buf;
			break;
		case HALTTIME_CHUNKID:
			if (IO_OK == iload->ReadWStringChunk(&buf))
				halttimewstr = buf;
			break;
		case VFBREFRESHRATE_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				vbinterval = buf;
			break;

		}
		case LENSRADIUS_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				LensRadiusstr = buf;
			break;
		}
		case FILTERINDEX_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FilterIndexWstr = buf;
				break;
		}
		case FILTERXWIDTH_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FilterXWidthWst = buf;
			break;
		}
		case FILTERYWIDTH_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FilterYWidthWst = buf;
			break;
		}
		case RENDERTYPE_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				RenderTypeWstr = buf;
			break;
		}

		case FILTERGUASSIANALPHA_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FilterGuassianAlphaWstr = buf;
			break;
		}

		case FILTERMITCHELLA_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FilterMitchellAWstr = buf;
			break;
		}

		case FILTERMITCHELLB_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				FilterMitchellBWstr = buf;
			break;
		}

		case LIGHTSTRATEGY_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				LightStrategyIndexWstr = buf;
			break;
		}

		case METROPOLIS_IMAGE_MUTATION_RATE_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				MetrolpolisImageMutationRateWstr = buf;
			break;
		}

		case METROPOLIS_LARGEST_EP_RATE_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				MetropolisLargestEpRateWstr = buf;
			break;
		}

		case METROPOLIS_MAX_CONSECUTIVE_REJECT:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				MetropolisMaxConsecutiveRejectWstr = buf;
			break;
		}

		case SAMPLER_INDEX_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				SamplerIndexWstr = buf;
			break;
		}
		case ENABLE_FILE_SAVER_OUTPUT_CHUNKID:
		{
			if (IO_OK == iload->ReadWStringChunk(&buf))
				enableFileSaverOutoutWstr = buf;
			break;
		}
	}
		iload->CloseChunk();
		if (res != IO_OK)
			return res;
	}
	return IO_OK;
}

//***************************************************************************
// Initialize our custom options.
//***************************************************************************

LuxRenderParams::LuxRenderParams()
{
	//SetDefaults();

	/*envMap = NULL;
	atmos = NULL;
	rendType = RENDTYPE_NORMAL;
	nMinx = 0;
	nMiny = 0;
	nMaxx = 0;
	nMaxy = 0;
	nNumDefLights = 0;
	nRegxmin = 0;
	nRegxmax = 0;
	nRegymin = 0;
	nRegymax = 0;
	//scrDUV = Point2(0.0f, 0.0f);
	//pDefaultLights = NULL;
	//pFrp = NULL;
	bVideoColorCheck = 0;
	bForce2Sided = FALSE;
	bRenderHidden = FALSE;
	bSuperBlack = FALSE;
	bRenderFields = FALSE;
	bNetRender = FALSE;

	renderer = NULL;
	projType = PROJ_PERSPECTIVE;
	devWidth = 0;
	devHeight = 0;
	xscale = 0;
	yscale = 0;
	xc = 0;
	yc = 0;
	antialias = FALSE;
	nearRange = 0;
	farRange = 0;
	devAspect = 0;
	frameDur = 0;
	time = 0;
	wireMode = FALSE;
	inMtlEdit = FALSE;
	fieldRender = FALSE;
	first_field = FALSE;
	field_order = FALSE;
	objMotBlur = FALSE;
	nBlurFrames = 0;*/
}

void LuxRenderParams::SetDefaults()
{
	nMaxDepth = 0;
	//nAntiAliasLevel = 0x00;
	//bReflectEnv = FALSE;
}
