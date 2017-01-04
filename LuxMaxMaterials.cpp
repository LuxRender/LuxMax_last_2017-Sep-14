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
#include <string>
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
#include <bitmap.h>
#include "AssetManagement/iassetmanager.h"
#include "IFileResolutionManager.h"
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
#define STANDARDBITMAP_CLASSID Class_ID(576, 0)
#define LR_MATTE_TRANSLUCENT_CLASSID Class_ID(0x31b26e70, 0x2de454e4)
#define LR_ROUGH_MATTE_CLASSID Class_ID(0x34b56e70, 0x7de894e5)
#define LR_GLOSSY2_CLASSID Class_ID(0x67b86e70, 0x7de456e1)
#define LR_ROUGHGLASS_CLASSID Class_ID(0x56b76e70, 0x8de321e5)
#define LR_VELVET_CLASSID Class_ID(0x59b79e53, 0x2de567e3)

LuxMaxUtils * lmutil;

LuxMaxMaterials::LuxMaxMaterials()
{
}

LuxMaxMaterials::~LuxMaxMaterials()
{
}

std::string LuxMaxMaterials::getFloatFromParamBlockID(int paramID, ::Mtl* mat)
{
	std::string stringValue = "";
	float value = 0.0f;
	IParamBlock2 *pBlock = mat->GetParamBlock(0);

	if (pBlock != NULL)
	{
		value = pBlock->GetFloat(paramID, GetCOREInterface()->GetTime());
		stringValue = lmutil->floatToString(value);
	}
	return stringValue;
}

std::string LuxMaxMaterials::getIntFromParamBlockID(int paramID, ::Mtl* mat)
{
	std::string stringValue = "";
	float value = 0.0f;
	IParamBlock2 *pBlock = mat->GetParamBlock(0);

	if (pBlock != NULL)
	{
		value = pBlock->GetInt(paramID, GetCOREInterface()->GetTime());
		stringValue = std::to_string(value);
	}
	return stringValue;
}

std::string LuxMaxMaterials::getTexturePathFromParamBlockID(int paramID, ::Mtl* mat)
{
	Texmap *tex;
	std::string path = "";

	IParamBlock2 *pBlock = mat->GetParamBlock(0);
	tex = pBlock->GetTexmap(paramID, GetCOREInterface()->GetTime(), 0);
	
	if (tex != NULL)
	{
		if (tex->ClassID() == STANDARDBITMAP_CLASSID)
		{
			BitmapTex *bmt = (BitmapTex*)tex;

			if (bmt != NULL)
			{
				//Non-Unicode string, we should fix this so that it does not crash with Chinese characters for example.
				// http://www.luxrender.net/mantis/view.php?id=1624#bugnotes

				path = bmt->GetMap().GetFullFilePath().ToUTF8();
			}
		}
		else
		{
			OutputDebugStringW(L"\nUnsupported texture map in material: ");//mprintf(L"ERROR : Unsupported texture in material: '%s' , named: '%s' , will not render texture. standard bitmap is supported.\n", mat->GetName(), tex->GetName());
			OutputDebugStringW(mat->GetName());
		}
	}
	return path;
}

std::string LuxMaxMaterials::getBumpTextureName(::Mtl* mat)
{
	if ((mat->ClassID() == LR_INTERNAL_MATTE_CLASSID))
	{
		std::string tmpBumpTexName = getTextureName(6, mat);
		lmutil->removeUnwatedChars(tmpBumpTexName);

		return tmpBumpTexName;
	}
	else
	{
		return "";
	}
}

std::string LuxMaxMaterials::getDiffuseTextureName(::Mtl* mat)
{
	if ((mat->ClassID() == LR_INTERNAL_MATTE_CLASSID) || (mat->ClassID() == LR_INTERNAL_MAT_TEMPLATE_CLASSID))
	{
		std::string tmpDiffTexName = getTextureName(4, mat);
		lmutil->removeUnwatedChars(tmpDiffTexName);

		return tmpDiffTexName;
	}
	else
	{
		return "";
	}
}

std::string LuxMaxMaterials::getTextureName(int paramID, ::Mtl* mat)
{
	Texmap *tex;
	Interval      ivalid;
	IParamBlock2 *pBlock = mat->GetParamBlock(0);
	tex = pBlock->GetTexmap(paramID, GetCOREInterface()->GetTime(), 0);
	if (tex != NULL)
	{
		BitmapTex *bmt = (BitmapTex*)tex;
		BitmapInfo bi(bmt->GetMapName());

		if (tex->GetName() != NULL)
		{
			std::string tmpTexName = tex->GetName().ToCStr();
			lmutil->removeUnwatedChars(tmpTexName);
			return tmpTexName;
		}
		else
		{
			return "";
		}
	}
}

