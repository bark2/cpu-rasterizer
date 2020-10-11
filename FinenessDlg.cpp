// FinenessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "FinenessDlg.h"


// FinenessDlg dialog

IMPLEMENT_DYNAMIC(FinenessDlg, CDialog)

FinenessDlg::FinenessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FinenessDlg::IDD, pParent),
	fineness(100)
{

}

FinenessDlg::~FinenessDlg()
{
}

void FinenessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_FINENESS, fineness);
}

int FinenessDlg::get_fineness() { return (int)(fineness * (18.0 / 100.0)) + 2; }

BEGIN_MESSAGE_MAP(FinenessDlg, CDialog)
END_MESSAGE_MAP()


// FinenessDlg message handlers
