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

#include "LR_GlossyTranslucent.h"
#include <maxscript\maxscript.h>

#define LR_GlossyTranslucent_CLASS_ID	Class_ID(0x24b19e11, 0x1de467e3)


#define NUM_SUBMATERIALS 14 // TODO: number of sub-materials supported by this plug-in
#define NUM_SUBTEXTURES 14
#define Num_REF 4
// Reference Indexes
// 
#define PBLOCK_REF 1

class LR_GlossyTranslucent : public Mtl {
public:
	LR_GlossyTranslucent();
	LR_GlossyTranslucent(BOOL loading);
	~LR_GlossyTranslucent();


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
	virtual Class_ID ClassID() {return LR_GlossyTranslucent_CLASS_ID;}
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



class LR_GlossyTranslucentClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL loading = FALSE) 		{ return new LR_GlossyTranslucent(loading); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return MATERIAL_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return LR_GlossyTranslucent_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("LR_GlossyTranslucent"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetLR_GlossyTranslucentDesc() { 
	static LR_GlossyTranslucentClassDesc LR_GlossyTranslucentDesc;
	return &LR_GlossyTranslucentDesc; 
}





enum { LR_GlossyTranslucent_params };


//TODO: Add enums for various parameters
enum 
{
	kd, /* color */
	kd_map, /* texture */
	kt, /* color*/
	kt_map, /* texture */
	ks, /*color */
	ks_map, /*texture*/
	ks_bf, /*color */
	ks_bf_map, /*texture*/
	uroughness, /*float*/
	uroughness_map, /*texture*/
	uroughness_bf, /*float*/
	uroughness_bf_map, /*texture*/
	vroughness,/*float*/
	vroughness_map, /*texture*/
	vroughness_bf, /*float*/
	vroughness_bf_map, /*texture*/
	ka,
	ka_map,
	ka_bf,
	ka_bf_map,
	d,
	d_map,
	d_bf,
	d_bf_map,
	index,
	index_map,
	index_bf,
	index_bf_map,
	multibounce,
	multibounce_bf,
};




static ParamBlockDesc2 LR_GlossyTranslucent_param_blk (
	LR_GlossyTranslucent_params, _T("params"),  0, GetLR_GlossyTranslucentDesc(),	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
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

	kt, _T("kt"), TYPE_RGBA, P_ANIMATABLE, "kt",
	p_default, Color(0.5f, 0.5f, 0.5f),
	p_ui, TYPE_COLORSWATCH, IDC_KT_COLOR,
	p_end,

	kt_map, _T("kt_map"), TYPE_TEXMAP, P_OWNERS_REF, "kt_map",
	p_refno, 3,
	p_subtexno, 1,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KT_MAP,
	p_end,

	ks, _T("ks"), TYPE_RGBA, P_ANIMATABLE, "ks",
	p_default, Color(0.5f, 0.5f, 0.5f),
	p_ui, TYPE_COLORSWATCH, IDC_KS_COLOR,
	p_end,

	ks_map, _T("ks_map"), TYPE_TEXMAP, P_OWNERS_REF, "ks_map",
	p_refno, 4,
	p_subtexno, 2,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KS_MAP,
	p_end,

	ks_bf, _T("ks_bf"), TYPE_RGBA, P_ANIMATABLE, "ks_bf",
	p_default, Color(0.5f, 0.5f, 0.5f),
	p_ui, TYPE_COLORSWATCH, IDC_KS_BF_COLOR,
	p_end,

	ks_bf_map, _T("ks_bf_map"), TYPE_TEXMAP, P_OWNERS_REF, "ks_bf_map",
	p_refno, 5,
	p_subtexno, 3,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KS_BF_MAP,
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

	uroughness_bf, _T("uroughness_bf"), TYPE_FLOAT, P_ANIMATABLE, "uroughness_bf",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_UROUGHNESS_BF, IDC_UROUGHNESS_BF_SPIN, 0.0f,
	p_end,

	uroughness_bf_map, _T("uroughness_bf_map"), TYPE_TEXMAP, P_OWNERS_REF, "uroughness_bf_map",
	p_refno, 7,
	p_subtexno, 5,
	p_ui, TYPE_TEXMAPBUTTON, IDC_UROUGHNESS_BF_MAP,
	p_end,

	vroughness, _T("vuroughness"), TYPE_FLOAT, P_ANIMATABLE, "vroughness",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_VROUGHNESS, IDC_VROUGHNESS_SPIN, 0.0f,
	p_end,

	vroughness_map, _T("vroughness_map"), TYPE_TEXMAP, P_OWNERS_REF, "vroughness_map",
	p_refno, 8,
	p_subtexno, 6,
	p_ui, TYPE_TEXMAPBUTTON, IDC_VROUGHNESS_MAP,
	p_end,

	vroughness_bf, _T("vroughness_bf"), TYPE_FLOAT, P_ANIMATABLE, "vroughness_bf",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_VROUGHNESS_BF, IDC_VROUGHNESS_BF_SPIN, 0.0f,
	p_end,

	vroughness_bf_map, _T("vroughness_bf_map"), TYPE_TEXMAP, P_OWNERS_REF, "vroughness_bf_map",
	p_refno, 9,
	p_subtexno, 7,
	p_ui, TYPE_TEXMAPBUTTON, IDC_VROUGHNESS_BF_MAP,
	p_end,
	
	ka, _T("ka"), TYPE_FLOAT, P_ANIMATABLE, "ka",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_KA, IDC_KA_SPIN, 0.0f,
	p_end,

	ka_map, _T("ka_map"), TYPE_TEXMAP, P_OWNERS_REF, "ka_map",
	p_refno, 10,
	p_subtexno, 8,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KA_MAP,
	p_end,

	ka_bf, _T("ka_bf"), TYPE_FLOAT, P_ANIMATABLE, "ka_bf",
	p_default, 0.1f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_KA_BF, IDC_KA_BF_SPIN, 0.0f,
	p_end,

	ka_bf_map, _T("ka_bf_map"), TYPE_TEXMAP, P_OWNERS_REF, "ka_bf_map",
	p_refno, 11,
	p_subtexno, 9,
	p_ui, TYPE_TEXMAPBUTTON, IDC_KA_BF_MAP,
	p_end,

	d, _T("d"), TYPE_FLOAT, P_ANIMATABLE, "d",
	p_default, 0.0f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_D, IDC_D_SPIN, 0.0f,
	p_end,

	d_map, _T("d_map"), TYPE_TEXMAP, P_OWNERS_REF, "d_map",
	p_refno, 12,
	p_subtexno, 10,
	p_ui, TYPE_TEXMAPBUTTON, IDC_D_MAP,
	p_end,
	
	d_bf, _T("d_bf"), TYPE_FLOAT, P_ANIMATABLE, "d_bf",
	p_default, 0.0f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_D_BF, IDC_D_BF_SPIN, 0.0f,
	p_end,

	d_bf_map, _T("d_bf_map"), TYPE_TEXMAP, P_OWNERS_REF, "d_bf_map",
	p_refno, 13,
	p_subtexno, 11,
	p_ui, TYPE_TEXMAPBUTTON, IDC_D_BF_MAP,
	p_end,
	
	index, _T("index"), TYPE_FLOAT, P_ANIMATABLE, "index",
	p_default, 0.0f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_INDEX, IDC_INDEX_SPIN, 0.0f,
	p_end,

	index_map, _T("index_map"), TYPE_TEXMAP, P_OWNERS_REF, "index_map",
	p_refno, 14,
	p_subtexno, 12,
	p_ui, TYPE_TEXMAPBUTTON, IDC_INDEX_MAP,
	p_end,

	index_bf, _T("index_bf"), TYPE_FLOAT, P_ANIMATABLE, "index_bf",
	p_default, 0.0f,
	p_range, 0.0f, 999.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_INDEX_BF, IDC_INDEX_BF_SPIN, 0.0f,
	p_end,

	index_bf_map, _T("index_bf_map"), TYPE_TEXMAP, P_OWNERS_REF, "index_bf_map",
	p_refno, 15,
	p_subtexno, 13,
	p_ui, TYPE_TEXMAPBUTTON, IDC_INDEX_BF_MAP,
	p_end,
	
	multibounce, _T("multibounce"), TYPE_BOOL, 0, IDC_MULTIBOUNCE,
	p_default, FALSE,
	p_ui, TYPE_SINGLECHEKBOX, IDC_MULTIBOUNCE,
	p_end,

	multibounce_bf, _T("multibounce_bf"), TYPE_BOOL, 0, IDC_MULTIBOUNCE_BF,
	p_default, FALSE,
	p_ui, TYPE_SINGLECHEKBOX, IDC_MULTIBOUNCE_BF,
	p_end,

	p_end
	);




LR_GlossyTranslucent::LR_GlossyTranslucent()
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

LR_GlossyTranslucent::LR_GlossyTranslucent(BOOL loading)
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

LR_GlossyTranslucent::~LR_GlossyTranslucent()
{
	DeleteAllRefs();
}


void LR_GlossyTranslucent::Reset()
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

	GetLR_GlossyTranslucentDesc()->MakeAutoParamBlocks(this);
}



ParamDlg* LR_GlossyTranslucent::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	IAutoMParamDlg* masterDlg = GetLR_GlossyTranslucentDesc()->CreateParamDlgs(hwMtlEdit, imp, this);
	// TODO: Set param block user dialog if necessary
	return masterDlg;
	
}

