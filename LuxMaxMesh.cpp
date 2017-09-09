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

#include <luxcore/luxcore.h>
#include <luxrays\utils\utils.h>
#include <luxrays\utils\properties.h>
#include <luxrays\utils\exportdefs.h>
#include <algorithm>
using std::max;
using std::min;
#include "LuxMaxMesh.h"
#include "max.h"
//#include "maxtypes.h"
//#include "interval.h"
#include "LuxMaxLights.h"
#include "LuxMaxUtils.h"
#include <stdio.h>
#include <string>
#include <maxapi.h>
#include "imtl.h"
#include "imaterial.h"
#include <imaterial.h>
#include <iparamb2.h>
#include <iparamb.h>
#include <maxscript\maxscript.h>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>

#include <maxscript\maxscript.h>
#include "LuxMaxMaterials.h"
#include "LuxMaxUtils.h"

////#include <luxrays\luxrays.h>

using namespace std;
using namespace luxcore;
using namespace luxrays;

LuxMaxMaterials *lxmMaterials;
LuxMaxUtils *lxmUtils;

LuxMaxMesh::LuxMaxMesh()
{
}

LuxMaxMesh::~LuxMaxMesh()
{
}

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

class BBox {
public:
	// BBox Public Methods

	BBox() {
		pMin = Point(numeric_limits<float>::infinity(),
			numeric_limits<float>::infinity(),
			numeric_limits<float>::infinity());
		pMax = Point(-numeric_limits<float>::infinity(),
			-numeric_limits<float>::infinity(),
			-numeric_limits<float>::infinity());
	}

	BBox(const Point &p1, const Point &p2) {
		pMin = p1;
		pMax = p2;
	}

	Point pMin, pMax;
};
static int hashPoint3(Point3& p) { return (*(int*)&p.x * 73856093) ^ (*(int*)&p.y * 19349663) ^ (*(int*)&p.z * 83492791); }

struct vertex : public MaxHeapOperators
{
	static const int hashTable[];

	Point3 p;
	Point3 n;
	Point3 uv;
	int	   mid;

	unsigned int index;
	int			 hash;

	vertex() : index(0), hash(0) {}

	bool operator==(const vertex& v) const { return hash == v.hash; }
	bool operator!=(const vertex& v) const { return hash != v.hash; }
	bool operator>(const vertex& v) const  { return hash > v.hash; }
	bool operator<(const vertex& v) const  { return hash < v.hash; }

	void hashit()
	{
		hash ^= hashPoint3(p) * hashTable[0];
		hash ^= hashPoint3(n) * hashTable[1];
		hash ^= hashPoint3(uv) * hashTable[2];
		hash ^= mid * hashTable[3];
		hash ^= index * hashTable[4];
	}
};

const int vertex::hashTable[] = { 93944371, 36311839, 82895123, 10033109, 59882063, 42133979, 24823181 };

typedef vertex* vertexPtr;

int CompareVertexFn(const void* i, const void* j)
{
	if (**(vertexPtr*)i < **(vertexPtr*)j)
		return -1;
	if (**(vertexPtr*)i > **(vertexPtr*)j)
		return 1;
	return 0;
}

void GetFaceRNormals(::Mesh& mesh, int fi, Point3* normals)
{
	Face& face = mesh.faces[fi];
	DWORD fsmg = face.getSmGroup();
	if (fsmg == 0)
	{
		normals[0] = normals[1] = normals[2] = mesh.getFaceNormal(fi);
		return;
	}
	MtlID fmtl = face.getMatID();
	DWORD* fverts = face.getAllVerts();
	for (int v = 0; v < 3; ++v)
	{
		RVertex& rvert = mesh.getRVert(fverts[v]);
		int numNormals = (int)(rvert.rFlags & NORCT_MASK);

		if (numNormals == 1)
			normals[v] = rvert.rn.getNormal();
		else
		{
			for (int n = 0; n < numNormals; ++n)
			{
				RNormal& rn = rvert.ern[n];
				if ((fsmg & rn.getSmGroup()) && fmtl == rn.getMtlIndex())
				{
					normals[v] = rn.getNormal();
					break;
				}
			}
		}
	}
}

vertexPtr CollectRawVerts(::Mesh& mesh, int rawcount)
{
	int numfaces = mesh.numFaces;
	vertexPtr rawverts = new vertex[rawcount];
	if (!rawverts) return NULL;

	mesh.checkNormals(TRUE);
	Face* faces = mesh.faces;
	Point3* verts = mesh.verts;
	TVFace* tvfaces = mesh.tvFace;
	Point3* uvverts = mesh.tVerts;

	for (int f = 0, i = 0; f < numfaces; ++f, ++faces, ++tvfaces)
	{
		Point3 fnormals[3];
		GetFaceRNormals(mesh, f, fnormals);
		short mid = faces->getMatID();
		Point2 tmpUv = Point2(0, 0);
		bool hasUvs = true;
		if (mesh.getNumTVerts() < 1)
		{
			hasUvs = false;
		}

		for (int v = 0; v < 3; ++v)
		{
			vertex& rv = rawverts[i++];
			rv.index = faces->v[v];
			rv.p = verts[faces->v[v]];
			rv.n = fnormals[v];
			if (hasUvs)
			{
				rv.uv = uvverts[tvfaces->t[v]];
			}
			else
			{
				rv.uv.x = 0;
				rv.uv.y = 0;
			}

			rv.mid = mid;
			rv.hashit();
		}
	}
	return rawverts;
}

