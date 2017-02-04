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

#include <iostream>
#include "LuxMaxpch.h"
#include "resource.h"
#include "LuxMax.h"
#include <maxscript\maxscript.h>
#include "3dsmaxport.h"
#include <sstream>
#include <string>

extern HINSTANCE hInstance;
static INT_PTR CALLBACK LuxMaxParamDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class LuxMaxParamDlg : public RendParamDlg {
public:
	LuxMax *rend;
	IRendParams *ir;
	HWND hPanel;
	//HWND hDlg;
	BOOL prog;
	HFONT hFont;
	
	TSTR workFileName;
	TSTR vbintervalWstr;
	//int workRenderType;
	int halttime;
	TSTR halttimewstr;// = L"30";
	//float LensRadius;
	TSTR LensRadiusWstr = L"33";
	float LensRadiusFloatTmp = 0.0f;
	int  rendertype;
	int samplerIndex;
	TSTR rendertypeWstr;

	int filterIndex;
	int lightStrategyIndex;
	float filterXvalue;
	float filterYvalue;
	float filterGuassianAlphavalue;
	float filterMitchellAvalue;
	float filterMitchellBvalue;
	float MetropolisLargestEpRatevalue;
	int MetropolisMaxConsecutiveRejectvalue;
	float MetrolpolisImageMutationRatevalue;
	//TSTR vbinterval = L"1";
	bool defaultlightchk = true;
	bool defaultlightauto = true;
	ISpinnerControl *depthSpinner = NULL;
	ISpinnerControl *filterXSpinner = NULL;
	ISpinnerControl *filterYSpinner = NULL;
	ISpinnerControl *filterGuassianAlphaSpinner = NULL;
	ISpinnerControl *filterMitchellASpinner = NULL;
	ISpinnerControl *filterMitchellBSpinner = NULL;
	ISpinnerControl* metropolisLargestEpRateSpinner = NULL;
	ISpinnerControl* metropolisMaxConsecutiveRejectSpinner = NULL;
	ISpinnerControl* metropolisImageMutationRateSpinner = NULL;

	LuxMaxParamDlg(LuxMax *r, IRendParams *i, BOOL prog);
	~LuxMaxParamDlg();
	void AcceptParams();
	void DeleteThis() { delete this; }
	void InitParamDialog(HWND hWnd);
	void InitProgDialog(HWND hWnd);
	void InitDepthDialog(HWND hWnd);
	void InitFilterDialog(HWND hWnd);
	void InitSamplerDialog(HWND hWnd);
	void ReleaseControls() {}
	BOOL FileBrowse();

	INT_PTR WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

LuxMaxParamDlg::~LuxMaxParamDlg()
{
	DeleteObject(hFont);
	ir->DeleteRollupPage(hPanel);
}

INT_PTR LuxMaxParamDlg::WndProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	LuxMaxParamDlg *dlg = DLGetWindowLongPtr<LuxMaxParamDlg*>(hWnd);

	switch (msg) {
	case WM_INITDIALOG:
		dlg = (LuxMaxParamDlg*)lParam;
		DLSetWindowLongPtr(hWnd, lParam);
		if (dlg)
		{
			// Enable this to get a dialog at the render progress screen.
			// Currently disabled because the dialog has no functionality yet. - no reason to show it.
			//if (dlg->prog)
				//dlg->InitProgDialog(hWnd);
			//else

				dlg->InitFilterDialog(hWnd);
				dlg->InitParamDialog(hWnd);
			//init the depth tab gui
				dlg->InitSamplerDialog(hWnd);
				dlg->InitDepthDialog(hWnd);
				
		}
		break;

	case WM_DESTROY:
		if (!dlg->prog)
		{
			//ReleaseISpinner(dlg->depthSpinner);
			ReleaseControls();
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

void showHideSamplerGUI(HWND hWnd, int index)
{
	ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LARGEST_EP_RATE_NEW), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_MAX_CONSECUTIVE_REJECT_NEW), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_IMAGE_MUTATION_RATE_NEW), SW_HIDE);

	ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LARGEST_EP_RATE_SPIN_NEW), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_MAX_CONSECUTIVE_REJECT_SPIN_NEW), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_IMAGE_MUTATION_RATE_SPIN_NEW), SW_HIDE);

	ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LABEL1), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LABEL2), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LABEL3), SW_HIDE);

	if (index == 0)
	{
		ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LARGEST_EP_RATE_NEW), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_MAX_CONSECUTIVE_REJECT_NEW), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_IMAGE_MUTATION_RATE_NEW), SW_SHOW);

		ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LARGEST_EP_RATE_SPIN_NEW), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_MAX_CONSECUTIVE_REJECT_SPIN_NEW), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_IMAGE_MUTATION_RATE_SPIN_NEW), SW_SHOW);

		ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LABEL1), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LABEL2), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_METROPOLIS_LABEL3), SW_SHOW);
	}


}

