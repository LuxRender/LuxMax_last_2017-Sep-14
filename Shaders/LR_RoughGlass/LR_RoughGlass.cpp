//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR:
//***************************************************************************/

#include "LR_RoughGlass.h"
#include <maxscript\maxscript.h>

#define LR_RoughGlass_CLASS_ID	Class_ID(0x56b76e70, 0x8de321e5)


#define NUM_SUBMATERIALS 6 // TODO: number of sub-materials supported by this plug-in
#define NUM_SUBTEXTURES 6
#define Num_REF 4
// Reference Indexes
// 
#define PBLOCK_REF 1

class LR_RoughGlass : public Mtl {
public:
	LR_RoughGlass();
	LR_RoughGlass(BOOL loading);
	~LR_RoughGlass();


	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp);
	void      Update(TimeValue t, Interval& valid);
	Interval  Validity(TimeValue t);
	void      Reset();

	void NotifyChanged();

	// From MtlBase and Mtl
	virtual void SetAmbient(Color c, TimeValue t);
	virtual void SetDiffuse(Color c, TimeValue t);
	virtual void SetSpecular(Color c, TimeValue t);
	virtual void SetShininess(float v, TimeValue t);
	virtual Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	virtual Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
	virtual Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
	virtual float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
	virtual float GetShininess(int mtlNum=0, BOOL backFace=FALSE);
	virtual float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
	virtual float WireSize(int mtlNum=0, BOOL backFace=FALSE);


	// Shade and displacement calculation
	virtual void     Shade(ShadeContext& sc);
	virtual float    EvalDisplacement(ShadeContext& sc);
	virtual Interval DisplacementValidity(TimeValue t);

	// SubMaterial access methods
	//virtual int  NumSubMtls() {return NUM_SUBMATERIALS;}
	virtual int  NumSubMtls() { return 0; }
	virtual Mtl* GetSubMtl(int i);
	virtual void SetSubMtl(int i, Mtl *m);
	virtual TSTR GetSubMtlSlotName(int i);
	virtual TSTR GetSubMtlTVName(int i);

	// SubTexmap access methods
	virtual int     NumSubTexmaps() { return NUM_SUBTEXTURES; }
	virtual Texmap* GetSubTexmap(int i);
	virtual void    SetSubTexmap(int i, Texmap *tx);
	virtual TSTR    GetSubTexmapSlotName(int i);
	virtual TSTR    GetSubTexmapTVName(int i);

	virtual BOOL SetDlgThing(ParamDlg* dlg);

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	// From Animatable
	virtual Class_ID ClassID() {return LR_RoughGlass_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);

	virtual int NumSubs() { return 1+NUM_SUBMATERIALS; }
	virtual Animatable* SubAnim(int i);
	virtual TSTR SubAnimName(int i);

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 1 + Num_REF; }
	virtual RefTargetHandle GetReference(int i);

	virtual int NumParamBlocks() { return 1; }					  // return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis() { delete this; }

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	Mtl*          submtl[NUM_SUBMATERIALS];  // Fixed size Reference array of sub-materials. (Indexes: 0-(N-1))
	Texmap*       subtexture[NUM_SUBTEXTURES];
	IParamBlock2* pblock;					 // Reference that comes AFTER the sub-materials. (Index: N)
	
	BOOL          mapOn[NUM_SUBMATERIALS];
	float         spin;
	Interval      ivalid;
	Interval	  mapValid;
};



