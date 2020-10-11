#pragma once

class CSensitivityDlg : public CDialog
{
	DECLARE_DYNAMIC(CSensitivityDlg)
public:
	CSensitivityDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSensitivityDlg();

	// Dialog Data
	enum { IDD = IDD_SENSITIVITY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int translation_sensitivity;
	int scaling_sensitivity;
	int rotation_sensitivity;

public:
	double get_translation_sensitivity();
	double get_scaling_sensitivity();
	double get_rotation_sensitivity();
};