void showHideFilterGUI(HWND hWnd, int index)
{

	//HIDE ALL ELEMENTS
	//ShowWindow(GetDlgItem(hWnd, IDC_FILTERXWIDTH), SW_HIDE);
	//ShowWindow(GetDlgItem(hWnd, IDC_FILTERXWIDTH_SPIN), SW_HIDE);
	//ShowWindow(GetDlgItem(hWnd, IDC_FILTERYWIDTH), SW_HIDE);
	//ShowWindow(GetDlgItem(hWnd, IDC_FILTERYWIDTH_SPIN), SW_HIDE);
	
	//ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_X_WIDTH_LABEL), SW_HIDE);
	//ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_Y_WIDTH_LABEL), SW_HIDE);
	//ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_REC_RANGE1), SW_HIDE);
	//ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_REC_RANGE2), SW_HIDE);

	ShowWindow(GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA_SPIN), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_GUASSIAN_FILTER_ALPHA), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_REC_GUASSIAN_RANGE), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_MITCHEL_FRAME), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_A_LABEL), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_B_LABEL), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_A_FLOAT), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_B_FLOAT), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_A_FLOAT_SPIN), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_B_FLOAT_SPIN), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_A_RANGE), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_B_RANGE), SW_HIDE);

	if (index == 0)
	{
		//blackman harris has no settings
	}
	if (index == 1)
	{
		
	}
	else if (index == 2)
	{
		ShowWindow(GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA_SPIN), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_REC_GUASSIAN_RANGE), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_GUASSIAN_FILTER_ALPHA), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_MITCHEL_FRAME), SW_SHOW);
	}
	else if (index == 3 || index == 4)
	{
		//mitchell // Mitchell SS
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_MITCHEL_FRAME), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_A_LABEL), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_B_LABEL), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_A_FLOAT), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_B_FLOAT), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_A_FLOAT_SPIN), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_B_FLOAT_SPIN), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_A_RANGE), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FILTER_MITCHELL_B_RANGE), SW_SHOW);
	}
	if (index == 5)
	{
		//Box filter has no separate settings.
	}

}

