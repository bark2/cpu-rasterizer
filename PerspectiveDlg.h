#pragma once

class CPerspectiveDlg : public CDialog
{
	DECLARE_DYNAMIC(CPerspectiveDlg)
public:
	CPerspectiveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPerspectiveDlg();

	// Dialog Data
	enum { IDD = IDD_PERSPECTIVE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int n; // near
	int f; // far

public:
	double get_n();
	double get_f();
};