class LR_RoughGlassClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL loading = FALSE) 		{ return new LR_RoughGlass(loading); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return MATERIAL_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return LR_RoughGlass_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("LR_RoughGlass"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetLR_RoughGlassDesc() { 
	static LR_RoughGlassClassDesc LR_RoughGlassDesc;
	return &LR_RoughGlassDesc; 
}





enum { LR_RoughGlass_params };


//TODO: Add enums for various parameters
enum 
{
	kr,
	kr_map,
	kt,
	kt_map,
	interiorior,
	interiorior_map,
	exteriorior,
	exteriorior_map,
	uroughness,
	uroughness_map,
	vroughness,
	vroughness_map,
};




static ParamBlockDesc2 LR_RoughGlass_param_blk (
	LR_RoughGlass_params, _T("params"),  0, GetLR_RoughGlassDesc(),	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	
	kr, _T("kr"), TYPE_RGBA, P_ANIMATABLE, "kr",
	p_default, Color(0.5f, 0.5f, 0.5f),
	p_ui, TYPE_COLORSWATCH, IDC_KR_COLOR,
	p_end,
	
	kr_map, _T("kr_map"), TYPE_TEXMAP, P_OWNERS_REF, "kr_map",
	p_refno, 2,
	p_subtexno, 0,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KR_MAP,
	p_end,
	
	kt, _T("kt"), TYPE_RGBA, P_ANIMATABLE, "kt",
	p_default, Color(0.5f, 0.5f, 0.5f),
	p_ui, TYPE_COLORSWATCH, IDC_KT_COLOR,
	p_end,

	kt_map, _T("kt_map"), TYPE_TEXMAP, P_OWNERS_REF, "kt_map",
	p_refno, 3,
	p_subtexno, 1,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KT_MAP,
	p_end,

	interiorior, _T("interiorior"), TYPE_FLOAT, P_ANIMATABLE, "interiorior",
	p_default, 1.5f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_INTERIORIOR, IDC_INTERIOR_SPIN, 0.0f,
	p_end, 

	interiorior_map, _T("interiorior_map"), TYPE_TEXMAP, P_OWNERS_REF, "interiorior_map",
	p_refno, 4,
	p_subtexno, 2,
	p_ui, TYPE_TEXMAPBUTTON, IDC_INTERIORIOR_MAP,
	p_end,

	exteriorior, _T("exteriorior"), TYPE_FLOAT, P_ANIMATABLE, "exteriorior",
	p_default, 1.0f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_EXTERIORIOR, IDC_EXTERIORIOR_SPIN, 0.0f,
	p_end,

	exteriorior_map, _T("exteriorior_map"), TYPE_TEXMAP, P_OWNERS_REF, "exteriorior_map",
	p_refno, 5,
	p_subtexno, 3,
	p_ui, TYPE_TEXMAPBUTTON, IDC_EXTERIORIOR_MAP,
	p_end,

	uroughness, _T("uroughness"), TYPE_FLOAT, P_ANIMATABLE, "uroughness",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_UROUGHNESS, IDC_UROUGHNESS_SPIN, 0.0f,
	p_end,

	uroughness_map, _T("uroughness_map"), TYPE_TEXMAP, P_OWNERS_REF, "uroughness_map",
	p_refno, 6,
	p_subtexno, 4,
	p_ui, TYPE_TEXMAPBUTTON, IDC_UROUGHNESS_MAP,
	p_end,

	vroughness, _T("vroughness"), TYPE_FLOAT, P_ANIMATABLE, "vroughness",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_VROUGHNESS, IDC_VROUGHNESS_SPIN, 0.0f,
	p_end,

	vroughness_map, _T("vroughness_map"), TYPE_TEXMAP, P_OWNERS_REF, "vroughness_map",
	p_refno, 7,
	p_subtexno, 5,
	p_ui, TYPE_TEXMAPBUTTON, IDC_VROUGHNESS_MAP,
	p_end,


	//// examples:
	/*prm_color,			_T("color"),			TYPE_RGBA,	P_ANIMATABLE,		IDS_COLOR,
		p_default,		Color(0.5f, 0.5f, 0.5f),
		p_ui,			TYPE_COLORSWATCH,		IDC_SAMP_COLOR,
		p_end,
	mtl_diffuse_map,	_T("mtl_DiffuseMap"),	TYPE_TEXMAP,	P_OWNERS_REF,	IDS_DIFFUSE_MAP,
		p_refno,		2,
		p_subtexno,		0,
		p_ui,			TYPE_TEXMAPBUTTON,		IDC_DIFFUSE_MAP,
		p_end,
	
		mtl_sigma_float, _T("sigma"), TYPE_FLOAT, P_ANIMATABLE, IDC_SIGMA_SPIN,
		p_default, 0.0f,
		p_range, 0.0f, 360.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIGMA, IDC_SIGMA_SPIN, 0.0f,
		p_end,*/



	p_end
	);




LR_RoughGlass::LR_RoughGlass()
	: pblock(nullptr)
{
	for (int i = 0; i < NUM_SUBMATERIALS; i++)
	{
		submtl[i] = nullptr;
	}
	for (int i = 0; i < NUM_SUBTEXTURES; i++)
	{
		subtexture[i] = nullptr;
	}
	Reset();
}

LR_RoughGlass::LR_RoughGlass(BOOL loading)
	: pblock(nullptr)
{
	for (int i = 0; i < NUM_SUBMATERIALS; i++)
	{
		submtl[i] = nullptr;
	}
	for (int i = 0; i < NUM_SUBTEXTURES; i++)
	{
		subtexture[i] = nullptr;
	}
	
	if (!loading)
		Reset();
}

LR_RoughGlass::~LR_RoughGlass()
{
	DeleteAllRefs();
}


void LR_RoughGlass::Reset()
{
	ivalid.SetEmpty();
	mapValid.SetEmpty();
	// Always have to iterate backwards when deleting references.
	for (int i = NUM_SUBMATERIALS - 1; i >= 0; i--)
	{
		if( submtl[i] )
		{
			DeleteReference(i);
			DbgAssert(submtl[i] == nullptr);
			submtl[i] = nullptr;
		}
		mapOn[i] = FALSE;
	}
	for (int i = NUM_SUBTEXTURES - 1; i >= 0; i--)
	{
		if (subtexture[i])
		{
			DeleteReference(i);
			DbgAssert(subtexture[i] == nullptr);
			subtexture[i] = nullptr;
		}
		//mapOn[i] = FALSE;
	}
	DeleteReference(PBLOCK_REF);

	GetLR_RoughGlassDesc()->MakeAutoParamBlocks(this);
}



ParamDlg* LR_RoughGlass::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	IAutoMParamDlg* masterDlg = GetLR_RoughGlassDesc()->CreateParamDlgs(hwMtlEdit, imp, this);
	// TODO: Set param block user dialog if necessary
	return masterDlg;
	
}

BOOL LR_RoughGlass::SetDlgThing(ParamDlg* /*dlg*/)
{
	return FALSE;
}

Interval LR_RoughGlass::Validity(TimeValue t)
{
	Interval valid = FOREVER;

	for (int i = 0; i < NUM_SUBMATERIALS; i++)
	{
		if (submtl[i])
			valid &= submtl[i]->Validity(t);
	}
	for (int i = 0; i < NUM_SUBTEXTURES; i++)
	{
		if (subtexture[i])
			valid &= subtexture[i]->Validity(t);
	}
	//float u;
	//pblock->GetValue(pb_spin,t,u,valid);
	return valid;
}

/*===========================================================================*\
 |	Sub-anim & References support
\*===========================================================================*/

RefTargetHandle LR_RoughGlass::GetReference(int i)
{
	switch (i)
	{
		//case 0: return subtexture[i]; break;
	case 1: return pblock; break;
		//case 2: return subtexture[i-2]; break;
	default: return subtexture[i - 2]; break;
	}

}

void LR_RoughGlass::SetReference(int i, RefTargetHandle rtarg)
{
	//mprintf(_T("\n SetReference Nubmer is ------->>>>: %i \n"), i);
	switch (i)
	{
		//case 0: subtexture[i] = (Texmap *)rtarg; break;
		case 1: pblock = (IParamBlock2 *)rtarg; break;
		//case 2: subtexture[i-2] = (Texmap *)rtarg; break;
		default: subtexture[i-2] = (Texmap *)rtarg; break;
	}
}

TSTR LR_RoughGlass::SubAnimName(int i)
{
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return GetSubTexmapTVName(i);
	else
		return GetSubTexmapTVName(i-2);
}

Animatable* LR_RoughGlass::SubAnim(int i)
{
	switch (i)
	{
	case 0: return subtexture[i];
	case 1: return pblock;
	default: return subtexture[i-2];
	}
}

RefResult LR_RoughGlass::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, 
	PartID& /*partID*/, RefMessage message, BOOL /*propagate*/ ) 
{
	switch (message) {
	case REFMSG_CHANGE:
		{
		ivalid.SetEmpty();
		mapValid.SetEmpty();
			if (hTarget == pblock)
			{
				ParamID changing_param = pblock->LastNotifyParamID();
				LR_RoughGlass_param_blk.InvalidateUI(changing_param);
			}
		}
		break;
	case REFMSG_TARGET_DELETED:
		{
			if (hTarget == pblock)
			{
				pblock = nullptr;
			} 
			else
			{
				for (int i = 0; i < NUM_SUBMATERIALS; i++)
				{
					if (hTarget == submtl[i])
					{
						submtl[i] = nullptr;
						break;
					}
				}
				for (int i = 0; i < NUM_SUBTEXTURES; i++)
				{
					if (hTarget == subtexture[i])
					{
						subtexture[i] = nullptr;
						break;
					}
				}
			}
			break;
		}
	}
	return REF_SUCCEED;
}

