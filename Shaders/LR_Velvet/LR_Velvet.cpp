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

#include "LR_Velvet.h"
#include <maxscript\maxscript.h>

#define LR_Velvet_CLASS_ID	Class_ID(0x59b79e53, 0x2de567e3)


#define NUM_SUBMATERIALS 5 // TODO: number of sub-materials supported by this plug-in
#define NUM_SUBTEXTURES 5
#define Num_REF 5 //was 4..
// Reference Indexes
// 
#define PBLOCK_REF 1

class LR_Velvet : public Mtl {
public:
	LR_Velvet();
	LR_Velvet(BOOL loading);
	~LR_Velvet();


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
	virtual Class_ID ClassID() {return LR_Velvet_CLASS_ID;}
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



class LR_VelvetClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL loading = FALSE) 		{ return new LR_Velvet(loading); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return MATERIAL_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return LR_Velvet_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("LR_Velvet"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetLR_VelvetDesc() { 
	static LR_VelvetClassDesc LR_VelvetDesc;
	return &LR_VelvetDesc; 
}





enum { LR_Velvet_params };


//TODO: Add enums for various parameters
enum 
{
	kd,
	kd_map,
	p1,
	p1_map,
	p2,
	p2_map,
	p3,
	p3_map,
	thickness,
	thickness_map,
};




static ParamBlockDesc2 LR_Velvet_param_blk (
	LR_Velvet_params, _T("params"),  0, GetLR_VelvetDesc(),	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params

	kd, _T("kd"), TYPE_RGBA, P_ANIMATABLE, "kd",
	p_default, Color(0.5f, 0.5f, 0.5f),
	p_ui, TYPE_COLORSWATCH, IDC_KD_COLOR,
	p_end,

	kd_map, _T("kd_map"), TYPE_TEXMAP, P_OWNERS_REF, "kd_map",
	p_refno, 2,
	p_subtexno, 0,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KD_MAP,
	p_end,

	p3, _T("p1"), TYPE_FLOAT, P_ANIMATABLE, IDC_P1_SPIN,
	p_default, -2.0f,
	p_range, -999.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_P1, IDC_P1_SPIN, 0.0f,
	p_end,

	p1_map, _T("p1_map"), TYPE_TEXMAP, P_OWNERS_REF, "p1",
	p_refno, 3,
	p_subtexno, 1,
	p_ui, TYPE_TEXMAPBUTTON, IDC_P1_MAP,
	p_end,

	p2, _T("p2"), TYPE_FLOAT, P_ANIMATABLE, IDC_P2_SPIN,
	p_default, 20.0f,
	p_range, -999.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_P2, IDC_P2_SPIN, 0.0f,
	p_end,

	p2_map, _T("p2_map"), TYPE_TEXMAP, P_OWNERS_REF, "p2",
	p_refno, 4,
	p_subtexno, 2,
	p_ui, TYPE_TEXMAPBUTTON, IDC_P2_MAP,
	p_end,

	p3, _T("p3"), TYPE_FLOAT, P_ANIMATABLE, IDC_P3_SPIN,
	p_default, 2.0f,
	p_range, -999.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_P3, IDC_P3_SPIN, 0.0f,
	p_end,

	p3_map, _T("p3_map"), TYPE_TEXMAP, P_OWNERS_REF, "p3",
	p_refno, 5,
	p_subtexno, 3,
	p_ui, TYPE_TEXMAPBUTTON, IDC_P3_MAP,
	p_end,

	thickness, _T("thickness"), TYPE_FLOAT, P_ANIMATABLE, IDC_THICKNESS_SPIN,
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_THICKNESS, IDC_THICKNESS_SPIN, 0.0f,
	p_end,

	thickness_map, _T("thickness_map"), TYPE_TEXMAP, P_OWNERS_REF, "thickness",
	p_refno, 6,
	p_subtexno, 4,
	p_ui, TYPE_TEXMAPBUTTON, IDC_THICKNESS_MAP,
	p_end,

	p_end
	);




LR_Velvet::LR_Velvet()
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

LR_Velvet::LR_Velvet(BOOL loading)
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

LR_Velvet::~LR_Velvet()
{
	DeleteAllRefs();
}


void LR_Velvet::Reset()
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

	GetLR_VelvetDesc()->MakeAutoParamBlocks(this);
}



ParamDlg* LR_Velvet::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	IAutoMParamDlg* masterDlg = GetLR_VelvetDesc()->CreateParamDlgs(hwMtlEdit, imp, this);
	// TODO: Set param block user dialog if necessary
	return masterDlg;
	
}

BOOL LR_Velvet::SetDlgThing(ParamDlg* /*dlg*/)
{
	return FALSE;
}

Interval LR_Velvet::Validity(TimeValue t)
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

RefTargetHandle LR_Velvet::GetReference(int i)
{
	switch (i)
	{
		//case 0: return subtexture[i]; break;
	case 1: return pblock; break;
		//case 2: return subtexture[i-2]; break;
	default: return subtexture[i - 2]; break;
	}

}

void LR_Velvet::SetReference(int i, RefTargetHandle rtarg)
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

TSTR LR_Velvet::SubAnimName(int i)
{
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return GetSubTexmapTVName(i);
	else
		return GetSubTexmapTVName(i-2);
}

Animatable* LR_Velvet::SubAnim(int i)
{
	switch (i)
	{
	case 0: return subtexture[i];
	case 1: return pblock;
	default: return subtexture[i-2];
	}
}

RefResult LR_Velvet::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, 
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
				LR_Velvet_param_blk.InvalidateUI(changing_param);
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

Mtl* LR_Velvet::GetSubMtl(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	return 
		nullptr;
}

