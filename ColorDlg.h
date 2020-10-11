#pragma once

class ColorDlg : public CDialog
{
	DECLARE_DYNAMIC(ColorDlg)
public:
	ColorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ColorDlg();

	// Dialog Data
	enum { IDD = IDD_COLOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	COLORREF wireframe_c;
	COLORREF normals_c;
	COLORREF bg_c;


public:
	COLORREF get_wireframe_c() { return wireframe_c; }
	COLORREF get_normals_c() { return normals_c; }
	COLORREF get_bg_c() { return bg_c; }

	bool should_invalidate;

	afx_msg void OnBnClickedButtonW();
	afx_msg void OnBnClickedButtonN();
	afx_msg void OnBnClickedButtonBG();
};