static INT_PTR CALLBACK LuxMaxParamDlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//LuxMaxParamDlg *info = DLGetWindowLongPtr<LuxMaxParamDlg*>(hWnd);

	DisableAccelerators();
	LuxMaxParamDlg *dlg = DLGetWindowLongPtr<LuxMaxParamDlg*>(hWnd);
	switch (msg) 
	{
	case WM_INITDIALOG:
	{
		dlg = (LuxMaxParamDlg*)lParam;
		DLSetWindowLongPtr(hWnd, lParam);
		
		SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"BIASPATHCPU");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"BIASPATHOCL");
		SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"BIDIRCPU");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"BIDIRHYBRID");
		SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"BIDIRVMCPU");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"CBIDIRHYBRID");
		SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"LIGHTCPU");
		SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"PATHCPU");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"PATHHYBRID");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"PATHOCL");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"PATHOCLBASE");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"RTBIASPATHOCL");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_ADDSTRING, 0, (LPARAM)L"RTPATHOCL");
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_SELECTSTRING, 0, (LPARAM)L"PATHCPU");
		
		//SendDlgItemMessage(hWnd, IDC_RENDERTYPE_NEW, CB_SETCURSEL, 0, (LPARAM)L"PATHCPU");
		//store value back into workRenderType = rend->renderType
		SendDlgItemMessage(hWnd, IDC_SAMPLER_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Random");
		SendDlgItemMessage(hWnd, IDC_SAMPLER_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Sobol");
		SendDlgItemMessage(hWnd, IDC_SAMPLER_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Metropolis");
		SendDlgItemMessage(hWnd, IDC_SAMPLER_TYPE_COMBO, CB_SELECTSTRING, 0, (LPARAM)L"Sobol");

		//Add filters to the dropdown.
		SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"None");
		SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Blackman Harris");
		SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Box");
		SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Gaussian");
		SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Mitchell");
		SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_ADDSTRING, 0, (LPARAM)L"Mitchell ss");
		//SendDlgItemMessage(hWnd, IDC_FILTERS_TYPE_COMBO, CB_SELECTSTRING, 0, (LPARAM)L"Box");
		
		//Light strategy dropdown
		SendDlgItemMessage(hWnd, IDC_COMBO_LIGHT_STRATEGY, CB_ADDSTRING, 0, (LPARAM)L"UNIFORM");
		SendDlgItemMessage(hWnd, IDC_COMBO_LIGHT_STRATEGY, CB_ADDSTRING, 0, (LPARAM)L"POWER");
		SendDlgItemMessage(hWnd, IDC_COMBO_LIGHT_STRATEGY, CB_ADDSTRING, 0, (LPARAM)L"LOG_POWER");


		SendDlgItemMessage(hWnd, IDC_COMBO_FILM_OUTPUT_TYPE, CB_ADDSTRING, 0, (LPARAM)L"RGBA_TONEMAPPED");
		SendDlgItemMessage(hWnd, IDC_COMBO_FILM_OUTPUT_TYPE, CB_ADDSTRING, 0, (LPARAM)L"RGB_TONEMAPPED");
		SendDlgItemMessage(hWnd, IDC_COMBO_FILM_OUTPUT_TYPE, CB_SELECTSTRING, 0, (LPARAM)L"RGBA_TONEMAPPED");
		
		CheckDlgButton(hWnd, IDC_CHECK_OPENCL_GPU, BST_UNCHECKED);
		CheckDlgButton(hWnd, IDC_CHECK_OPENCL_CPU, BST_CHECKED);
		//CheckDlgButton(hWnd, IDC_OUTPUTSCENE, BST_UNCHECKED);

		CheckDlgButton(hWnd, IDC_CHECK_DEFAULT_LIGHT, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_CHECK_DEFUALT_LIGHT_DISABLE, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_CHECK_OVERRIDE_MATTERIALS, BST_UNCHECKED);

		break;
	}
	case WM_LBUTTONDOWN:
	{}
	case WM_MOUSEMOVE:
	{}
	case WM_LBUTTONUP:
	{
			dlg->ir->RollupMouseMessage(hWnd, msg, wParam, lParam);
			break;
	}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON1:
			{
				if (dlg->FileBrowse()) 
				{
					SetDlgItemText(hWnd, IDC_FILENAME, dlg->workFileName.data());
					break;
				}
				break;
			}
			case IDC_HALTTIME:
			{
				HWND hwndOutput = GetDlgItem(hWnd, IDC_HALTTIME);
				dlg->halttimewstr = GetWindowText(hwndOutput);
				break;
			}
			case IDC_VBINTERVAL:
			{
				HWND hwndOutput = GetDlgItem(hWnd, IDC_VBINTERVAL);
				dlg->vbintervalWstr = GetWindowText(hwndOutput);
				break;
			}

			case IDC_FILTERS_TYPE_COMBO:
			{
				switch (HIWORD(wParam))
				{
					case CBN_SELCHANGE:
					{
						HWND filterCombo = GetDlgItem(hWnd, IDC_FILTERS_TYPE_COMBO);
						dlg->filterIndex = ComboBox_GetCurSel(filterCombo);
						//mprintf(_T("\n Selected filter index %i \n"), dlg->filterIndex);
						showHideFilterGUI(hWnd, dlg->filterIndex);
						SetFocus(hWnd);
						break;
					}
					
				}
				break;
			}

			case IDC_COMBO_LIGHT_STRATEGY:
			{
				switch (HIWORD(wParam))
				{
					case CBN_SELCHANGE:
					{
						HWND lightStrategyCombo = GetDlgItem(hWnd, IDC_COMBO_LIGHT_STRATEGY);
						dlg->lightStrategyIndex = ComboBox_GetCurSel(lightStrategyCombo);
					
						SetFocus(hWnd);
						break;
					}

				}
				break;
			}

			case IDC_RENDERTYPE_NEW:
			{
				switch (HIWORD(wParam))
				{
					case CBN_SELCHANGE:
					{
						HWND comboCtl = GetDlgItem(hWnd, IDC_RENDERTYPE_NEW);
						dlg->rendertype = ComboBox_GetCurSel(comboCtl);
						SetFocus(hWnd);
						break;
					}
				}
				break;

			case IDC_SAMPLER_TYPE_COMBO:
			{
				switch (HIWORD(wParam))
				{
					case CBN_SELCHANGE:
					{
						HWND comboCtl = GetDlgItem(hWnd, IDC_SAMPLER_TYPE_COMBO);
						dlg->samplerIndex = ComboBox_GetCurSel(comboCtl);
						showHideSamplerGUI(hWnd, dlg->samplerIndex);
						SetFocus(hWnd);
						break;
					}
				}
			}
			}
			case IDC_CHECK_DEFAULT_LIGHT:
			{
				dlg->defaultlightchk = (GetCheckBox(hWnd, IDC_CHECK_DEFAULT_LIGHT) != 0);
				break;
			}
			case IDC_CHECK_DEFUALT_LIGHT_DISABLE:
			{
				dlg->defaultlightauto = (GetCheckBox(hWnd, IDC_CHECK_DEFUALT_LIGHT_DISABLE) != 0);
				break;
			}
		}

		}

		case CC_SPINNER_CHANGE:
			switch (LOWORD(wParam)) { // Switch on ID
				case IDC_LENSRADIUS_SPIN:
				{
					dlg->LensRadiusFloatTmp = ((ISpinnerControl *)lParam)->GetFVal();
					break;
				}
				case IDC_FILTERXWIDTH_SPIN:
				{
					dlg->filterXvalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
				case IDC_FILTERYWIDTH_SPIN:
				{
					dlg->filterYvalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
				case IDC_FILTER_GUASSIAN_ALPHA_SPIN:
				{
					dlg->filterGuassianAlphavalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
				case IDC_FILTER_MITCHEL_A_FLOAT_SPIN:
				{
					dlg->filterMitchellAvalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
				case IDC_FILTER_MITCHEL_B_FLOAT_SPIN:
				{
					dlg->filterMitchellBvalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
				case IDC_METROPOLIS_LARGEST_EP_RATE_SPIN_NEW:
				{
					dlg->MetropolisLargestEpRatevalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
				case IDC_MAX_CONSECUTIVE_REJECT_SPIN_NEW:
				{
					dlg->MetropolisMaxConsecutiveRejectvalue = ((ISpinnerControl*)lParam)->GetIVal();
					break;
				}
				case IDC_IMAGE_MUTATION_RATE_SPIN_NEW:
				{
					dlg->MetrolpolisImageMutationRatevalue = ((ISpinnerControl*)lParam)->GetFVal();
					break;
				}
			};
			break;

		break;
	}
	if (dlg) return dlg->WndProc(hWnd, msg, wParam, lParam);
	else return FALSE;
}

LuxMaxParamDlg::LuxMaxParamDlg(
	LuxMax *r, IRendParams *i, BOOL prog)
{
	hFont = hFont = CreateFont(14, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, VARIABLE_PITCH | FF_SWISS, _T(""));
	rend = r;
	ir = i;
	this->prog = prog;
	
	//Enable this to add dialog at the 'render' panel during rendering.
	//Disabled for now since we do not have any useful info there.
	if (prog) {
	//	hPanel = ir->AddRollupPage(
		//	hInstance,
		//	MAKEINTRESOURCE(IDD_RENDER_PROG),
		//	LuxMaxParamDlgProc,
		//	GetString(IDS_VRENDTITLE),
		//	(LPARAM)this);
	}
	else {
		hPanel = ir->AddRollupPage(
			hInstance,
			MAKEINTRESOURCE(IDD_RENDER_PARAMS),
			LuxMaxParamDlgProc,
			GetString(IDS_VRENDTITLE),
			(LPARAM)this);
		hPanel = ir->AddRollupPage(
			hInstance,
			MAKEINTRESOURCE(IDD_RENDER_SAMPLER),
			LuxMaxParamDlgProc,
			GetString(IDS_SAMPLER),
			(LPARAM)this);
		hPanel = ir->AddRollupPage(
			hInstance,
			MAKEINTRESOURCE(IDD_RENDER_FILTER),
			LuxMaxParamDlgProc,
			GetString(IDS_FILTERS),
			(LPARAM)this);
		hPanel = ir->AddRollupPage(
			hInstance,
			MAKEINTRESOURCE(IDD_DEPTH),
			LuxMaxParamDlgProc,
			GetString(IDS_DEPTH),
			(LPARAM)this);
	}
}

void LuxMaxParamDlg::InitParamDialog(HWND hWnd) {
	workFileName = rend->FileName;
	halttimewstr = rend->halttimewstr;
	defaultlightchk = rend->defaultlightchk;
	defaultlightauto = rend->defaultlightauto;
	vbintervalWstr = rend->vbinterval;
	filterIndex = (int)_wtoi(rend->FilterIndexWstr);
	rendertype = (int)_wtoi(rend->RenderTypeWstr);
	lightStrategyIndex = (int)_wtoi(rend->LightStrategyIndexWstr);
	
	HWND hwndOutput = GetDlgItem(hWnd, IDC_HALTTIME);
	SetWindowText(hwndOutput, rend->halttimewstr);

	hwndOutput = GetDlgItem(hWnd, IDC_VBINTERVAL);
	SetWindowText(hwndOutput, rend->vbinterval);

	hwndOutput = GetDlgItem(hWnd, IDC_FILTERS_TYPE_COMBO);
	ComboBox_SetCurSel(hwndOutput,filterIndex);

	hwndOutput = GetDlgItem(hWnd, IDC_RENDERTYPE_NEW);
	ComboBox_SetCurSel(hwndOutput, rendertype);

	hwndOutput = GetDlgItem(hWnd, IDC_COMBO_LIGHT_STRATEGY);
	ComboBox_SetCurSel(hwndOutput,lightStrategyIndex);

	//hwndOutput = GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA);
	//SetWindowText(hwndOutput, rend->FilterGuassianAlphaWstr);
	
}

void LuxMaxParamDlg::InitDepthDialog(HWND hWnd)
{
	depthSpinner = GetISpinner(GetDlgItem(hWnd, IDC_LENSRADIUS_SPIN));
	if (depthSpinner != NULL)
	{
		depthSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_LENSRADIUS), EDITTYPE_FLOAT);
		depthSpinner->SetLimits(0, 100, false);
		depthSpinner->SetResetValue(0.0f);
		depthSpinner->SetScale(0.1f);
		depthSpinner->SetValue((float)_wtof(rend->LensRadiusstr), TRUE);
		ReleaseISpinner(depthSpinner);
	}

	
}

void LuxMaxParamDlg::InitSamplerDialog(HWND hWnd)
{
	samplerIndex = (int)_wtoi(rend->SamplerIndexWstr);

	metropolisLargestEpRateSpinner = GetISpinner(GetDlgItem(hWnd, IDC_METROPOLIS_LARGEST_EP_RATE_SPIN_NEW));
	if (metropolisLargestEpRateSpinner != NULL)
	{
		metropolisLargestEpRateSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_METROPOLIS_LARGEST_EP_RATE_NEW), EDITTYPE_FLOAT);
		metropolisLargestEpRateSpinner->SetLimits(0, 1, false);
		metropolisLargestEpRateSpinner->SetResetValue(0.4f);
		metropolisLargestEpRateSpinner->SetScale(0.4f);
		metropolisLargestEpRateSpinner->SetValue((float)_wtof(rend->MetropolisLargestEpRateWstr), TRUE);
		ReleaseISpinner(metropolisLargestEpRateSpinner);
	}

	metropolisMaxConsecutiveRejectSpinner = GetISpinner(GetDlgItem(hWnd, IDC_MAX_CONSECUTIVE_REJECT_SPIN_NEW));
	if (metropolisMaxConsecutiveRejectSpinner != NULL)
	{
		metropolisMaxConsecutiveRejectSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_MAX_CONSECUTIVE_REJECT_NEW), EDITTYPE_INT);
		metropolisMaxConsecutiveRejectSpinner->SetLimits(0, 32768, false);
		metropolisMaxConsecutiveRejectSpinner->SetResetValue(512);
		metropolisMaxConsecutiveRejectSpinner->SetScale(1);
		metropolisMaxConsecutiveRejectSpinner->SetValue((int)_wtof(rend->MetropolisMaxConsecutiveRejectWstr), TRUE);
		ReleaseISpinner(metropolisMaxConsecutiveRejectSpinner);
	}

	metropolisImageMutationRateSpinner = GetISpinner(GetDlgItem(hWnd, IDC_IMAGE_MUTATION_RATE_SPIN_NEW));
	if (metropolisImageMutationRateSpinner != NULL)
	{
		metropolisImageMutationRateSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_IMAGE_MUTATION_RATE_NEW), EDITTYPE_FLOAT);
		metropolisImageMutationRateSpinner->SetLimits(0, 1, false);
		metropolisImageMutationRateSpinner->SetResetValue(0.1f);
		metropolisImageMutationRateSpinner->SetScale(0.1f);
		metropolisImageMutationRateSpinner->SetValue((float)_wtof(rend->MetrolpolisImageMutationRateWstr), TRUE);
		ReleaseISpinner(metropolisImageMutationRateSpinner);
	}

	HWND hwndSampler = GetDlgItem(hWnd, IDC_SAMPLER_TYPE_COMBO);
	ComboBox_SetCurSel(hwndSampler, samplerIndex);

	showHideSamplerGUI(hWnd,samplerIndex);
}

