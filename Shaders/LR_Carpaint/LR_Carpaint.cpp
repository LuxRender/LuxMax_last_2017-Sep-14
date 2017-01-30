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

#include "LR_Carpaint.h"
#include <maxscript\maxscript.h>

#define LR_Carpaint_CLASS_ID	Class_ID(0x12b48e28, 0x5de432e3)


#define NUM_SUBMATERIALS 15 // TODO: number of sub-materials supported by this plug-in
#define NUM_SUBTEXTURES 15
#define Num_REF 4
// Reference Indexes
// 
#define PBLOCK_REF 1

class LR_Carpaint : public Mtl {
public:
	LR_Carpaint();
	LR_Carpaint(BOOL loading);
	~LR_Carpaint();


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
	virtual Class_ID ClassID() {return LR_Carpaint_CLASS_ID;}
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



class LR_CarpaintClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL loading = FALSE) 		{ return new LR_Carpaint(loading); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return MATERIAL_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return LR_Carpaint_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("LR_Carpaint"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetLR_CarpaintDesc() { 
	static LR_CarpaintClassDesc LR_CarpaintDesc;
	return &LR_CarpaintDesc; 
}





enum { LR_Carpaint_params };


//TODO: Add enums for various parameters
enum 
{
	preset,
	ka,
	ka_map,
	d,
	d_map,
	kd,
	kd_map,
	ks1,
	ks1_map,
	ks2,
	ks2_map,
	ks3,
	ks3_map,
	r1,
	r1_map,
	r2,
	r2_map,
	r3,
	r3_map,
	m1,
	m1_map,
	m2,
	m2_map,
	m3,
	m3_map,
};




static ParamBlockDesc2 LR_Carpaint_param_blk (
	LR_Carpaint_params, _T("params"),  0, GetLR_CarpaintDesc(),	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params

	preset, _T("preset"), TYPE_RADIOBTN_INDEX, P_ANIMATABLE, IDS_PRESET,
	p_default, 0,
	p_range, 1, 9,
	p_ui, TYPE_RADIO, 9, IDC_PRESET1, IDC_PRESET2, IDC_PRESET3, IDC_PRESET4, IDC_PRESET5, IDC_PRESET6, IDC_PRESET7, IDC_PRESET8, IDC_PRESET9,
	p_end,

	ka, _T("ka"), TYPE_RGBA, P_ANIMATABLE, IDS_KA,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_KA,
	p_end,
	
	ka_map, _T("ka_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_KA_MAP,
	p_refno, 2,
	p_subtexno, 0,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KA_MAP,
	p_end,

	d, _T("d"), TYPE_FLOAT, P_ANIMATABLE, IDS_D,
	p_default, 0.0f,
	p_range, 0.0f, 9999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_D, IDC_D_SPIN, 0.1f,
	p_end,

	d_map, _T("d_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_D_MAP,
	p_refno, 3,
	p_subtexno, 1,
	p_ui, TYPE_TEXMAPBUTTON, IDC_D_MAP,
	p_end,

	kd, _T("kd"), TYPE_RGBA, P_ANIMATABLE, IDS_KD,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_KD,
	p_end,

	kd_map, _T("kd_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_KD_MAP,
	p_refno, 4,
	p_subtexno, 2,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KD_MAP,
	p_end,

	ks1, _T("ks1"), TYPE_RGBA, P_ANIMATABLE, IDS_KS1,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_KS1,
	p_end,

	ks1_map, _T("ks1_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_KS1_MAP,
	p_refno, 5,
	p_subtexno, 3,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KS1_MAP,
	p_end,

	ks2, _T("ks2"), TYPE_RGBA, P_ANIMATABLE, IDS_KS2,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_KS2,
	p_end,

	ks2_map, _T("ks2_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_KS2_MAP,
	p_refno, 6,
	p_subtexno, 4,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KS2_MAP,
	p_end,

	ks3, _T("ks3"), TYPE_RGBA, P_ANIMATABLE, IDS_KS3,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_KS3,
	p_end,

	ks3_map, _T("ks3_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_KS3_MAP,
	p_refno, 7,
	p_subtexno, 5,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KS3_MAP,
	p_end,

	r1, _T("r1"), TYPE_RGBA, P_ANIMATABLE, IDS_R1,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_R1,
	p_end,

	r1_map, _T("r1_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_R1_MAP,
	p_refno, 8,
	p_subtexno, 6,
	p_ui, TYPE_TEXMAPBUTTON, IDC_R1_MAP,
	p_end,

	r2, _T("r2"), TYPE_RGBA, P_ANIMATABLE, IDS_R2,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_R2,
	p_end,

	r2_map, _T("r2_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_R2_MAP,
	p_refno, 9,
	p_subtexno, 7,
	p_ui, TYPE_TEXMAPBUTTON, IDC_R2_MAP,
	p_end,

	r3, _T("r3"), TYPE_RGBA, P_ANIMATABLE, IDS_R3,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_R3,
	p_end,

	r3_map, _T("r3_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_R3_MAP,
	p_refno, 10,
	p_subtexno, 8,
	p_ui, TYPE_TEXMAPBUTTON, IDC_R3_MAP,
	p_end,

	m1, _T("m1"), TYPE_RGBA, P_ANIMATABLE, IDS_M1,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_M1,
	p_end,

	m1_map, _T("m1_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_M1_MAP,
	p_refno, 11,
	p_subtexno, 9,
	p_ui, TYPE_TEXMAPBUTTON, IDC_M1_MAP,
	p_end,

	m2, _T("m2"), TYPE_RGBA, P_ANIMATABLE, IDS_M2,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_M2,
	p_end,

	m2_map, _T("m2_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_M2_MAP,
	p_refno, 12,
	p_subtexno, 10,
	p_ui, TYPE_TEXMAPBUTTON, IDC_M2_MAP,
	p_end,

	m3, _T("m3"), TYPE_RGBA, P_ANIMATABLE, IDS_M3,
	p_default, Color(0.0f, 0.0f, 0.0f),
	p_ui, TYPE_COLORSWATCH, IDC_M3,
	p_end,

	m3_map, _T("m3_map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_M3_MAP,
	p_refno, 13,
	p_subtexno, 11,
	p_ui, TYPE_TEXMAPBUTTON, IDC_M3_MAP,
	p_end,

	p_end
	);




LR_Carpaint::LR_Carpaint()
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

LR_Carpaint::LR_Carpaint(BOOL loading)
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

LR_Carpaint::~LR_Carpaint()
{
	DeleteAllRefs();
}


void LR_Carpaint::Reset()
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

	GetLR_CarpaintDesc()->MakeAutoParamBlocks(this);
}



ParamDlg* LR_Carpaint::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	IAutoMParamDlg* masterDlg = GetLR_CarpaintDesc()->CreateParamDlgs(hwMtlEdit, imp, this);
	// TODO: Set param block user dialog if necessary
	return masterDlg;
	
}

BOOL LR_Carpaint::SetDlgThing(ParamDlg* /*dlg*/)
{
	return FALSE;
}

Interval LR_Carpaint::Validity(TimeValue t)
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

RefTargetHandle LR_Carpaint::GetReference(int i)
{
	switch (i)
	{
		//case 0: return subtexture[i]; break;
	case 1: return pblock; break;
		//case 2: return subtexture[i-2]; break;
	default: return subtexture[i - 2]; break;
	}

}

void LR_Carpaint::SetReference(int i, RefTargetHandle rtarg)
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

TSTR LR_Carpaint::SubAnimName(int i)
{
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return GetSubTexmapTVName(i);
	else
		return GetSubTexmapTVName(i-2);
}

Animatable* LR_Carpaint::SubAnim(int i)
{
	switch (i)
	{
	case 0: return subtexture[i];
	case 1: return pblock;
	default: return subtexture[i-2];
	}
}

RefResult LR_Carpaint::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, 
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
				LR_Carpaint_param_blk.InvalidateUI(changing_param);
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

Mtl* LR_Carpaint::GetSubMtl(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	return 
		nullptr;
}

void LR_Carpaint::SetSubMtl(int i, Mtl* m)
{
	//mprintf(_T("\n SetSubMtl Nubmer is : %i \n"), i);
	ReplaceReference(i , m);
	if (i == 0)
	{
		LR_Carpaint_param_blk.InvalidateUI(ka);
		mapValid.SetEmpty();
	}
}

TSTR LR_Carpaint::GetSubMtlSlotName(int i)
{
	// Return i'th sub-material name
	return submtl[i]->GetName();
	//return _T("");
}

TSTR LR_Carpaint::GetSubMtlTVName(int i)
{
	return GetSubMtlSlotName(i);
}

/*===========================================================================*\
 |	Texmap get and set
\*===========================================================================*/

Texmap* LR_Carpaint::GetSubTexmap(int i)
{
	//mprintf(_T("\n GetSubTexmap Nubmer ::::::::::::===>>>  is : Get %i \n"), i);
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return subtexture[i];
	return
		nullptr;
}

void LR_Carpaint::SetSubTexmap(int i, Texmap* tx)
{
	//mprintf(_T("\n SetSubTexmap Nubmer ============>>>  is : %i \n"), i);
	ReplaceReference(i +2, tx);
	if (i == 0)
	{
		LR_Carpaint_param_blk.InvalidateUI(ka_map);
		mapValid.SetEmpty();
	}
	if (i == 1)
	{
		LR_Carpaint_param_blk.InvalidateUI(d_map);
		mapValid.SetEmpty();
	}
	if (i == 2)
	{
		LR_Carpaint_param_blk.InvalidateUI(kd_map);
		mapValid.SetEmpty();
	}
	if (i == 3)
	{
		LR_Carpaint_param_blk.InvalidateUI(ks1_map);
		mapValid.SetEmpty();
	}
	if (i == 4)
	{
		LR_Carpaint_param_blk.InvalidateUI(ks2_map);
		mapValid.SetEmpty();
	}
	if (i == 5)
	{
		LR_Carpaint_param_blk.InvalidateUI(ks3_map);
		mapValid.SetEmpty();
	}
	if (i == 6)
	{
		LR_Carpaint_param_blk.InvalidateUI(r1_map);
		mapValid.SetEmpty();
	}
	if (i == 7)
	{
		LR_Carpaint_param_blk.InvalidateUI(r2_map);
		mapValid.SetEmpty();
	}
	if (i == 8)
	{
		LR_Carpaint_param_blk.InvalidateUI(r3_map);
		mapValid.SetEmpty();
	}
	if (i == 9)
	{
		LR_Carpaint_param_blk.InvalidateUI(m1_map);
		mapValid.SetEmpty();
	}
	if (i == 10)
	{
		LR_Carpaint_param_blk.InvalidateUI(m2_map);
		mapValid.SetEmpty();
	}
	if (i == 11)
	{
		LR_Carpaint_param_blk.InvalidateUI(m3_map);
		mapValid.SetEmpty();
	}

}

TSTR LR_Carpaint::GetSubTexmapSlotName(int i)
{
	switch (i)
	{
		case 0:
			return _T("ka_map");
		case 1:
			return _T("d_map");
		case 2:
			return _T("kd_map");
		case 3:
			return _T("ks1_map");
		case 4:
			return _T("ks2_map");
		case 5:
			return _T("ks3_map");
		case 6:
			return _T("r1_map");
		case 7:
			return _T("r2_map");
		case 8:
			return _T("r3_map");
		case 9:
			return _T("m1_map");
		case 10:
			return _T("m2_map");
		case 11:
			return _T("m3_map");
		default:
			return _T("");
	}
}

TSTR LR_Carpaint::GetSubTexmapTVName(int i)
{
	// Return i'th sub-texture name
	return GetSubTexmapSlotName(i);
}



/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000
#define PARAM2_CHUNK 0x1010

IOResult LR_Carpaint::Save(ISave* isave)
{
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK)
		return res;
	isave->EndChunk();

	return IO_OK;
}

IOResult LR_Carpaint::Load(ILoad* iload)
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

RefTargetHandle LR_Carpaint::Clone(RemapDir &remap)
{
	LR_Carpaint *mnew = new LR_Carpaint(FALSE);
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

void LR_Carpaint::NotifyChanged()
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void LR_Carpaint::Update(TimeValue t, Interval& valid)
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

void LR_Carpaint::SetAmbient(Color /*c*/, TimeValue /*t*/) {}		
void LR_Carpaint::SetDiffuse(Color /*c*/, TimeValue /*t*/) {}		
void LR_Carpaint::SetSpecular(Color /*c*/, TimeValue /*t*/) {}
void LR_Carpaint::SetShininess(float /*v*/, TimeValue /*t*/) {}

Color LR_Carpaint::GetAmbient(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(ka, GetCOREInterface()->GetTime(), p, ivalid);
	return submtl[0] ? submtl[0]->GetAmbient(mtlNum, backFace) : Color(p.x, p.y, p.z);//Bound(Color(p.x, p.y, p.z));
}

Color LR_Carpaint::GetDiffuse(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(ka, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetDiffuse(mtlNum, backFace) : Color(p.x, p.y, p.z);
}

Color LR_Carpaint::GetSpecular(int mtlNum, BOOL backFace)
{
	Point3 p;
	pblock->GetValue(ka, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetSpecular(mtlNum,backFace): Color(p.x, p.y, p.z);
}

float LR_Carpaint::GetXParency(int mtlNum, BOOL backFace)
{
	float t = 0.0f;
	//pblock->GetValue(pb_opacity, 0, t, ivalid);
	return submtl[0] ? submtl[0]->GetXParency(mtlNum,backFace): t;
}

float LR_Carpaint::GetShininess(int mtlNum, BOOL backFace)
{
	float sh = 1.0f;
	//pblock->GetValue(pb_shin, 0, sh, ivalid);
	return submtl[0] ? submtl[0]->GetShininess(mtlNum,backFace): sh;
}

float LR_Carpaint::GetShinStr(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetShinStr(mtlNum,backFace): 0.0f;
}

float LR_Carpaint::WireSize(int mtlNum, BOOL backFace)
{
	float wf = 0.0f;
	//pblock->GetValue(pb_wiresize, 0, wf, ivalid);
	return submtl[0] ? submtl[0]->WireSize(mtlNum, backFace) : wf;
}


/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void LR_Carpaint::Shade(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	if (gbufID)
		sc.SetGBufferID(gbufID);

	if(subMaterial)
		subMaterial->Shade(sc);
	// TODO: compute the color and transparency output returned in sc.out.
}

float LR_Carpaint::EvalDisplacement(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	return (subMaterial) ? subMaterial->EvalDisplacement(sc) : 0.0f;
}

Interval LR_Carpaint::DisplacementValidity(TimeValue t)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;

	Interval iv;
	iv.SetInfinite();
	if(subMaterial) 
		iv &= subMaterial->DisplacementValidity(t);

	return iv;
}


