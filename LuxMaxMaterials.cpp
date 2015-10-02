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

#include <algorithm>
using std::max;
using std::min;

#include "LuxMaxMaterials.h"
#include "LuxMaxUtils.h"
#include <bitmap.h>
#include <pbbitmap.h>
#include <map>
#include <IGame\IGameMaterial.h>
#include <stdio.h>
#include <string>
#include <maxapi.h>
#include "imtl.h"
#include "imaterial.h"
#include <imaterial.h>
#include <iparamb2.h>
#include <iparamb.h>
#include "path.h"
//#include "AssetType.h"
#include "IFileResolutionManager.h"
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <maxscript\maxscript.h>



//Import luxcore into separate namespace to avoid conflict with max SDK includes.
namespace luxcore
{
#include <luxcore/luxcore.h>
}

using namespace std;
using namespace luxcore;
using namespace luxrays;

#define STANDARDMATERIAL_CLASSID Class_ID(2,0)
#define ARCHITECTURAL_CLASSID Class_ID(332471230,1763586103)
#define ARCHDESIGN_CLASSID Class_ID(1890604853, 1242969684)
#define LUXCORE_CHEKER_CLASSID Class_ID(0x34e85fea, 0x855292c)
#define LR_INTERNAL_MATTE_CLASSID Class_ID(0x31b54e60, 0x1de956e4)
#define LR_INTERNAL_MATTELIGHT_CLASSID Class_ID(0x5d2f7ac1, 0x7dd93354)
#define LR_INTERNAL_MAT_TEMPLATE_CLASSID Class_ID(0x64691d17, 0x288d50d9)

LuxMaxUtils * lmutil;

LuxMaxMaterials::LuxMaxMaterials()
{
}


LuxMaxMaterials::~LuxMaxMaterials()
{
}

//std::string LuxMaxMaterials::getMaterialDiffuseTexturePath(::Mtl* mat)
//{

//}

Point3 LuxMaxMaterials::getMaterialDiffuseColor(::Mtl* mat)
{
	std::string objString;
	::Point3 diffcolor;
	Interval      ivalid;

	if (mat->ClassID() == LR_INTERNAL_MATTE_CLASSID)
	{
		IParamBlock2 *pBlock = mat->GetParamBlock(0);
		pBlock->GetValue(3, GetCOREInterface()->GetTime(), diffcolor, ivalid);
	}
	if (mat->ClassID() == LR_INTERNAL_MATTELIGHT_CLASSID)
	{
		IParamBlock2 *pBlock = mat->GetParamBlock(0);
		pBlock->GetValue(3, GetCOREInterface()->GetTime(), diffcolor, ivalid);
	}
	if (mat->ClassID() == LR_INTERNAL_MAT_TEMPLATE_CLASSID)
	{
		IParamBlock2 *pBlock = mat->GetParamBlock(0);
		pBlock->GetValue(3, GetCOREInterface()->GetTime(), diffcolor, ivalid);
	}
	if (mat->ClassID() == STANDARDMATERIAL_CLASSID || mat->ClassID() == ARCHITECTURAL_CLASSID)
	{
		diffcolor = mat->GetDiffuse(0);
	}

	return diffcolor;
}

//void LuxMaxMaterials::defineTexture(BitmapTex* bmTex, luxcore::Scene &scene, std::string textureName)
//{
//	std::string texString = "";
//	/*
//	Type: imagemap
//
//	Property name 						Property type 	Default value 	Description
//	scene.textures.<texture name>.type 	string 			"imagemap"   	type of texture
//	scene.textures.<texture name>.file 	string 			"image.png" 	name of the image file
//	scene.textures.<texture name>.gamma float			2.2				gamma correction value used in the image
//	scene.textures.<texture name>.gain 	float 			1.0 			image values are multiplied by gain value
//
//		Example data:
//		scene.textures.tex.type = imagemap
//		scene.textures.tex.file = scenes / bump / map.png
//		scene.textures.tex.gain = 0.6
//		scene.textures.tex.mapping.uvscale = 16 - 16
//	*/
//
//	// http://docs.autodesk.com/3DSMAX/16/ENU/3ds-Max-SDK-Programmer-Guide/files/GUID-50105419-69CD-4EF0-B96F-D59BBD46C743.htm
//
//	texString.append("scene.textures.");
//	texString.append(textureName + "\n");
//	
//
//	Texmap* tm = (BitmapTex*)bmTex;
//	BitmapTex *bmt = (BitmapTex*)tm;
//	BitmapInfo *btinfo = (BitmapInfo*)tm;
//	//std::string str = getstring(tm->GetFullName());
//	//mprintf(_T("\n Defining texture file named: %s \n", lmutil->getstring(btinfo->Filename())));
//	
//	IFileResolutionManager* pFRM = IFileResolutionManager::GetInstance();
//	MaxSDK::Util::Path path(btinfo->Filename());
//	if (pFRM->GetFullFilePath(path, MaxSDK::AssetManagement::kBitmapAsset))
//	{
//		// path now holds a fully qualified path
//		texString.append("scene.textures." + textureName + ".file = " + path.GetCStr);
//		mprintf(_T("\n Defining texture file path: %s \n", path.GetCStr));
//	}
//	else
//	{
//		// the bitmap file could not be found using 3ds Max asset
//		// resolution rules. The value of path is left unchanged
//	}
//
//	
//}