std::string LuxMaxMaterials::getMaterialBumpTexturePath(::Mtl* mat)
{
	if ((mat->ClassID() == LR_INTERNAL_MATTE_CLASSID))
	{
		// 4 is diffuse, 6 is bumpmap
		return getTexturePathFromParamBlockID(6, mat);
	}
}

std::string LuxMaxMaterials::getMaterialDiffuseTexturePath(::Mtl* mat)
{
	if (mat->ClassID() == LR_INTERNAL_MAT_TEMPLATE_CLASSID)
	{
		return getTexturePathFromParamBlockID(4, mat);
	}
	if ((mat->ClassID() == LR_INTERNAL_MATTE_CLASSID))
	{
		// 4 is diffuse, 6 is bumpmap
		return getTexturePathFromParamBlockID(4, mat);
	}
	if ((mat->ClassID() == LR_MATTE_TRANSLUCENT_CLASSID))
	{
		// 4 is diffuse, 6 is bumpmap
		return getTexturePathFromParamBlockID(4, mat);
	}
}

Point3 LuxMaxMaterials::getMaterialColor(int pblockIndex ,::Mtl* mat)
{
	std::string objString;
	::Point3 diffcolor;
	Interval      ivalid;
	IParamBlock2 *pBlock = mat->GetParamBlock(0);
	pBlock->GetValue(pblockIndex, GetCOREInterface()->GetTime(), diffcolor, ivalid);

	return diffcolor;
}

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
	if (mat->ClassID() == LR_MATTE_TRANSLUCENT_CLASSID)
	{
		diffcolor = mat->GetDiffuse(0);
	}
		 
	return diffcolor;
}

