#pragma once

class FinenessDlg : public CDialog
{
	DECLARE_DYNAMIC(FinenessDlg)
public:
	FinenessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~FinenessDlg();

	// Dialog Data
	enum { IDD = IDD_FINENESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int fineness;

public:
	int get_fineness();
};