/*===========================================================================*\
 |	SubMtl get and set
\*===========================================================================*/

Mtl* LR_RoughGlass::GetSubMtl(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	return 
		nullptr;
}

void LR_RoughGlass::SetSubMtl(int i, Mtl* m)
{
	//mprintf(_T("\n SetSubMtl Nubmer is : %i \n"), i);
	ReplaceReference(i , m);
	if (i == 0)
	{
		LR_RoughGlass_param_blk.InvalidateUI(kr_map);
		mapValid.SetEmpty();
	}
}

TSTR LR_RoughGlass::GetSubMtlSlotName(int i)
{
	// Return i'th sub-material name
	return submtl[i]->GetName();
	//return _T("");
}

TSTR LR_RoughGlass::GetSubMtlTVName(int i)
{
	return GetSubMtlSlotName(i);
}

/*===========================================================================*\
 |	Texmap get and set
\*===========================================================================*/

Texmap* LR_RoughGlass::GetSubTexmap(int i)
{
	//mprintf(_T("\n GetSubTexmap Nubmer ::::::::::::===>>>  is : Get %i \n"), i);
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return subtexture[i];
	return
		nullptr;
}

void LR_RoughGlass::SetSubTexmap(int i, Texmap* tx)
{
	//mprintf(_T("\n SetSubTexmap Nubmer ============>>>  is : %i \n"), i);
	ReplaceReference(i +2, tx);
	if (i == 0)
	{
		LR_RoughGlass_param_blk.InvalidateUI(kr_map);
		mapValid.SetEmpty();
	}
	if (i == 1)
		{
			LR_RoughGlass_param_blk.InvalidateUI(kt_map);
			mapValid.SetEmpty();
		}
	if (i == 2)
	{
		LR_RoughGlass_param_blk.InvalidateUI(interiorior_map);
		mapValid.SetEmpty();
	}
	if (i == 3)
	{
		LR_RoughGlass_param_blk.InvalidateUI(exteriorior_map);
		mapValid.SetEmpty();
	}
	if (i == 4)
	{
		LR_RoughGlass_param_blk.InvalidateUI(uroughness_map);
		mapValid.SetEmpty();
	}
	if (i == 5)
	{
		LR_RoughGlass_param_blk.InvalidateUI(vroughness_map);
		mapValid.SetEmpty();
	}

//	else
//	{
//		LR_RoughGlass_param_blk.InvalidateUI(mtl_bump_map);
//		mapValid.SetEmpty();
//	}
}

