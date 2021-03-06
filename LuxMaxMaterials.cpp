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
#define LR_GLOSSYTRANSLUCENT_CLASSID Class_ID(0x24b19e11, 0x1de467e3)
#define LR_Archglass_CLASS_ID	Class_ID(0x34b16e78, 0x3de467e3)
#define LR_Cloth_CLASS_ID	Class_ID(0x45b18e28, 0x2de456e3)
#define LR_Carpaint_CLASS_ID	Class_ID(0x12b48e28, 0x5de432e3)
#define LR_Texture_CLASS_ID Class_ID(0x482c6db2, 0x9cb39d3d)
LuxMaxUtils * lmutil;

LuxMaxMaterials::LuxMaxMaterials()
{
}

LuxMaxMaterials::~LuxMaxMaterials()
{
}

std::string LuxMaxMaterials::getFloatFromParamBlockID(int paramID, ::Mtl* mat, ::Texmap* tex)
{
	std::string stringValue = "";
	float value = 0.0f;
	IParamBlock2 *pBlock = NULL;

	if (mat != NULL)
	{
		pBlock = mat->GetParamBlock(0);
	}
	else
	{
		pBlock = tex->GetParamBlock(0);
	}

	if (pBlock != NULL)
	{
		value = pBlock->GetFloat(paramID, GetCOREInterface()->GetTime());
		stringValue = lmutil->floatToString(value);
	}
	return stringValue;
}

std::string LuxMaxMaterials::exportTexturesInMaterial(::Mtl* mat, std::string texSlotName)
{
	std::string stringValue = "";

	Texmap *tex;
	std::string path = "";

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


	IParamBlock2 *pBlock = mat->GetParamBlock(0);
	for (int i = 0, count = mat->NumParamBlocks(); i < count; ++i)
	{
		IParamBlock2 *pBlock = mat->GetParamBlock(i);
		//for (int currParam = 0, currParam = pBlock->NumParams(); currParam < currParam; ++currParam)
		for (int a = 0; a < pBlock->NumParams(); a = a + 1) 
		{
			{
				tex = pBlock->GetTexmap(a, GetCOREInterface()->GetTime());

				if (tex != NULL)
				{
					if (tex->ClassID() == STANDARDBITMAP_CLASSID)
					{
						BitmapTex *bmt = (BitmapTex*)tex;

						if (bmt != NULL)
						{
							//Non-Unicode string, we should fix this so that it does not crash with Chinese characters for example.
							// http://www.luxrender.net/mantis/view.php?id=1624#bugnotes
							std::string tmpTexName = tex->GetName().ToCStr();
							lmutil->removeUnwatedChars(tmpTexName);
							//path = bmt->GetMap().GetFullFilePath().ToUTF8();
							stringValue.append("scene.textures." + tmpTexName + ".type = imagemap");
							stringValue.append("\n");
							stringValue.append("scene.textures." + tmpTexName + ".file = " + "\"" + getMaterialDiffuseTexturePath(mat) + "\"");
							stringValue.append("\n");
							stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + tmpTexName);
							stringValue.append("\n");
						}
					}
					else if (tex->ClassID() == LR_Texture_CLASS_ID)
					{
						std::string tmpTexName = tex->GetName().ToCStr();
						lmutil->removeUnwatedChars(tmpTexName);
						
						stringValue.append("scene.textures." + tmpTexName + ".type = blender_noise");
						stringValue.append("\n");
						stringValue.append("scene.textures." + tmpTexName + ".noisedepth =" + getIntFromParamBlockID(2, NULL, tex));
						stringValue.append("\n");
						stringValue.append("scene.textures." + tmpTexName + ".bright = " + getFloatFromParamBlockID(3, NULL, tex));
						stringValue.append("\n");
						stringValue.append("scene.textures." + tmpTexName + ".contrast = " + getFloatFromParamBlockID(4, NULL, tex));
						stringValue.append("\n");

						//Currently color diffuse is supported, we need a way to set *any* texture type.
						//We should set the texture name outside from here..
						if (texSlotName != "")
						{
							stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + "." + texSlotName + "= " + tmpTexName);
						}
						
						stringValue.append("\n");
					}
					else
					{
						OutputDebugStringW(L"\nUnsupported texture map in material: ");//mprintf(L"ERROR : Unsupported texture in material: '%s' , named: '%s' , will not render texture. standard bitmap is supported.\n", mat->GetName(), tex->GetName());
						OutputDebugStringW(mat->GetName());
					}
				}
			}
		}
	}
	//tex = pBlock->GetTexmap(0, GetCOREInterface()->GetTime(), 0);
	
	
	//}
	return stringValue;
}