void LuxMaxParamDlg::InitFilterDialog(HWND hWnd)
{
	filterXSpinner = GetISpinner(GetDlgItem(hWnd, IDC_FILTERXWIDTH_SPIN));
	if (filterXSpinner != NULL)
	{
		filterXSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_FILTERXWIDTH), EDITTYPE_FLOAT);
		filterXSpinner->SetLimits(0, 10, false);
		filterXSpinner->SetResetValue(2.0f);
		filterXSpinner->SetScale(1.0f);
		filterXSpinner->SetValue((float)_wtof(rend->FilterXWidthWst), TRUE);
		ReleaseISpinner(filterXSpinner);
	}

	filterYSpinner = GetISpinner(GetDlgItem(hWnd, IDC_FILTERYWIDTH_SPIN));
	if (filterYSpinner != NULL)
	{
		filterYSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_FILTERYWIDTH), EDITTYPE_FLOAT);
		filterYSpinner->SetLimits(0, 10, false);
		filterYSpinner->SetResetValue(2.0f);
		filterYSpinner->SetScale(1.0f);
		filterYSpinner->SetValue((float)_wtof(rend->FilterYWidthWst), TRUE);
		ReleaseISpinner(filterYSpinner);
	}

	filterGuassianAlphaSpinner = GetISpinner(GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA_SPIN));
	if (filterGuassianAlphaSpinner != NULL)
	{
		filterGuassianAlphaSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_FILTER_GUASSIAN_ALPHA),EDITTYPE_FLOAT);
		filterGuassianAlphaSpinner->SetLimits(0.1, 10,FALSE);
		filterGuassianAlphaSpinner->SetResetValue(0.1f);
		filterGuassianAlphaSpinner->SetScale(0.1f);
		filterGuassianAlphaSpinner->SetValue((float)_wtof(rend->FilterGuassianAlphaWstr), TRUE);
		ReleaseISpinner(filterGuassianAlphaSpinner);
	}

	filterMitchellASpinner = GetISpinner(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_A_FLOAT_SPIN));
	if (filterMitchellASpinner != NULL)
	{
		filterMitchellASpinner->LinkToEdit(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_A_FLOAT), EDITTYPE_FLOAT);
		filterMitchellASpinner->SetLimits(0, 1);
		filterMitchellASpinner->SetResetValue(1.0f);
		filterMitchellASpinner->SetScale(0.1f);
		filterMitchellASpinner->SetValue((float)_wtof(rend->FilterMitchellAWstr),true);
		ReleaseISpinner(filterMitchellASpinner);
	}

	filterMitchellBSpinner = GetISpinner(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_B_FLOAT_SPIN));
	if (filterMitchellBSpinner != NULL)
	{
		filterMitchellBSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_FILTER_MITCHEL_B_FLOAT), EDITTYPE_FLOAT);
		filterMitchellBSpinner->SetLimits(0, 1);
		filterMitchellBSpinner->SetResetValue(1.0f);
		filterMitchellBSpinner->SetScale(0.1f);
		filterMitchellBSpinner->SetValue((float)_wtof(rend->FilterMitchellBWstr), true);
		ReleaseISpinner(filterMitchellBSpinner);
	}

	showHideFilterGUI(hWnd, filterIndex);
}