BOOL LR_GlossyTranslucent::SetDlgThing(ParamDlg* /*dlg*/)
{
	return FALSE;
}

Interval LR_GlossyTranslucent::Validity(TimeValue t)
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

RefTargetHandle LR_GlossyTranslucent::GetReference(int i)
{
	switch (i)
	{
		//case 0: return subtexture[i]; break;
	case 1: return pblock; break;
		//case 2: return subtexture[i-2]; break;
	default: return subtexture[i - 2]; break;
	}

}

void LR_GlossyTranslucent::SetReference(int i, RefTargetHandle rtarg)
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

TSTR LR_GlossyTranslucent::SubAnimName(int i)
{
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return GetSubTexmapTVName(i);
	else
		return GetSubTexmapTVName(i-2);
}

Animatable* LR_GlossyTranslucent::SubAnim(int i)
{
	switch (i)
	{
	case 0: return subtexture[i];
	case 1: return pblock;
	default: return subtexture[i-2];
	}
}

RefResult LR_GlossyTranslucent::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, 
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
				LR_GlossyTranslucent_param_blk.InvalidateUI(changing_param);
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

Mtl* LR_GlossyTranslucent::GetSubMtl(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	return 
		nullptr;
}

void LR_GlossyTranslucent::SetSubMtl(int i, Mtl* m)
{
	//mprintf(_T("\n SetSubMtl Nubmer is : %i \n"), i);
	ReplaceReference(i , m);
	if (i == 0)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(kd);
		mapValid.SetEmpty();
	}
}