std::string LuxMaxMaterials::getLightEmission(::Mtl* mat)
{
	std::string stringValue = "";
	bool enableEmission = false;

	for (int i = 0, count = mat->NumParamBlocks(); i < count; ++i)
	{
		IParamBlock2 *pBlock = mat->GetParamBlockByID(i);
		Interval      ivalid;
		for (int j = 0, count = pBlock->NumParams(); j < count; ++j)
		{
			std::string tmp = pBlock->GetLocalName(j).ToCStr();
			OutputDebugStringW(L"Checking param for 'enableemission: ");
			OutputDebugStringW(pBlock->GetLocalName(j));
			OutputDebugStringW(L"\n");
			if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "enableemission") == 0)
			{
				enableEmission = pBlock->GetInt(j, GetCOREInterface()->GetTime());
			}
		}
	}

	for (int i = 0, count = mat->NumParamBlocks(); i < count; ++i)
	{
		IParamBlock2 *pBlock = mat->GetParamBlockByID(i);
		Interval      ivalid;
	

		if (enableEmission)
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

			for (int j = 0, count = pBlock->NumParams(); j < count; ++j)
			{
				OutputDebugStringW(L"\nMatte params\n");
				OutputDebugStringW(pBlock->GetLocalName(j));

				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission") == 0)
				{
					Point3 color;
				
					pBlock->GetValue(j, GetCOREInterface()->GetTime(), color, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".emission = " + to_string(color.x) + " " + to_string(color.y)  + " " + to_string(color.z));
					stringValue.append("\n");

				}

				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_power") == 0)
				{
					float spectrum;
				
					pBlock->GetValue(j, GetCOREInterface()->GetTime(), spectrum, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".emission.power = " + to_string(spectrum));
					stringValue.append("\n");
				}

				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_efficency") == 0)
				{
					float efficency;

					pBlock->GetValue(j, GetCOREInterface()->GetTime(), efficency, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".emission.efficency = " + to_string(efficency));
					stringValue.append("\n");
				}


				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_map") == 0)
				{
					if (getTexturePathFromParamBlockID(j, mat) != "")
					{
						std::string emission_mapfile_name = getTextureName(j, mat);
						stringValue.append("scene.textures." + emission_mapfile_name + ".type = imagemap");
						stringValue.append("\n");
						stringValue.append("scene.textures." + emission_mapfile_name + ".file = " + "\"" + getTexturePathFromParamBlockID(j, mat) + "\"");
						stringValue.append("\n");
						stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + "emission.mapfile = " + emission_mapfile_name);
					}
				}
			
				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_gamma") == 0)
				{
					float gamma;

					pBlock->GetValue(j, GetCOREInterface()->GetTime(), gamma, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".emission.gamma = " + to_string(gamma));
					stringValue.append("\n");
				}

				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_samples") == 0)
				{
					int samples;

					pBlock->GetValue(j, GetCOREInterface()->GetTime(), samples, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".emission.samples = " + to_string(samples));
					stringValue.append("\n");
				}

				//emission_map_width
				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_map_width") == 0)
				{
					int map_width;

					pBlock->GetValue(j, GetCOREInterface()->GetTime(), map_width, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + "emission.map.width = " + to_string(map_width));
					stringValue.append("\n");
				}

				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_map_height") == 0)
				{
					int map_height;

					pBlock->GetValue(j, GetCOREInterface()->GetTime(), map_height, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + "emission.map.height = " + to_string(map_height));
					stringValue.append("\n");
				}

				if (::_stricmp(pBlock->GetLocalName(j).ToCStr(), "emission_id") == 0)
				{
					int emision_id;

					pBlock->GetValue(j, GetCOREInterface()->GetTime(), emision_id, ivalid);
					stringValue.append("scene.materials." + lmutil->ToNarrow(matName) + ".emission.id = " + to_string(emision_id));
					stringValue.append("\n");
				}

				//TODO: create a bool function that checks if a texture is defined by param 'name'..
				//use that to check, set texture or color based on that.

			}
		}
	}


	return stringValue;
}