void LuxMaxParamDlg::InitProgDialog(HWND hWnd) {
}

void LuxMaxParamDlg::AcceptParams() {
	rend->FileName = workFileName;
	//rend->renderType = rendertype;
	rend->halttimewstr = halttimewstr;
	rend->LensRadiusstr = std::to_wstring(LensRadiusFloatTmp).c_str();
	rend->vbinterval = vbintervalWstr;
	rend->defaultlightchk = defaultlightchk;
	rend->defaultlightauto = defaultlightauto;
	rend->FilterIndexWstr = std::to_wstring(filterIndex).c_str();
	rend->FilterXWidthWst = std::to_wstring(filterXvalue).c_str();
	rend->FilterYWidthWst = std::to_wstring(filterYvalue).c_str();
	rend->RenderTypeWstr = std::to_wstring(rendertype).c_str();
	rend->FilterGuassianAlphaWstr = std::to_wstring(filterGuassianAlphavalue).c_str();
	rend->FilterMitchellAWstr = std::to_wstring(filterMitchellAvalue).c_str();
	rend->FilterMitchellBWstr = std::to_wstring(filterMitchellBvalue).c_str();
	rend->LightStrategyIndexWstr = std::to_wstring(lightStrategyIndex).c_str();
	rend->MetrolpolisImageMutationRateWstr = std::to_wstring(MetrolpolisImageMutationRatevalue).c_str();
	rend->MetropolisLargestEpRateWstr = std::to_wstring(MetropolisLargestEpRatevalue).c_str();
	rend->MetropolisMaxConsecutiveRejectWstr = std::to_wstring(MetropolisMaxConsecutiveRejectvalue).c_str();
	rend->SamplerIndexWstr = std::to_wstring(samplerIndex).c_str();
}

