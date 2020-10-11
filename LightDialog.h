#pragma once

#include "Light.h"
// CLightDialog dialog



class CLightDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightDialog)

public:
	CLightDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLightDialog();

	//dialog interface
	void SetDialogData(LightID id,const Light& light);
	Light GetDialogData(LightID id);

// Dialog Data
	enum { IDD = IDD_LIGHTS_DLG };

	// specular exponent
	int specular_exp;

protected:
	Light m_lights[MAX_LIGHT];
	Light m_ambiant;
	int m_currentLightIdx;
	CEdit m_ConeEditBox;
	CEdit m_PosXEditBox;
	CEdit m_PosYEditBox;
	CEdit m_PosZEditBox;
	CEdit m_DirXEditBox;
	CEdit m_DirYEditBox;
	CEdit m_DirZEditBox;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedRadioLight();
	afx_msg void UpdateDisabled();
//    afx_msg void On();
    virtual BOOL OnInitDialog();
};