std::string LuxMaxMaterials::getIntFromParamBlockID(int paramID, ::Mtl* mat, ::Texmap* tex)
{

	std::string stringValue = "";
	//float value = 0.0f;
	int value = 0;
	IParamBlock2 *pBlock = NULL;

	if (mat != NULL)
	{
		pBlock = mat->GetParamBlock(0);
	}
	else
	{
		pBlock = tex->GetParamBlock(0);
	}

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
		std::string tmpBumpTexName = getTextureName(3, mat);
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
		std::string tmpDiffTexName = getTextureName(1, mat);
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
		return getTexturePathFromParamBlockID(3, mat);
	}
}

std::string LuxMaxMaterials::getMaterialDiffuseTexturePath(::Mtl* mat)
{
	if (mat->ClassID() == LR_INTERNAL_MAT_TEMPLATE_CLASSID)
	{
		return getTexturePathFromParamBlockID(2, mat);
	}
	if ((mat->ClassID() == LR_INTERNAL_MATTE_CLASSID))
	{
		// 4 is diffuse, 6 is bumpmap
		return getTexturePathFromParamBlockID(1, mat);
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
	diffcolor.x = 155;
	diffcolor.y = 155;
	diffcolor.z = 155;

	if (mat->ClassID() == LR_INTERNAL_MATTE_CLASSID)
	{
		IParamBlock2 *pBlock = mat->GetParamBlock(0);
		pBlock->GetValue(0, GetCOREInterface()->GetTime(), diffcolor, ivalid);
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
	else if ((mat->ClassID() == LR_INTERNAL_MATTE_CLASSID))
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
		//if (getMaterialDiffuseTexturePath(mat) == "")
		if (exportTexturesInMaterial(mat) == "")
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
			tmpTexString.append(exportTexturesInMaterial(mat,"kd"));
		/*	tmpTexString.append("scene.textures." + diffuseMapName + ".type = imagemap");
			tmpTexString.append("\n");
			tmpTexString.append("scene.textures." + diffuseMapName + ".file = " + "\"" + getMaterialDiffuseTexturePath(mat) + "\"");
			tmpTexString.append("\n");*/
			//tmpTexString.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + diffuseMapName);
			//tmpTexString.append("\n");
		}

		if (bumpMapName != "")
		{
			tmpTexString.append("scene.materials." + lmutil->ToNarrow(matName) + ".bumptex = " + bumpMapName);
			tmpTexString.append("\n");
			tmpTexString.append("scene.materials." + lmutil->ToNarrow(matName) + ".bumpsamplingdistance = 1.0");
			tmpTexString.append("\n");
		}

		tmpTexString.append(getLightEmission(mat));
		
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

		/*if (getTexturePathFromParamBlockID(12, mat) == "")
		{*/
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".index = " + getFloatFromParamBlockID(12, mat));
			tmpmat.append("\n");
		/*}
		else
		{
			std::string indexTexName = getTextureName(12, mat);
			tmpmat.append("scene.textures." + indexTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + indexTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(12, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".index = " + indexTexName);
			tmpmat.append("\n");
		}*/
		
		//std::string temp = getIntFromParamBlockID(14, mat);
		//TODO: We should make a 'rounded' int (0) from the getintFromParamBlockID function.
		//Then return it as bool value.
		if (getIntFromParamBlockID( 13,mat) == "0")
		{
			//std::string temp = getIntFromParamBlockID(14, mat);

			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce = 0");
			tmpmat.append("\n");
		}
		else
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce = 1");
			tmpmat.append("\n");
		}
		

		tmpmat.append(getLightEmission(mat));

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

	else if (mat->ClassID() == LR_GLOSSYTRANSLUCENT_CLASSID)
	{
		OutputDebugStringW(_T("\nCreating GlossyTranslucent material.\n"));
		luxrays::Properties prop;

		std::string tmpmat;
		tmpmat.append(currmat + ".type = glossytranslucent");
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
			::Point3 ks;
			ks = getMaterialColor(4, mat);
			tmpmat.append(currmat + ".ks = " + std::to_string(ks.x) + " " + std::to_string(ks.y) + " " + std::to_string(ks.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string ks_map = getTextureName(5, mat);
			tmpmat.append("scene.textures." + ks_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + ks_map + ".file = " + "\"" + getTexturePathFromParamBlockID(5, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ks = " + ks_map);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(7, mat) == "")
		{
			::Point3 ks_bf;
			ks_bf = getMaterialColor(6, mat);
			tmpmat.append(currmat + ".ks_bf = " + std::to_string(ks_bf.x) + " " + std::to_string(ks_bf.y) + " " + std::to_string(ks_bf.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string ks_bf_map = getTextureName(7, mat);
			tmpmat.append("scene.textures." + ks_bf_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + ks_bf_map + ".file = " + "\"" + getTexturePathFromParamBlockID(7, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ks_bf = " + ks_bf_map);
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
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".uroughness_bf = " + getFloatFromParamBlockID(10, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string uroughness_bf_map = getTextureName(11, mat);
			tmpmat.append("scene.textures." + uroughness_bf_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + uroughness_bf_map + ".file = " + "\"" + getTexturePathFromParamBlockID(11, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".uroughness_bf = " + uroughness_bf_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(13, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness = " + getFloatFromParamBlockID(12, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string vroughness_map = getTextureName(13, mat);
			tmpmat.append("scene.textures." + vroughness_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + vroughness_map + ".file = " + "\"" + getTexturePathFromParamBlockID(13, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness = " + vroughness_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(15, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness_bf = " + getFloatFromParamBlockID(14, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string vroughness_bf_map = getTextureName(15, mat);
			tmpmat.append("scene.textures." + vroughness_bf_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + vroughness_bf_map + ".file = " + "\"" + getTexturePathFromParamBlockID(15, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".vroughness_bf = " + vroughness_bf_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(17, mat) == "")
		{
			::Point3 ka;
			ka = getMaterialColor(16, mat);
			tmpmat.append(currmat + ".ka = " + std::to_string(ka.x) + " " + std::to_string(ka.y) + " " + std::to_string(ka.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string ka_map = getTextureName(17, mat);
			tmpmat.append("scene.textures." + ka_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + ka_map + ".file = " + "\"" + getTexturePathFromParamBlockID(17, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ka = " + ka_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(19, mat) == "")
		{
			::Point3 ka_bf;
			ka_bf = getMaterialColor(18, mat);
			tmpmat.append(currmat + ".ka_bf = " + std::to_string(ka_bf.x) + " " + std::to_string(ka_bf.y) + " " + std::to_string(ka_bf.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string ka_bf_map = getTextureName(19, mat);
			tmpmat.append("scene.textures." + ka_bf_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + ka_bf_map + ".file = " + "\"" + getTexturePathFromParamBlockID(17, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ka_bf = " + ka_bf_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(21, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d = " + getFloatFromParamBlockID(20, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string d_map = getTextureName(21, mat);
			tmpmat.append("scene.textures." + d_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + d_map + ".file = " + "\"" + getTexturePathFromParamBlockID(21, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d = " + d_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(23, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d_bf = " + getFloatFromParamBlockID(22, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string d_bf_map = getTextureName(21, mat);
			tmpmat.append("scene.textures." + d_bf_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + d_bf_map + ".file = " + "\"" + getTexturePathFromParamBlockID(21, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d_bf = " + d_bf_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(25, mat) == "")
		{
			::Point3 index;
			index = getMaterialColor(24, mat);
			tmpmat.append(currmat + ".index = " + std::to_string(index.x) + " " + std::to_string(index.y) + " " + std::to_string(index.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string index_map = getTextureName(25, mat);
			tmpmat.append("scene.textures." + index_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + index_map + ".file = " + "\"" + getTexturePathFromParamBlockID(25, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".index = " + index_map);
			tmpmat.append("\n");
		}
		
		if (getTexturePathFromParamBlockID(27, mat) == "")
		{
			::Point3 index_bf;
			index_bf = getMaterialColor(26, mat);
			tmpmat.append(currmat + ".index = " + std::to_string(index_bf.x) + " " + std::to_string(index_bf.y) + " " + std::to_string(index_bf.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string index_bf_map = getTextureName(27, mat);
			tmpmat.append("scene.textures." + index_bf_map + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + index_bf_map + ".file = " + "\"" + getTexturePathFromParamBlockID(27, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".index_bf = " + index_bf_map);
			tmpmat.append("\n");
		}

		if (getIntFromParamBlockID(28, mat) == "0")
		{
			std::string temp = getIntFromParamBlockID(28, mat);

			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce = 0");
			tmpmat.append("\n");
		}
		else
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce = 1");
			tmpmat.append("\n");
		}


		if (getIntFromParamBlockID(29, mat) == "0")
		{
			std::string temp = getIntFromParamBlockID(28, mat);

			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce_bf = 0");
			tmpmat.append("\n");
		}
		else
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".multibounce_bf = 1");
			tmpmat.append("\n");
		}

		prop.SetFromString(tmpmat);
		scene.Parse(prop);
	}

	else if (mat->ClassID() == LR_Archglass_CLASS_ID)
	{
		OutputDebugStringW(_T("\nCreating archglass material.\n"));
		luxrays::Properties prop;

		

		std::string tmpmat;
		tmpmat.append(currmat + ".type = archglass");
		tmpmat.append("\n");

		if (getTexturePathFromParamBlockID(1, mat) == "")
		{
			Point3 kr;
			kr = getMaterialColor(0, mat);
			tmpmat.append(currmat + ".kr = " + std::to_string(kr.x) + " " + std::to_string(kr.y) + " " + std::to_string(kr.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string krTexName = getTextureName(1, mat);
			tmpmat.append("scene.textures." + krTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + krTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(1, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kr = " + krTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(3, mat) == "")
		{
			Point3 kt;
			kt = getMaterialColor(2, mat);
			tmpmat.append(currmat + ".kt = " + std::to_string(kt.x) + " " + std::to_string(kt.y) + " " + std::to_string(kt.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string ktTexName = getTextureName(3, mat);
			tmpmat.append("scene.textures." + ktTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + ktTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(3, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kt = " + ktTexName);
			tmpmat.append("\n");
		}


		if (getTexturePathFromParamBlockID(5, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".interiorior = " + getFloatFromParamBlockID(4, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string interioriorTexName = getTextureName(5, mat);
			tmpmat.append("scene.textures." + interioriorTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + interioriorTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(5, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".interiorior = " + interioriorTexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(7, mat) == "")
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".exteriorior = " + getFloatFromParamBlockID(6, mat));
			tmpmat.append("\n");
		}
		else
		{
			std::string exterioriorTexName = getTextureName(7, mat);
			tmpmat.append("scene.textures." + exterioriorTexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + exterioriorTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(7, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".exteriorior = " + exterioriorTexName);
			tmpmat.append("\n");
		}

		prop.SetFromString(tmpmat);
		scene.Parse(prop);
	}
	else if (mat->ClassID() == LR_Cloth_CLASS_ID)
	{
		OutputDebugStringW(_T("\nCreating cloth material.\n"));
		luxrays::Properties prop;
		std::string tmpmat;
		tmpmat.append(currmat + ".type = cloth");
		tmpmat.append("\n");

		int presetIndex = mat->GetParamBlock(0)->GetInt(0, GetCOREInterface()->GetTime());;

		if (presetIndex == 1)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = denim");
			tmpmat.append("\n");
		}
		if (presetIndex == 2)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = silk_charmeuse");
			tmpmat.append("\n");
		}
		if (presetIndex == 3)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = silk_shantung");
			tmpmat.append("\n");
		}
		if (presetIndex == 4)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = cotton_twill");
			tmpmat.append("\n");
		}
		if (presetIndex == 5)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = wool_garbardine");
			tmpmat.append("\n");
		}
		if (presetIndex == 6)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = polyester_lining_clot");
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(2, mat) == "")
		{
			Point3 kd;
			kd = getMaterialColor(1, mat);
			tmpmat.append(currmat + ".weft_kd = " + std::to_string(kd.x) + " " + std::to_string(kd.y) + " " + std::to_string(kd.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string weft_kd_TexName = getTextureName(2, mat);
			tmpmat.append("scene.textures." + weft_kd_TexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + weft_kd_TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(2, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".weft_kd = " + weft_kd_TexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(4, mat) == "")
		{
			Point3 ks;
			ks = getMaterialColor(3, mat);
			tmpmat.append(currmat + ".weft_ks = " + std::to_string(ks.x) + " " + std::to_string(ks.y) + " " + std::to_string(ks.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string weft_ks_TexName = getTextureName(4, mat);
			tmpmat.append("scene.textures." + weft_ks_TexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + weft_ks_TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(4, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".weft_ks = " + weft_ks_TexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(6, mat) == "")
		{
			Point3 kd;
			kd = getMaterialColor(5, mat);
			tmpmat.append(currmat + ".warp_kd = " + std::to_string(kd.x) + " " + std::to_string(kd.y) + " " + std::to_string(kd.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string warp_kd_TexName = getTextureName(6, mat);
			tmpmat.append("scene.textures." + warp_kd_TexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + warp_kd_TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(6, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".warp_kd = " + warp_kd_TexName);
			tmpmat.append("\n");
		}

		if (getTexturePathFromParamBlockID(8, mat) == "")
		{
			Point3 ks;
			ks = getMaterialColor(7, mat);
			tmpmat.append(currmat + ".warp_ks = " + std::to_string(ks.x) + " " + std::to_string(ks.y) + " " + std::to_string(ks.z));
			tmpmat.append("\n");
		}
		else
		{
			std::string warp_ks_TexName = getTextureName(8, mat);
			tmpmat.append("scene.textures." + warp_ks_TexName + ".type = imagemap");
			tmpmat.append("\n");
			tmpmat.append("scene.textures." + warp_ks_TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(8, mat) + "\"");
			tmpmat.append("\n");
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".warp_ks = " + warp_ks_TexName);
			tmpmat.append("\n");
		}

		tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".repeat_u = " + getFloatFromParamBlockID(9, mat));
		tmpmat.append("\n");
		tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".repeat_v = " + getFloatFromParamBlockID(10, mat));
		tmpmat.append("\n");

		prop.SetFromString(tmpmat);
		scene.Parse(prop);
	}

	else if (mat->ClassID() == LR_Carpaint_CLASS_ID)
	{
		OutputDebugStringW(_T("\nCreating carpaint material.\n"));
		luxrays::Properties prop;
		std::string tmpmat;
		tmpmat.append(currmat + ".type = carpaint");
		tmpmat.append("\n");

		int presetIndex = mat->GetParamBlock(0)->GetInt(0, GetCOREInterface()->GetTime());;

		if (presetIndex == 1)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = \"ford f8\"");
			tmpmat.append("\n");
		}
		if (presetIndex == 2)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = \"polaris silber\"");
			tmpmat.append("\n");
		}
		if (presetIndex == 3)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = \"opel titan\"");
			tmpmat.append("\n");
		}
		if (presetIndex == 4)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = bmw339");
			tmpmat.append("\n");
		}
		if (presetIndex == 5)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = \"2k acrylack\"");
			tmpmat.append("\n");
		}
		if (presetIndex == 6)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = white");
			tmpmat.append("\n");
		}
		if (presetIndex == 7)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = blue");
			tmpmat.append("\n");
		}
		if (presetIndex == 8)
		{
			tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".preset = \"blue matte\"");
			tmpmat.append("\n");
		}


		if (presetIndex == 0)
		{
			if (getTexturePathFromParamBlockID(2, mat) == "")
			{
				Point3 ka;
				ka = getMaterialColor(1, mat);
				tmpmat.append(currmat + ".ka = " + std::to_string(ka.x) + " " + std::to_string(ka.y) + " " + std::to_string(ka.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string kaTexName = getTextureName(2, mat);
				tmpmat.append("scene.textures." + kaTexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + kaTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(2, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ka = " + kaTexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(4, mat) == "")
			{
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d = " + getFloatFromParamBlockID(3, mat));
				tmpmat.append("\n");
			}
			else
			{
				std::string dTexName = getTextureName(4, mat);
				tmpmat.append("scene.textures." + dTexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + dTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(4, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".d = " + dTexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(6, mat) == "")
			{
				Point3 kd;
				kd = getMaterialColor(5, mat);
				tmpmat.append(currmat + ".kd = " + std::to_string(kd.x) + " " + std::to_string(kd.y) + " " + std::to_string(kd.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string kdTexName = getTextureName(6, mat);
				tmpmat.append("scene.textures." + kdTexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + kdTexName + ".file = " + "\"" + getTexturePathFromParamBlockID(6, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".kd = " + kdTexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(8, mat) == "")
			{
				Point3 ks1;
				ks1 = getMaterialColor(7, mat);
				tmpmat.append(currmat + ".ks1 = " + std::to_string(ks1.x) + " " + std::to_string(ks1.y) + " " + std::to_string(ks1.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string ks1TexName = getTextureName(8, mat);
				tmpmat.append("scene.textures." + ks1TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + ks1TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(8, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ks1 = " + ks1TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(10, mat) == "")
			{
				Point3 ks2;
				ks2 = getMaterialColor(9, mat);
				tmpmat.append(currmat + ".ks2 = " + std::to_string(ks2.x) + " " + std::to_string(ks2.y) + " " + std::to_string(ks2.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string ks2TexName = getTextureName(10, mat);
				tmpmat.append("scene.textures." + ks2TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + ks2TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(10, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ks2 = " + ks2TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(12, mat) == "")
			{
				Point3 ks3;
				ks3 = getMaterialColor(11, mat);
				tmpmat.append(currmat + ".ks3 = " + std::to_string(ks3.x) + " " + std::to_string(ks3.y) + " " + std::to_string(ks3.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string ks3TexName = getTextureName(12, mat);
				tmpmat.append("scene.textures." + ks3TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + ks3TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(12, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".ks3 = " + ks3TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(14, mat) == "")
			{
				Point3 r1;
				r1 = getMaterialColor(13, mat);
				tmpmat.append(currmat + ".r1 = " + std::to_string(r1.x) + " " + std::to_string(r1.y) + " " + std::to_string(r1.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string r1TexName = getTextureName(14, mat);
				tmpmat.append("scene.textures." + r1TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + r1TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(14, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".r1 = " + r1TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(16, mat) == "")
			{
				Point3 r2;
				r2 = getMaterialColor(15, mat);
				tmpmat.append(currmat + ".r2 = " + std::to_string(r2.x) + " " + std::to_string(r2.y) + " " + std::to_string(r2.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string r2TexName = getTextureName(16, mat);
				tmpmat.append("scene.textures." + r2TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + r2TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(16, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".r2 = " + r2TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(18, mat) == "")
			{
				Point3 r3;
				r3 = getMaterialColor(17, mat);
				tmpmat.append(currmat + ".r3 = " + std::to_string(r3.x) + " " + std::to_string(r3.y) + " " + std::to_string(r3.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string r3TexName = getTextureName(18, mat);
				tmpmat.append("scene.textures." + r3TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + r3TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(18, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".r3 = " + r3TexName);
				tmpmat.append("\n");
			}


			if (getTexturePathFromParamBlockID(20, mat) == "")
			{
				Point3 m1;
				m1 = getMaterialColor(19, mat);
				tmpmat.append(currmat + ".m1 = " + std::to_string(m1.x) + " " + std::to_string(m1.y) + " " + std::to_string(m1.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string m1TexName = getTextureName(20, mat);
				tmpmat.append("scene.textures." + m1TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + m1TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(20, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".m1 = " + m1TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(22, mat) == "")
			{
				Point3 m2;
				m2 = getMaterialColor(21, mat);
				tmpmat.append(currmat + ".m2 = " + std::to_string(m2.x) + " " + std::to_string(m2.y) + " " + std::to_string(m2.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string m2TexName = getTextureName(22, mat);
				tmpmat.append("scene.textures." + m2TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + m2TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(22, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".m2 = " + m2TexName);
				tmpmat.append("\n");
			}

			if (getTexturePathFromParamBlockID(24, mat) == "")
			{
				Point3 m3;
				m3 = getMaterialColor(23, mat);
				tmpmat.append(currmat + ".m3 = " + std::to_string(m3.x) + " " + std::to_string(m3.y) + " " + std::to_string(m3.z));
				tmpmat.append("\n");
			}
			else
			{
				std::string m3TexName = getTextureName(24, mat);
				tmpmat.append("scene.textures." + m3TexName + ".type = imagemap");
				tmpmat.append("\n");
				tmpmat.append("scene.textures." + m3TexName + ".file = " + "\"" + getTexturePathFromParamBlockID(24, mat) + "\"");
				tmpmat.append("\n");
				tmpmat.append("scene.materials." + lmutil->ToNarrow(matName) + ".m3 = " + m3TexName);
				tmpmat.append("\n");
			}
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
		//diffcol = getMaterialDiffuseColor(mat);
		diffcol.x = 0.5;
		diffcol.y = 0.5;
		diffcol.z = 0.5;

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
	else if (mat->ClassID() == LR_GLOSSYTRANSLUCENT_CLASSID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_Archglass_CLASS_ID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_Cloth_CLASS_ID)
	{
		return true;
	}
	else if (mat->ClassID() == LR_Carpaint_CLASS_ID)
	{
		return true;
	}
	else
	{
		return false;
	}
}