RendParamDlg * LuxMax::CreateParamDialog(IRendParams *ir, BOOL prog) {
	return new LuxMaxParamDlg(this, ir, prog);
}

// File Browse ------------------------------------------------------------
BOOL FileExists(const TCHAR *filename) {
	HANDLE findhandle;
	WIN32_FIND_DATA file;
	findhandle = FindFirstFile(filename, &file);
	FindClose(findhandle);
	if (findhandle == INVALID_HANDLE_VALUE)
		return(FALSE);
	else
		return(TRUE);
}

BOOL RunningNewShell()
{
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT && os.dwMajorVersion < 4)
		return FALSE;
	return TRUE;
}

#define FileEXT _T(".png")
#define FileFILTER _T("*.png")

void FixFileExt(OPENFILENAME &ofn, TCHAR* ext = FileEXT) {
	int l = static_cast<int>(_tcslen(ofn.lpstrFile));  // SR DCAST64: Downcast to 2G limit.
	int e = static_cast<int>(_tcslen(ext));   // SR DCAST64: Downcast to 2G limit.
	if (_tcsicmp(ofn.lpstrFile + l - e, ext)) {
		_tcscat(ofn.lpstrFile, ext);
	}
}

#if 0
UINT_PTR WINAPI FileHook( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDOK, _T("OK"));
		break;
	case WM_COMMAND:{
	}

		break;
	}
	return FALSE;
}