void LuxMaxMaterials::exportMaterial(Mtl* mat, luxcore::Scene &scene)
{
	const wchar_t *matName = L"";
	matName = mat->GetName();
	std::string tmpMatName = lmutil->ToNarrow(matName);
	lmutil->removeUnwatedChars(tmpMatName);
	std::wstring replacedMaterialName = std::wstring(tmpMatName.begin(), tmpMatName.end());
	matName = replacedMaterialName.c_str();
	
	//if (scene->IsMaterialDefined(ToNarrow(matName)) == false)
	//{
	std::string objString = "";
	objString.append("scene.materials.");
	objString.append(lmutil->ToNarrow(matName));
	std::string currmat = objString;

	objString.append(".type");

	if (mat->ClassID() == ARCHITECTURAL_CLASSID)
	{
		const wchar_t *matType = L"";
		luxrays::Properties prop;
		::Point3 colorDiffuse;
		float ioroutside, iorinside;

		colorDiffuse = getMaterialDiffuseColor(mat);

		for (int i = 0, count = mat->NumParamBlocks(); i < count; ++i)
		{
			IParamBlock2 *pBlock = mat->GetParamBlock(i);
			matType = pBlock->GetStr(0, GetCOREInterface()->GetTime(), 0);
			//2 is ior
			iorinside = pBlock->GetFloat(2, GetCOREInterface()->GetTime(), 0);
			ioroutside = iorinside;
		}
		OutputDebugStringW(matType);
		//((getstring(matType) == "Glass - Translucent") ||
		if ((lmutil->getstring(matType) == "Metal - Brushed"))
		{
			//metal2
			OutputDebugStringW(_T("\nCreating Metal2 material.\n"));

			std::string tmpmat;// = currmat;
			tmpmat.append(currmat + ".type = metal2");
			tmpmat.append("\n");
			prop.SetFromString(tmpmat);
			scene.Parse(prop);
		}

		//Glass - Clear
		else if ((lmutil->getstring(matType) == "Glass - Translucent") || (lmutil->getstring(matType) == "Glass - Clear"))
		{
			OutputDebugStringW(_T("\nCreating Glass.\n"));

			std::string tmpmat;
			tmpmat.append(currmat + ".type = glass");
			tmpmat.append("\n");
			
			tmpmat.append(currmat + ".ioroutside = " + std::to_string(ioroutside));
			tmpmat.append("\n");

			tmpmat.append(currmat + ".iorinside = " + std::to_string(1.0));
			tmpmat.append("\n");

			tmpmat.append(currmat + ".kr = " + std::to_string(colorDiffuse.x) + " " + std::to_string(colorDiffuse.y) + " " + std::to_string(colorDiffuse.z));
			tmpmat.append("\n");

			prop.SetFromString(tmpmat);
			scene.Parse(prop);
		}
		else if ((lmutil->getstring(matType) == "Mirror"))
		{
			//metal2
			OutputDebugStringW(_T("\nCreating Mirror material.\n"));
			scene.Parse(
				luxrays::Property(objString)("mirror") <<
				luxrays::Property("")("")
				);
		}
		else
		{
			OutputDebugStringW(_T("\nCreating fallback architectural material for unsupported template.\n"));
			scene.Parse(
				luxrays::Property(objString)("matte") <<
				luxrays::Property("")("")
				);
		}
	}
	else if (mat->ClassID() == LR_INTERNAL_MATTELIGHT_CLASSID)
	{
		mprintf(_T("\n Creating Emission material %i \n"));
		scene.Parse(
			luxrays::Property(objString)("matte") <<
			luxrays::Property("")("")
			);

		::Point3 diffcol;
		diffcol = getMaterialDiffuseColor(mat);
		::std::string tmpMatStr;

		tmpMatStr.append("scene.materials.");
		tmpMatStr.append(lmutil->ToNarrow(matName));
		tmpMatStr.append(".emission");

		scene.Parse(
			luxrays::Property(tmpMatStr)(float(diffcol.x), float(diffcol.y), float(diffcol.z)) <<
			luxrays::Property("")("")
			);
		tmpMatStr = "";
	}
	else if (mat->ClassID() == LUXCORE_CHEKER_CLASSID)
	{

		scene.Parse(
			luxrays::Property(objString)("matte") <<
			luxrays::Property("")("")
			);

		/*scene.textures.check.type = checkerboard2d
		scene.textures.check.texture1 = 0.7 0.0 0.0
		scene.textures.check.texture2 = 0.7 0.7 0.0
		scene.textures.check.mapping.uvscale = 16 -16*/
		mprintf(_T("\n Creating Cheker material %i \n"));
		scene.Parse(
			luxrays::Property("scene.textures.check.type")("checkerboard2d") <<
			luxrays::Property("scene.textures.check.texture1")("0.7 0.0 0.0") <<
			luxrays::Property("scene.textures.check.texture2")("0.7 0.7 0.0") <<
			luxrays::Property("scene.textures.check.mapping.uvscale")(16.0f, 16.0f)
			);
		mprintf(_T("\n Creating Cheker 01 %i \n"));
		/*scene->Parse(
		Property(tmpMatStr)("checkerboard2d") <<
		Property("")("")
		);

		::Point3 diffcol;
		diffcol = getMaterialDiffuseColor(mat);
		::std::string tmpMatStr;

		tmpMatStr.append("scene.textures.");
		tmpMatStr.append(ToNarrow(matName));
		tmpMatStr.append(".type");

		mprintf(_T("\n Creating Cheker material %i \n"));
		scene->Parse(
		Property(tmpMatStr)("checkerboard2d") <<
		Property("")("")
		);
		tmpMatStr = "";

		/*tmpMatStr.append("scene.textures.");
		tmpMatStr.append(ToNarrow(matName));
		tmpMatStr.append(".texture1");

		mprintf(_T("\n tx01 %i \n"));
		scene->Parse(
		Property(tmpMatStr)(float(diffcol.x)/256, float(diffcol.y)/256, float(diffcol.z)/256) <<
		Property("")("")
		);

		tmpMatStr = "";

		tmpMatStr.append("scene.textures.");
		tmpMatStr.append(ToNarrow(matName));
		tmpMatStr.append(".texture2");

		mprintf(_T("\n tx02 %i \n"));
		scene->Parse(
		Property(tmpMatStr)(float(1), float(1), float(1)) <<
		Property("")("")
		);

		tmpMatStr = "";

		tmpMatStr.append("scene.textures.");
		tmpMatStr.append(ToNarrow(matName));
		tmpMatStr.append(".mapping.uvscale");

		mprintf(_T("\n tx03 %i \n"));
		scene->Parse(
		Property(tmpMatStr)("16 - 16") <<
		Property("")("")
		);

		/*tmpMatStr.append("scene.materials.");
		tmpMatStr.append(ToNarrow(matName));
		tmpMatStr.append(".texture2");

		scene->Parse(
		Property(tmpMatStr)(float(diffcol.x) / 256, float(diffcol.y) / 256, float(diffcol.z) / 256) <<
		Property("")("")
		);*/
		//tmpMatStr = "";

	}
	else	//Parse as matte material.
	{
		OutputDebugStringW(_T("\nCreating fallback material.\n"));
		scene.Parse(
			luxrays::Property(objString)("matte") <<
			luxrays::Property("")("")
			);
		//objString = "";

		::Point3 diffcol;
		diffcol = getMaterialDiffuseColor(mat);
		::std::string tmpMatStr;
		tmpMatStr.append("scene.materials.");
		tmpMatStr.append(lmutil->ToNarrow(matName));
		tmpMatStr.append(".kd");
		//mprintf(L"Material kd string: %s\n", tmpMatStr.c_str());
		scene.Parse(
			luxrays::Property(tmpMatStr)(float(diffcol.x), float(diffcol.y), float(diffcol.z)) <<
			luxrays::Property("")("")
			);
		tmpMatStr = "";
	}

}

bool LuxMaxMaterials::isSupportedMaterial(::Mtl* mat)
{
	if (mat->ClassID() == LR_INTERNAL_MATTE_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == STANDARDMATERIAL_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == ARCHITECTURAL_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_INTERNAL_MATTELIGHT_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LUXCORE_CHEKER_CLASSID)
	{
		return true;
	}
	else
	{
		return false;
	}
}