TSTR LR_GlossyTranslucent::GetSubMtlSlotName(int i)
{
	// Return i'th sub-material name
	return submtl[i]->GetName();
	//return _T("");
}

TSTR LR_GlossyTranslucent::GetSubMtlTVName(int i)
{
	return GetSubMtlSlotName(i);
}

/*===========================================================================*\
 |	Texmap get and set
\*===========================================================================*/

Texmap* LR_GlossyTranslucent::GetSubTexmap(int i)
{
	//mprintf(_T("\n GetSubTexmap Nubmer ::::::::::::===>>>  is : Get %i \n"), i);
	if ((i >= 0) && (i < NUM_SUBTEXTURES))
		return subtexture[i];
	return
		nullptr;
}

void LR_GlossyTranslucent::SetSubTexmap(int i, Texmap* tx)
{
	//mprintf(_T("\n SetSubTexmap Nubmer ============>>>  is : %i \n"), i);
	ReplaceReference(i +2, tx);
	if (i == 0)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(kd_map);
		mapValid.SetEmpty();
	}
	if (i == 1)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(kt_map);
		mapValid.SetEmpty();
	}
	if (i == 2)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(ks_map);
		mapValid.SetEmpty();
	}
	if (i == 3)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(ks_bf_map);
		mapValid.SetEmpty();
	}
	if (i == 4)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(uroughness_map);
		mapValid.SetEmpty();
	}
	if (i == 5)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(uroughness_bf_map);
		mapValid.SetEmpty();
	}
	if (i == 6)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(vroughness_map);
		mapValid.SetEmpty();
	}
	if (i == 7)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(vroughness_bf_map);
		mapValid.SetEmpty();
	}
	if (i == 8)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(ka_map);
		mapValid.SetEmpty();
	}
	if (i == 9)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(ka_bf_map);
		mapValid.SetEmpty();
	}
	if (i == 10)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(d_map);
		mapValid.SetEmpty();
	}
	if (i == 11)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(d_bf_map);
		mapValid.SetEmpty();
	}
	if (i == 12)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(index_map);
		mapValid.SetEmpty();
	}
	if (i == 13)
	{
		LR_GlossyTranslucent_param_blk.InvalidateUI(index_bf_map);
		mapValid.SetEmpty();
	}

	//mapValid.SetEmpty();
}

