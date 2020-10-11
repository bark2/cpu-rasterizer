// ColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "ColorDlg.h"


// ColorDlg dialog

IMPLEMENT_DYNAMIC(ColorDlg, CDialog)

ColorDlg::ColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ColorDlg::IDD, pParent),
	wireframe_c(RGB(255, 255, 255)),
	normals_c(RGB(255, 0, 255)),
	bg_c(RGB(0, 0, 0)),
	should_invalidate(false)
{

}

ColorDlg::~ColorDlg()
{
}

void ColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ColorDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WIREFRAME_COLOR, &ColorDlg::OnBnClickedButtonW)
	ON_BN_CLICKED(IDC_BUTTON_NORMALS_COLOR, &ColorDlg::OnBnClickedButtonN)
	ON_BN_CLICKED(IDC_BUTTON_BG_COLOR, &ColorDlg::OnBnClickedButtonBG)
END_MESSAGE_MAP()


// ColorDlg message handlers


void ColorDlg::OnBnClickedButtonW()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		if (wireframe_c != colorDlg.GetColor()) {
			wireframe_c = colorDlg.GetColor();
			should_invalidate = true;
		}
	}
}

void ColorDlg::OnBnClickedButtonN()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		if (normals_c != colorDlg.GetColor()) {
			normals_c = colorDlg.GetColor();
			should_invalidate = true;
		}
	}
}

void ColorDlg::OnBnClickedButtonBG()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		if (bg_c != colorDlg.GetColor()) {
			bg_c = colorDlg.GetColor();
			should_invalidate = true;
		}
	}
}