void LuxMaxMaterials::exportMaterial(Mtl* mat, luxcore::Scene &scene)
{
	const wchar_t *matName = L"";
	
	matName = mat->GetName();


	std::string tmpMatName = lmutil->ToNarrow(matName);
	if (tmpMatName == "")
	{
		tmpMatName = "undefinedMaterial";
		matName = L"undefinedMaterial";
	}

	lmutil->removeUnwatedChars(tmpMatName);
	std::wstring replacedMaterialName = std::wstring(tmpMatName.begin(), tmpMatName.end());
	matName = replacedMaterialName.c_str();

	OutputDebugStringW(L"\nLuxMaxMaterials.cpp -> Exporting material: ");
	OutputDebugStringW(matName);
	OutputDebugStringW(L"\n");

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
		float ioroutside = 0.0f;
		float iorinside = 0.0f;

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
		OutputDebugStringW(L"\n Creating Emission material\n");
		scene.Parse(
			luxrays::Property(objString)("matte") <<
			luxrays::Property("")("")
			);

	//	Check why it crashes, it might not be the light material, it could be something else.


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

		//mprintf(_T("\n Creating Cheker material %i \n"));
		scene.Parse(
			luxrays::Property("scene.textures.check.type")("checkerboard2d") <<
			luxrays::Property("scene.textures.check.texture1")("0.7 0.0 0.0") <<
			luxrays::Property("scene.textures.check.texture2")("0.7 0.7 0.0") <<
			luxrays::Property("scene.textures.check.mapping.uvscale")(16.0f, 16.0f)
			);
		//mprintf(_T("\n Creating Cheker 01 %i \n"));
	}
	else if (mat->ClassID() == LR_INTERNAL_MAT_TEMPLATE_CLASSID || (mat->ClassID() == LR_INTERNAL_MATTE_CLASSID))
	{
		luxrays::Properties prop;

		scene.Parse(
			luxrays::Property(objString)("matte") <<
			luxrays::Property("")("")
			);

		std::string bumpMapName = "";
		std::string bumpMapPath = "";
		std::string diffuseMapName = "";
		std::string tmpTexString;

		//Check if there is a bumpmap texture assigned.
		if (getMaterialBumpTexturePath(mat) != "")
		{
			std::string bumpTexName = getBumpTextureName(mat);
			if (bumpTexName != "")
			{
				luxrays::Properties prop;
				std::string bumpString;
				bumpString.append("scene.textures." + bumpTexName + ".type = imagemap");
				bumpString.append("\n");
				bumpString.append("scene.textures." + bumpTexName + ".file = " + "\"" + getMaterialBumpTexturePath(mat) + "\"");
				bumpString.append("\n");
				bumpMapPath = getMaterialBumpTexturePath(mat);
				bumpMapName = bumpTexName;

				prop.SetFromString(bumpString);
				scene.Parse(prop);
			}
		}

		//Check if there is a diffuse material assigned.
		if (getMaterialDiffuseTexturePath(mat) == "")
		{
			::Point3 diffcol;
			diffcol = getMaterialDiffuseColor(mat);
			::std::string tmpMatStr;
			tmpMatStr.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd");

			scene.Parse(
				luxrays::Property(tmpMatStr)(float(diffcol.x), float(diffcol.y), float(diffcol.z)) <<
				luxrays::Property("")("")
				);
			tmpMatStr = "";
		}
		else
		{
			diffuseMapName = getDiffuseTextureName(mat);
			tmpTexString.append("scene.textures." + diffuseMapName + ".type = imagemap");
			tmpTexString.append("\n");
			tmpTexString.append("scene.textures." + diffuseMapName + ".file = " + "\"" + getMaterialDiffuseTexturePath(mat) + "\"");
			tmpTexString.append("\n");
			tmpTexString.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + diffuseMapName);
			tmpTexString.append("\n");
		}

		if (bumpMapName != "")
		{
			tmpTexString.append("scene.materials." + lmutil->ToNarrow(matName) + ".bumptex = " + bumpMapName);
			tmpTexString.append("\n");
			tmpTexString.append("scene.materials." + lmutil->ToNarrow(matName) + ".bumpsamplingdistance = 1.0");
			tmpTexString.append("\n");
		}

		prop.SetFromString(tmpTexString);
		scene.Parse(prop);
	}
	else if (mat->ClassID() == LR_MATTE_TRANSLUCENT_CLASSID)
	{
		OutputDebugStringW(_T("\nCreating matte translucent material.\n"));
		luxrays::Properties prop;
		::Point3 diffcol;
		diffcol = getMaterialDiffuseColor(mat);

		std::string tmpmat;
		tmpmat.append(currmat + ".type = mattetranslucent");
		tmpmat.append("\n");

		if (getTexturePathFromParamBlockID(4,mat) == "")
		{
			tmpmat.append(currmat + ".kr = " + std::to_string(diffcol.x) + " " + std::to_string(diffcol.y) + " " + std::to_string(diffcol.z));
			tmpmat.append("\n");
		}else
		{
			std::string krTexName = getTextureName(4, mat);
			tmpmat.append("scene.textures." + krTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + krTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(4,mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kr = " + krTexName);
			tmpmat.append("\n");
		}
			
		if (getTexturePathFromParamBlockID(8, mat) == "")
		{
			diffcol = getMaterialColor(10, mat);
			tmpmat.append(currmat + ".kt = " + std::to_string(diffcol.x) + " " + std::to_string(diffcol.y) + " " + std::to_string(diffcol.z));
			tmpmat.append("\n");
		
		}else
		{
			std::string kdTexName = getTextureName(8, mat);
			tmpmat.append("scene.textures." + kdTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + kdTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(8,mat)+ "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + kdTexName);
			tmpmat.append("\n");
		}

		prop.SetFromString(tmpmat);
		scene.Parse(prop);
	}
	else if (mat->ClassID() == LR_ROUGH_MATTE_CLASSID)
	{
		OutputDebugStringW(_T("\nCreating rough matte material.\n"));
		luxrays::Properties prop;
		::Point3 diffcol;
		diffcol = getMaterialColor(3, mat);

		std::string tmpmat;
		tmpmat.append(currmat + ".type = roughmatte");
		tmpmat.append("\n");

		if (getTexturePathFromParamBlockID(4, mat) == "")
		{
			tmpmat.append(currmat + ".kd = " + std::to_string(diffcol.x) + " " + std::to_string(diffcol.y) + " " + std::to_string(diffcol.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string krTexName = getTextureName(4, mat);
			tmpmat.append("scene.textures." + krTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + krTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(4, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + krTexName);
			tmpmat.append("\n");
		}
		if (getTexturePathFromParamBlockID(8, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".sigma = " + getFloatFromParamBlockID(11, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string krTexName = getTextureName(8, mat);
			tmpmat.append("scene.textures." + krTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + krTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(8, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".sigma = " + krTexName);
			tmpmat.append("\n");
		}

		//Also get 'sigma'... a float, either a texture or a 0-360 value.
		prop.SetFromString(tmpmat);
		scene.Parse(prop);

	}
	else if (mat->ClassID() == LR_GLOSSY2_CLASSID)
	{
		OutputDebugStringW(_T("\nCreating Glossy2 material.\n"));
		luxrays::Properties prop;

		bool indexOverridesKs = false;

		std::string tmpmat;
		tmpmat.append(currmat + ".type = glossy2");
		tmpmat.append("\n");

		if (getTexturePathFromParamBlockID(0, mat) == "")
		{
			::Point3 kd;
			kd = getMaterialColor(1, mat);
			tmpmat.append(currmat + ".kd = " + std::to_string(kd.x) + " " + std::to_string(kd.y) + " " + std::to_string(kd.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string krTexName = getTextureName(0, mat);
			tmpmat.append("scene.textures." + krTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + krTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(0, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + krTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(2, mat) == "")
		{
			::Point3 ks;
			ks = getMaterialColor(3, mat);
			tmpmat.append(currmat + ".ks = " + std::to_string(ks.x) + " " + std::to_string(ks.y) + " " + std::to_string(ks.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string krTexName = getTextureName(2, mat);
			tmpmat.append("scene.textures." + krTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + krTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(2, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ks = " + krTexName);
			tmpmat.append("\n");
		}
		

		if (getTexturePathFromParamBlockID(4, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".uroughness = " + getFloatFromParamBlockID(5, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string uroughnessTexName = getTextureName(4, mat);
			tmpmat.append("scene.textures." + uroughnessTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + uroughnessTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(4, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".uroughness = " + uroughnessTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(6, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness = " + getFloatFromParamBlockID(7, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string vroughnessTexName = getTextureName(6, mat);
			tmpmat.append("scene.textures." + vroughnessTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + vroughnessTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(6, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness = " + vroughnessTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(8, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ka = " + getFloatFromParamBlockID(9, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string kaTexName = getTextureName(8, mat);
			tmpmat.append("scene.textures." + kaTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + kaTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(8, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ka = " + kaTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(10, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d = " + getFloatFromParamBlockID(11, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string dTexName = getTextureName(10, mat);
			tmpmat.append("scene.textures." + dTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + dTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(10, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d = " + dTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(12, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".index = " + getFloatFromParamBlockID(13, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string indexTexName = getTextureName(12, mat);
			tmpmat.append("scene.textures." + indexTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + indexTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(12, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".index = " + indexTexName);
			tmpmat.append("\n");
		}
		
		std::string temp = getIntFromParamBlockID(14, mat);
		//TODO: We should make a 'rounded' int (0) from the getintFromParamBlockID function.
		//Then return it as bool value.
		if (getIntFromParamBlockID( 14,mat) == "0.000000")
		{
			std::string temp = getIntFromParamBlockID(14, mat);

			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce = 0");
			tmpmat.append("\n");
		}
		else
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce = 1");
			tmpmat.append("\n");
		}
		

		prop.SetFromString(tmpmat);
		scene.Parse(prop);
	}
	else if (mat->ClassID() == LR_ROUGHGLASS_CLASSID)
	{
		OutputDebugStringW(_T("\nCreating RoughGlass material.\n"));
		luxrays::Properties prop;
		

		std::string tmpmat;
		tmpmat.append(currmat + ".type = roughglass");
		tmpmat.append("\n");

		if (getTexturePathFromParamBlockID(1, mat) == "")
		{
			::Point3 kr;
			kr = getMaterialColor(0, mat);
			tmpmat.append(currmat + ".kr = " + std::to_string(kr.x) + " " + std::to_string(kr.y) + " " + std::to_string(kr.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string kr_map = getTextureName(1, mat);
			tmpmat.append("scene.textures." + kr_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + kr_map + ".file = " + "\"" + getTexturePathFromParamBlockID(1, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kr = " + kr_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(3, mat) == "")
		{
			::Point3 kt;
			kt = getMaterialColor(2, mat);
			tmpmat.append(currmat + ".kt = " + std::to_string(kt.x) + " " + std::to_string(kt.y) + " " + std::to_string(kt.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string kt_map = getTextureName(3, mat);
			tmpmat.append("scene.textures." + kt_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + kt_map + ".file = " + "\"" + getTexturePathFromParamBlockID(3, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kt = " + kt_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(5, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".interiorior = " + getFloatFromParamBlockID(4, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string interiorior_map = getTextureName(5, mat);
			tmpmat.append("scene.textures." + interiorior_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + interiorior_map + ".file = " + "\"" + getTexturePathFromParamBlockID(5, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".interiorior = " + interiorior_map);
			tmpmat.append("\n");
		}


		if (getTexturePathFromParamBlockID(7, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".exteriorior = " + getFloatFromParamBlockID(6, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string exteriorior_map = getTextureName(7, mat);
			tmpmat.append("scene.textures." + exteriorior_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + exteriorior_map + ".file = " + "\"" + getTexturePathFromParamBlockID(7, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".exteriorior = " + exteriorior_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(9, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".uroughness = " + getFloatFromParamBlockID(8, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string uroughness_map = getTextureName(9, mat);
			tmpmat.append("scene.textures." + uroughness_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + uroughness_map + ".file = " + "\"" + getTexturePathFromParamBlockID(9, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".uroughness = " + uroughness_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(11, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness = " + getFloatFromParamBlockID(8, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string vroughness_map = getTextureName(11, mat);
			tmpmat.append("scene.textures." + vroughness_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + vroughness_map + ".file = " + "\"" + getTexturePathFromParamBlockID(11, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness = " + vroughness_map);
			tmpmat.append("\n");
		}

		prop.SetFromString(tmpmat);
		scene.Parse(prop);


	}
	else if (mat->ClassID() == LR_VELVET_CLASSID)
	{
		OutputDebugStringW(_T("\nCreating Velvet material.\n"));
		luxrays::Properties prop;


		std::string tmpmat;
		tmpmat.append(currmat + ".type = velvet");
		tmpmat.append("\n");
			
		if (getTexturePathFromParamBlockID(1, mat) == "")
		{
			::Point3 kd;
			kd = getMaterialColor(0, mat);
			tmpmat.append(currmat + ".kd = " + std::to_string(kd.x) + " " + std::to_string(kd.y) + " " + std::to_string(kd.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string kd_map = getTextureName(1, mat);
			tmpmat.append("scene.textures." + kd_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + kd_map + ".file = " + "\"" + getTexturePathFromParamBlockID(1, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + kd_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(3, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".p1 = " + getFloatFromParamBlockID(2, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string p1_map = getTextureName(3, mat);
			tmpmat.append("scene.textures." + p1_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + p1_map + ".file = " + "\"" + getTexturePathFromParamBlockID(3, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".p1 = " + p1_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(5, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".p1 = " + getFloatFromParamBlockID(4, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string p2_map = getTextureName(5, mat);
			tmpmat.append("scene.textures." + p2_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + p2_map + ".file = " + "\"" + getTexturePathFromParamBlockID(5, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".p2 = " + p2_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(7, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".p3 = " + getFloatFromParamBlockID(6, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string p3_map = getTextureName(7, mat);
			tmpmat.append("scene.textures." + p3_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + p3_map + ".file = " + "\"" + getTexturePathFromParamBlockID(7, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".p3 = " + p3_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(9, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".thickness = " + getFloatFromParamBlockID(8, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string thickness_map = getTextureName(9, mat);
			tmpmat.append("scene.textures." + thickness_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + thickness_map + ".file = " + "\"" + getTexturePathFromParamBlockID(9, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".thickness = " + thickness_map);
			tmpmat.append("\n");
		}
		prop.SetFromString(tmpmat);
		scene.Parse(prop);
	}
	else	//Parse as matte material.
	{
		OutputDebugStringW(_T("\nCreating fallback material.\n"));
		scene.Parse(
			luxrays::Property(objString)("matte") <<
			luxrays::Property("")("")
			);

		::Point3 diffcol;
		diffcol = getMaterialDiffuseColor(mat);
		::std::string tmpMatStr;
		tmpMatStr.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd");

		scene.Parse(	
			luxrays::Property(tmpMatStr)(float(diffcol.x), float(diffcol.y), float(diffcol.z)) <<
			luxrays::Property("")("")
			);
		tmpMatStr = "";
	}
}

bool LuxMaxMaterials::isSupportedMaterial(::Mtl* mat)
{
	if (mat->GetName() == L"")
	{
		OutputDebugStringW(L"LuxMaxMaterials.cpp -> IsSupportedMaterial: False - Material has no name.");
		return false;
	}
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
	else if (mat->ClassID() == LR_INTERNAL_MAT_TEMPLATE_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_MATTE_TRANSLUCENT_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_ROUGH_MATTE_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_GLOSSY2_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_ROUGHGLASS_CLASSID)
	{
		return true;
	}
	else if(mat->ClassID() == LR_VELVET_CLASSID)
	{
		return true;
	}
	else
	{
		return false;
	}
}