TSTR LR_RoughGlass::GetSubTexmapSlotName(int i)
{
	switch (i)
	{
		case 0:
			return _T("kr_map");
		case 1:
			return _T("kt_map");
		case 2:
			return _T("interiorior_map");
		case 3:
			return _T("exteriorior_map"); 
		case 4:
			return _T("uroughness_map");
		case 5:
			return _T("vroughness_map");
		default:
			return _T("kr_map");
	}
}

TSTR LR_RoughGlass::GetSubTexmapTVName(int i)
{
	// Return i'th sub-texture name
	return GetSubTexmapSlotName(i);
}



/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000
#define PARAM2_CHUNK 0x1010

IOResult LR_RoughGlass::Save(ISave* isave)
{
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK)
		return res;
	isave->EndChunk();

	return IO_OK;
}

IOResult LR_RoughGlass::Load(ILoad* iload)
{
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk()))
	{
		int id = iload->CurChunkID();
		switch(id)
		{
		case MTL_HDR_CHUNK:
			res = MtlBase::Load(iload);
			break;
		}

		iload->CloseChunk();
		if (res!=IO_OK)
			return res;
	}

	return IO_OK;
}


/*===========================================================================*\
 |	Updating and cloning
\*===========================================================================*/

RefTargetHandle LR_RoughGlass::Clone(RemapDir &remap)
{
	LR_RoughGlass *mnew = new LR_RoughGlass(FALSE);
	*((MtlBase*)mnew) = *((MtlBase*)this);
	// First clone the parameter block
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	// Next clone the sub-materials
	mnew->ivalid.SetEmpty();
	mnew->mapValid.SetEmpty();
	for (int i = 0; i < NUM_SUBMATERIALS; i++) 
	{
		mnew->submtl[i] = nullptr;
		if (submtl[i])
			mnew->ReplaceReference(i,remap.CloneRef(submtl[i]));
		mnew->mapOn[i] = mapOn[i];
	}
	for (int i = 0; i < NUM_SUBTEXTURES; i++)
	{
		mnew->subtexture[i] = nullptr;
		if (subtexture[i])
			mnew->ReplaceReference(i + 2, remap.CloneRef(subtexture[i]));
		//mnew->mapOn[i] = mapOn[i];
	}
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
	}