UINT_PTR PMFileHook(HWND hWnd,UINT message,WPARAM wParam,LPARAM   lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDOK, _T("OK"));
		break;
	case WM_COMMAND:{
	}

		break;
	}
	return 0;
}
#endif

BOOL LuxMaxParamDlg::FileBrowse() {
	FilterList filterList;
	HWND hWnd = hPanel;
	static int filterIndex = 1;
	OPENFILENAME  ofn;
	
	TSTR filename;
	TCHAR fname[512];
	TCHAR saveDir[1024];
	{
		TSTR dir;
		SplitFilename(workFileName, &dir, &filename, NULL);
		_tcscpy(saveDir, dir.data());
	}
	_tcscpy(fname, filename.data());
	_tcscat(fname, FileEXT);

	filterList.Append(GetString(IDS_FILE));
	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInstance;

	ofn.nFilterIndex = filterIndex;
	ofn.lpstrFilter = filterList;

	ofn.lpstrTitle = GetString(IDS_WRITE_FILE);
	ofn.lpstrFile = fname;
	ofn.nMaxFile = _countof(fname);

	Interface *iface = GetCOREInterface();

	if (saveDir[0])
		ofn.lpstrInitialDir = saveDir;
	else
		ofn.lpstrInitialDir = iface->GetDir(APP_SCENE_DIR);

	if (RunningNewShell()) {
		ofn.Flags = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING;
		ofn.lpfnHook = NULL;
		ofn.lCustData = 0;   // 0 for save, 1 for open

	}
	else {
		ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING;
		ofn.lpfnHook = NULL; 
		ofn.lCustData = 0;
	}

	FixFileExt(ofn, FileEXT);
	while (GetSaveFileName(&ofn))    {
	//while ((&ofn)){
	FixFileExt(ofn, FileEXT); // add ".vue" if absent

		workFileName = ofn.lpstrFile;
		return TRUE;
	}
	return FALSE;
}