TSTR LR_GlossyTranslucent::GetSubTexmapSlotName(int i)
{
	switch (i)
	{
	default:
		return _T("kd");

		//case 0:
		//	return _T("kd");
		case 1:
			return _T("kt");
		case 2:
			return _T("ks");
		case 3:
			return _T("ks_bf");
		case 4:
			return _T("uroughness");
		case 5:
			return _T("uroughness_bf");
		case 6:
			return _T("vroughness");
		case 7:
			return _T("vroughness_bf");
		case 8:
			return _T("ka");
		case 9:
			return _T("ka_bf");
		case 10:
			return _T("d");
		case 11:
			return _T("d_bf");
		case 12:
			return _T("index");
		case 13:
			return _T("index_bf");
		
	}
}

TSTR LR_GlossyTranslucent::GetSubTexmapTVName(int i)
{
	// Return i'th sub-texture name
	return GetSubTexmapSlotName(i);
}



/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000
#define PARAM2_CHUNK 0x1010

IOResult LR_GlossyTranslucent::Save(ISave* isave)
{
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK)
		return res;
	isave->EndChunk();

	return IO_OK;
}

IOResult LR_GlossyTranslucent::Load(ILoad* iload)
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

RefTargetHandle LR_GlossyTranslucent::Clone(RemapDir &remap)
{
	LR_GlossyTranslucent *mnew = new LR_GlossyTranslucent(FALSE);
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

void LR_GlossyTranslucent::NotifyChanged()
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void LR_GlossyTranslucent::Update(TimeValue t, Interval& valid)
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

void LR_GlossyTranslucent::SetAmbient(Color /*c*/, TimeValue /*t*/) {}		
void LR_GlossyTranslucent::SetDiffuse(Color /*c*/, TimeValue /*t*/) {}		
void LR_GlossyTranslucent::SetSpecular(Color /*c*/, TimeValue /*t*/) {}
void LR_GlossyTranslucent::SetShininess(float /*v*/, TimeValue /*t*/) {}

Color LR_GlossyTranslucent::GetAmbient(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(kd, GetCOREInterface()->GetTime(), p, ivalid);
	return submtl[0] ? submtl[0]->GetAmbient(mtlNum, backFace) : Color(p.x, p.y, p.z);//Bound(Color(p.x, p.y, p.z));
}

Color LR_GlossyTranslucent::GetDiffuse(int mtlNum, BOOL backFace)
{
	Point3 p;
	//TimeValue t; //Zero for first frame //GetCOREInterface()->GetTime() for every frame
	pblock->GetValue(kd, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetDiffuse(mtlNum, backFace) : Color(p.x, p.y, p.z);
}

Color LR_GlossyTranslucent::GetSpecular(int mtlNum, BOOL backFace)
{
	Point3 p;
	pblock->GetValue(kd, 0, p, ivalid);
	return submtl[0] ? submtl[0]->GetSpecular(mtlNum,backFace): Color(p.x, p.y, p.z);
}

float LR_GlossyTranslucent::GetXParency(int mtlNum, BOOL backFace)
{
	float t = 0.0f;
	//pblock->GetValue(pb_opacity, 0, t, ivalid);
	return submtl[0] ? submtl[0]->GetXParency(mtlNum,backFace): t;
}

float LR_GlossyTranslucent::GetShininess(int mtlNum, BOOL backFace)
{
	float sh = 1.0f;
	//pblock->GetValue(pb_shin, 0, sh, ivalid);
	return submtl[0] ? submtl[0]->GetShininess(mtlNum,backFace): sh;
}

float LR_GlossyTranslucent::GetShinStr(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetShinStr(mtlNum,backFace): 0.0f;
}

float LR_GlossyTranslucent::WireSize(int mtlNum, BOOL backFace)
{
	float wf = 0.0f;
	//pblock->GetValue(pb_wiresize, 0, wf, ivalid);
	return submtl[0] ? submtl[0]->WireSize(mtlNum, backFace) : wf;
}


/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void LR_GlossyTranslucent::Shade(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	if (gbufID)
		sc.SetGBufferID(gbufID);

	if(subMaterial)
		subMaterial->Shade(sc);
	// TODO: compute the color and transparency output returned in sc.out.
}

float LR_GlossyTranslucent::EvalDisplacement(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	return (subMaterial) ? subMaterial->EvalDisplacement(sc) : 0.0f;
}

Interval LR_GlossyTranslucent::DisplacementValidity(TimeValue t)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;

	Interval iv;
	iv.SetInfinite();
	if(subMaterial) 
		iv &= subMaterial->DisplacementValidity(t);

	return iv;
}