void LR_RoughGlass::NotifyChanged()
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void LR_RoughGlass::Update(TimeValue t, Interval& valid)
{
	if (!ivalid.InInterval(t))
	{

		ivalid.SetInfinite();
		//pblock->GetValue( kr_map_on, t, mapOn[0], ivalid);
		//pblock->GetValue( pb_spin, t, spin, ivalid);

		for (int i=0; i < NUM_SUBMATERIALS; i++)
		{
			if (submtl[i])
				submtl[i]->Update(t,ivalid);
		}
	}

	if (!mapValid.InInterval(t))
	{
		mapValid.SetInfinite();
		for (int i = 0; i<NUM_SUBTEXTURES; i++) {
			if (subtexture[i])
				subtexture[i]->Update(t, mapValid);
		}
	}

	valid &= mapValid;
	valid &= ivalid;
}

/*===========================================================================*\
 |	Determine the characteristics of the material
\*===========================================================================*/

void LR_RoughGlass::SetAmbient(Color /*c*/, TimeValue /*t*/) {}		
void LR_RoughGlass::SetDiffuse(Color /*c*/, TimeValue /*t*/) {}		
void LR_RoughGlass::SetSpecular(Color /*c*/, TimeValue /*t*/) {}
void LR_RoughGlass::SetShininess(float /*v*/, TimeValue /*t*/) {}

Color LR_RoughGlass::GetAmbient(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(kr_map, GetCOREInterface()->GetTime(), p, ivalid);
	return submtl[0] ? submtl[0]->GetAmbient(mtlNum, backFace) : Color(p.x, p.y, p.z);//Bound(Color(p.x, p.y, p.z));
}

Color LR_RoughGlass::GetDiffuse(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(kr_map, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetDiffuse(mtlNum, backFace) : Color(p.x, p.y, p.z);
}

Color LR_RoughGlass::GetSpecular(int mtlNum, BOOL backFace)
{
	Point3 p;
	pblock->GetValue(kr_map, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetSpecular(mtlNum,backFace): Color(p.x, p.y, p.z);
}

float LR_RoughGlass::GetXParency(int mtlNum, BOOL backFace)
{
	float t = 0.0f;
	//pblock->GetValue(pb_opacity, 0, t, ivalid);
	return submtl[0] ? submtl[0]->GetXParency(mtlNum,backFace): t;
}

float LR_RoughGlass::GetShininess(int mtlNum, BOOL backFace)
{
	float sh = 1.0f;
	//pblock->GetValue(pb_shin, 0, sh, ivalid);
	return submtl[0] ? submtl[0]->GetShininess(mtlNum,backFace): sh;
}

float LR_RoughGlass::GetShinStr(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetShinStr(mtlNum,backFace): 0.0f;
}

float LR_RoughGlass::WireSize(int mtlNum, BOOL backFace)
{
	float wf = 0.0f;
	//pblock->GetValue(pb_wiresize, 0, wf, ivalid);
	return submtl[0] ? submtl[0]->WireSize(mtlNum, backFace) : wf;
}


/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void LR_RoughGlass::Shade(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	if (gbufID)
		sc.SetGBufferID(gbufID);

	if(subMaterial)
		subMaterial->Shade(sc);
	// TODO: compute the color and transparency output returned in sc.out.
}

float LR_RoughGlass::EvalDisplacement(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	return (subMaterial) ? subMaterial->EvalDisplacement(sc) : 0.0f;
}

Interval LR_RoughGlass::DisplacementValidity(TimeValue t)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;

	Interval iv;
	iv.SetInfinite();
	if(subMaterial) 
		iv &= subMaterial->DisplacementValidity(t);

	return iv;
}