void LR_Velvet::SetSubMtl(int i, Mtl* m)
{
	//mprintf(_T("\n SetSubMtl Nubmer is : %i \n"), i);
	ReplaceReference(i , m);
	if (i == 0)
	{
		LR_Velvet_param_blk.InvalidateUI(kd_map);
		mapValid.SetEmpty();
	}
}

TSTR LR_Velvet::GetSubMtlSlotName(int i)
{
	// Return i'th sub-material name
	return submtl[i]->GetName();
	//return _T("");
}

TSTR LR_Velvet::GetSubMtlTVName(int i)
{
	return GetSubMtlSlotName(i);
}

/*===========================================================================*\
 |	Texmap get and set
\*===========================================================================*/

Texmap* LR_Velvet::GetSubTexmap(int i)
{
	//mprintf(_T("\n GetSubTexmap Nubmer ::::::::::::===>>>  is : Get %i \n"), i);
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return subtexture[i];
	return
		nullptr;
}

void LR_Velvet::SetSubTexmap(int i, Texmap* tx)
{
	//mprintf(_T("\n SetSubTexmap Nubmer ============>>>  is : %i \n"), i);
	ReplaceReference(i +2, tx);
	if (i == 0)
	{
		LR_Velvet_param_blk.InvalidateUI(kd_map);
		mapValid.SetEmpty();
	}
	if (i == 1)
		{
			LR_Velvet_param_blk.InvalidateUI(p1_map);
			mapValid.SetEmpty();
		}
	if (i == 2)
	{
		LR_Velvet_param_blk.InvalidateUI(p2_map);
		mapValid.SetEmpty();
	}
	if (i == 3)
	{
		LR_Velvet_param_blk.InvalidateUI(p3_map);
		mapValid.SetEmpty();
	}
}

TSTR LR_Velvet::GetSubTexmapSlotName(int i)
{
	switch (i)
	{
		case 0:
			return _T("Kd (color of the material's fuzz)");
		case 1:
			return _T("P1 (polynomial that influences the fuzz)");
		case 2:
			return _T("P2 (polynomial that influences the fuzz)");
		case 3:
			return _T("P3 (polynomial that influences the fuzz)");
		case 4:
			return _T("Thickness (height of the fuzz)");
		default:
			return _T("Kd");
	}
}

TSTR LR_Velvet::GetSubTexmapTVName(int i)
{
	// Return i'th sub-texture name
	return GetSubTexmapSlotName(i);
}



/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000
#define PARAM2_CHUNK 0x1010

IOResult LR_Velvet::Save(ISave* isave)
{
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK)
		return res;
	isave->EndChunk();

	return IO_OK;
}

IOResult LR_Velvet::Load(ILoad* iload)
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

RefTargetHandle LR_Velvet::Clone(RemapDir &remap)
{
	LR_Velvet *mnew = new LR_Velvet(FALSE);
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

void LR_Velvet::NotifyChanged()
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void LR_Velvet::Update(TimeValue t, Interval& valid)
{
	if (!ivalid.InInterval(t))
	{

		ivalid.SetInfinite();
		//pblock->GetValue( mtl_mat1_on, t, mapOn[0], ivalid);
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

void LR_Velvet::SetAmbient(Color /*c*/, TimeValue /*t*/) {}		
void LR_Velvet::SetDiffuse(Color /*c*/, TimeValue /*t*/) {}		
void LR_Velvet::SetSpecular(Color /*c*/, TimeValue /*t*/) {}
void LR_Velvet::SetShininess(float /*v*/, TimeValue /*t*/) {}

Color LR_Velvet::GetAmbient(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(kd, GetCOREInterface()->GetTime(), p, ivalid);
	return submtl[0] ? submtl[0]->GetAmbient(mtlNum, backFace) : Color(p.x, p.y, p.z);//Bound(Color(p.x, p.y, p.z));
}

Color LR_Velvet::GetDiffuse(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(kd, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetDiffuse(mtlNum, backFace) : Color(p.x, p.y, p.z);
}

Color LR_Velvet::GetSpecular(int mtlNum, BOOL backFace)
{
	Point3 p;
	pblock->GetValue(kd, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetSpecular(mtlNum,backFace): Color(p.x, p.y, p.z);
}

float LR_Velvet::GetXParency(int mtlNum, BOOL backFace)
{
	float t = 0.0f;
	//pblock->GetValue(pb_opacity, 0, t, ivalid);
	return submtl[0] ? submtl[0]->GetXParency(mtlNum,backFace): t;
}

float LR_Velvet::GetShininess(int mtlNum, BOOL backFace)
{
	float sh = 1.0f;
	//pblock->GetValue(pb_shin, 0, sh, ivalid);
	return submtl[0] ? submtl[0]->GetShininess(mtlNum,backFace): sh;
}

float LR_Velvet::GetShinStr(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetShinStr(mtlNum,backFace): 0.0f;
}

float LR_Velvet::WireSize(int mtlNum, BOOL backFace)
{
	float wf = 0.0f;
	//pblock->GetValue(pb_wiresize, 0, wf, ivalid);
	return submtl[0] ? submtl[0]->WireSize(mtlNum, backFace) : wf;
}


/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void LR_Velvet::Shade(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	if (gbufID)
		sc.SetGBufferID(gbufID);

	if(subMaterial)
		subMaterial->Shade(sc);
	// TODO: compute the color and transparency output returned in sc.out.
}

float LR_Velvet::EvalDisplacement(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	return (subMaterial) ? subMaterial->EvalDisplacement(sc) : 0.0f;
}

Interval LR_Velvet::DisplacementValidity(TimeValue t)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;

	Interval iv;
	iv.SetInfinite();
	if(subMaterial) 
		iv &= subMaterial->DisplacementValidity(t);

	return iv;
}


