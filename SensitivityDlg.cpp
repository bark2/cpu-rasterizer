// SensitivityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "SensitivityDlg.h"


// CSensitivityDlg dialog

IMPLEMENT_DYNAMIC(CSensitivityDlg, CDialog)

CSensitivityDlg::CSensitivityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSensitivityDlg::IDD, pParent),
		translation_sensitivity(30),
		scaling_sensitivity(30),
		rotation_sensitivity(30)
{

}

CSensitivityDlg::~CSensitivityDlg()
{
}

void CSensitivityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_TRANSLATE_SENSITIVITY, translation_sensitivity);
	DDX_Slider(pDX, IDC_SCALING_SENSITIVITY, scaling_sensitivity);
	DDX_Slider(pDX, IDC_ROTATION_SENSITIVITY, rotation_sensitivity);
}

double CSensitivityDlg::get_translation_sensitivity() { return (translation_sensitivity + 1) / 20.0; }
double CSensitivityDlg::get_scaling_sensitivity() { return (scaling_sensitivity + 1) / 20.0; }
double CSensitivityDlg::get_rotation_sensitivity() { return (rotation_sensitivity + 1) / 20.0; }


BEGIN_MESSAGE_MAP(CSensitivityDlg, CDialog)
END_MESSAGE_MAP()


// CSensitivityDlg message handlers
