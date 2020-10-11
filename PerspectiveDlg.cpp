// PerspectiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "PerspectiveDlg.h"


// CPerspectiveDlg dialog

IMPLEMENT_DYNAMIC(CPerspectiveDlg, CDialog)

CPerspectiveDlg::CPerspectiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPerspectiveDlg::IDD, pParent),
	n(30.0f),
	f(60.0f)
{

}

CPerspectiveDlg::~CPerspectiveDlg()
{
}

void CPerspectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_PERSPECTIVE_NEAR, n);
	DDX_Slider(pDX, IDC_PERSPECTIVE_FAR, f);
}

double CPerspectiveDlg::get_n() { return n; }
double CPerspectiveDlg::get_f() { return f; }


BEGIN_MESSAGE_MAP(CPerspectiveDlg, CDialog)
END_MESSAGE_MAP()


// CPerspectiveDlg message handlers