void LuxMaxMesh::createMesh(INode * currNode, luxcore::Scene &scene , TimeValue t)
{
	//for (int a = 0; currNode->NumChildren() > a; a++)
	//{
	//	INode* childNode = currNode->GetChildNode(a);
	//	createMesh(childNode, scene);
	//	//bool doExport = true;
	//}

	Object*	obj;
	ObjectState os = currNode->EvalWorldState(t);
	obj = os.obj;
	Matrix3 nodeInitTM;
	Point4 nodeRotation;
	TriObject *p_triobj = NULL;

	BOOL fConvertedToTriObject = obj->CanConvertToType(triObjectClassID) && (p_triobj = (TriObject*)obj->ConvertToType(0, triObjectClassID)) != NULL;

	if (!fConvertedToTriObject || p_triobj->mesh.getNumFaces() < 1)
	{
		OutputDebugStringW(L"Debug: Did not triangulate object :");
		OutputDebugStringW(currNode->GetName());
		exit;
	}
	else
	{
		//mprintf(L"Info: Creating mesh for object : %s\n", currNode->GetName());
		const wchar_t *objName = L"";
		std::string tmpName = lxmUtils->ToNarrow(currNode->GetName());
		std::string nodeHandle = std::to_string(currNode->GetHandle());
		tmpName.append(nodeHandle);
		lxmUtils->removeUnwatedChars(tmpName);
		std::wstring replacedObjName = std::wstring(tmpName.begin(), tmpName.end());
		objName = replacedObjName.c_str();

		OutputDebugStringW(L"\nLuxMaxMesh -> Exporting Object: ");
		OutputDebugStringW(objName);

		::Mesh *p_trimesh = &p_triobj->mesh;

		p_trimesh->checkNormals(true);
		p_trimesh->buildNormals();

		int numUvs = p_trimesh->getNumTVerts();
		int vertCount = p_trimesh->numFaces * 3;
		
		vertexPtr rawverts = CollectRawVerts(*p_trimesh, vertCount);
		unsigned int* indices = new unsigned int[vertCount];
		for (int i = 0; i < vertCount; i++)
		{
			indices[i] = i;
		}

		unsigned int numTriangles = p_trimesh->getNumFaces();

		//Point *p = Scene::AllocTrianglesBuffer(vertCount);
		Point *p = (Point *)Scene::AllocVerticesBuffer(vertCount);
		//Triangle *vi = Scene::AllocTrianglesBuffer(numTriangles);
		Triangle *vi = (Triangle *)Scene::AllocTrianglesBuffer(numTriangles);
		//Normal *n = new Normal[vertCount];
		
		float *n = new float[vertCount * 3];
		//int normalCount = rawverts->n.siz * 3;
		//float *n = new float[normalCount];

		UV *uv = new UV[vertCount];

		for (int vert = 0; vert < vertCount; vert++)
		{
			p[vert] = Point(rawverts[vert].p);
		}

		int ncounter = 0;
		for (int norm = 0; norm < vertCount; norm++)
		{
			n[ncounter] = rawverts[norm].n.x;
			n[ncounter + 1] = rawverts[norm].n.y;
			n[ncounter + 2] = rawverts[norm].n.z;
			ncounter += 3;
		}

		for (int i = 0, fi = 0; fi < numTriangles; i += 3, ++fi)
		{
			vi[fi] = Triangle(
				int(indices[i]),
				int(indices[i + 1]),
				int(indices[i + 2]));
		}

		if (numUvs > 0)
		{
			for (int u = 0; u < vertCount; u++)
			{
				uv[u].u = rawverts[u].uv.x;
				uv[u].v = rawverts[u].uv.y * -1;
			}
		}

		if (numUvs > 1)
		{
			scene.DefineMesh(lxmUtils->ToNarrow(objName), vertCount, numTriangles, (float *)p, (unsigned int *)vi, (float*)n, NULL, NULL, NULL);
		}
		else
		{
			scene.DefineMesh(lxmUtils->ToNarrow(objName), vertCount, numTriangles, (float *)p, (unsigned int *)vi, (float*)n,(float*)uv, NULL, NULL);
		}

		delete[] rawverts;
		delete[] indices;
		//delete[] n;
		p = NULL;
		vi = NULL;
		//n = NULL;
		uv = NULL;

		Properties props;
		std::string objString;

		objString = "scene.objects.";
		objString.append(lxmUtils->ToNarrow(objName));
		objString.append(".ply = ");
		objString.append(lxmUtils->ToNarrow(objName));
		objString.append("\n");
		props.SetFromString(objString);
		objString = "";

		Mtl *objmat = NULL;

		if (currNode->GetMtl() == NULL)
		{
			//TODO: Call a function here that causes it to create a 'fallback material
			//inside the material lib..
			OutputDebugStringW(L"\nLuxMaxMesh -> Creating fallback material for object: ");
			OutputDebugStringW(objName);

			objString.append("scene.materials.undefined");
			objString.append(".type");

			scene.Parse(
				Property(objString)("matte") <<
				Property("")("")
				);
			objString = "";

			::std::string tmpMatStr;
			tmpMatStr.append("scene.materials.undefined.kd");
			//mprintf(L"Creating fallback material for undefined material.\n");
			scene.Parse(
				Property(tmpMatStr)(float(0.5), float(0.5), float(0.5)) <<
				Property("")("")
				);
			tmpMatStr = "";

			objString = "";
			objString.append("scene.objects.");
			objString.append(lxmUtils->ToNarrow(objName));
			objString.append(".material = ");
			objString.append("undefined");
			objString.append("\n");
			props.SetFromString(objString);
			scene.Parse(props);
			objString = "";
		}
		else
		{
			const wchar_t *matName = L"";
			matName = currNode->GetMtl()->GetName();
			std::string tmpMatName = lxmUtils->ToNarrow(matName);
			lxmUtils->removeUnwatedChars(tmpMatName);
			std::wstring replacedMaterialName = std::wstring(tmpMatName.begin(), tmpMatName.end());
			matName = replacedMaterialName.c_str();
			
			if (tmpMatName == "")
			{
				matName = L"fallbackmaterial";
			}

			

			objmat = currNode->GetMtl();
			int numsubs = 0;
			numsubs = objmat->NumSubMtls();
			if (numsubs < 1)
			{
				numsubs = 1;
			}
			for (int f = 0; f < numsubs; ++f)
			{
				if (lxmMaterials->isSupportedMaterial(objmat))
				{
					OutputDebugStringW(L"\nLuxMaxMesh -> Creating supported material for object: ");
					OutputDebugStringW(objName);
					OutputDebugStringW(L"\nLuxMaxMesh -> Material name:");
					OutputDebugStringW(matName);

					lxmMaterials->exportMaterial(objmat, scene);
				}
				else
				{
					OutputDebugStringW(L"\nLuxMaxMesh -> Creating fallback material for object: ");
					OutputDebugStringW(objName);
					OutputDebugStringW(L"\nMaterial name:");
					OutputDebugStringW(matName);

					matName = L"fallbackmaterial";
					objString.append("scene.materials.");
					//objString.append(lxmUtils->ToNarrow(matName));
					objString.append("fallbackmaterial");
					objString.append(".type");

					scene.Parse(
						Property(objString)("matte") <<
						Property("")("")
						);
					objString = "";

					::std::string tmpMatStr;
					tmpMatStr.append("scene.materials.");
					//tmpMatStr.append(lxmUtils->ToNarrow(matName));
					tmpMatStr.append("fallbackmaterial");
					tmpMatStr.append(".kd");
					//mprintf(L"Creating fallback material for unsupported material: %s\n", matName);
					scene.Parse(
						Property(tmpMatStr)(float(0.5), float(0.5), float(0.5)) <<
						Property("")("")
						);
					tmpMatStr = "";
				}
			}

			objString = "";
			objString.append("scene.objects.");
			objString.append(lxmUtils->ToNarrow(objName));
			objString.append(".material = ");
			objString.append(lxmUtils->ToNarrow(matName));
			objString.append("\n");
			props.SetFromString(objString);
			scene.Parse(props);
			objString = "";
		}

		//Set the transformation matrix for the current mesh object.
		//the getMaxNodeTransform function returns the numbers for the matrix.
		//that is why we append it here.
		objString.append("scene.objects.");
		objString.append(lxmUtils->ToNarrow(objName));
		objString.append(".transformation = ");
		objString.append(lxmUtils->getMaxNodeTransform(currNode,t));
		props.SetFromString(objString);
		scene.Parse(props);
	}
}

void LuxMaxMesh::createMeshesInGroup(INode *currNode, luxcore::Scene &scene, TimeValue t)
{
	for (size_t i = 0; i < currNode->NumberOfChildren(); i++)
	{
		INode *groupChild;
		groupChild = currNode->GetChildNode(i);
		// If the child is a grouphead it means it's a group inside a group
		// so we send the node back in and loop further down into the groups.
		if (groupChild->IsGroupHead())
		{
			createMeshesInGroup(groupChild, scene,t);
		}
		else
		{
			createMesh(groupChild, scene,t);
		}
	}
}