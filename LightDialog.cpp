// LightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "LightDialog.h"
#include "vec3.hpp"


// CLightDialog dialog

IMPLEMENT_DYNAMIC(CLightDialog, CDialog)

CLightDialog::CLightDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLightDialog::IDD, pParent)
{
    m_currentLightIdx = 0;
}

CLightDialog::~CLightDialog()
{
}

void CLightDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int r, g, b;
	double x, y, z;

	//ambient light
	COLORREF curr_c = m_ambiant.color.get_rgb();

	r = GetRValue(curr_c);
	g = GetGValue(curr_c);
	b = GetBValue(curr_c);
	DDX_Text(pDX, IDC_AMBL_COLOR_R, r);
	DDX_Text(pDX, IDC_AMBL_COLOR_G, g);
	DDX_Text(pDX, IDC_AMBL_COLOR_B, b);
	m_ambiant.color = RGB(r, g, b);

	double temp_d = m_ambiant.ambient_coeff;
	DDX_Text(pDX, IDC_AMBIENT_COEFF, temp_d);
	m_ambiant.ambient_coeff = temp_d;

	//update light parameters for the currently selected light

	curr_c = m_lights[m_currentLightIdx].color.get_rgb();
	r = GetRValue(curr_c);
	g = GetGValue(curr_c);
	b = GetBValue(curr_c);
	DDX_Text(pDX, IDC_LIGHT_COLOR_R, r);
	DDX_Text(pDX, IDC_LIGHT_COLOR_G, g);
	DDX_Text(pDX, IDC_LIGHT_COLOR_B, b);
	m_lights[m_currentLightIdx].color = RGB(r, g, b);

	vec3 curr_p = m_lights[m_currentLightIdx].pos;
	x = curr_p.x;
	y = curr_p.y;
	z = curr_p.z;
	DDX_Text(pDX, IDC_LIGHT_POS_X, x);
	DDX_Text(pDX, IDC_LIGHT_POS_Y, y);
	DDX_Text(pDX, IDC_LIGHT_POS_Z, z);
	m_lights[m_currentLightIdx].pos = { x, y, z };

	curr_p = m_lights[m_currentLightIdx].dir;
	x = curr_p.x;
	y = curr_p.y;
	z = curr_p.z;
	DDX_Text(pDX, IDC_LIGHT_DIR_X, x);
	DDX_Text(pDX, IDC_LIGHT_DIR_Y, y);
	DDX_Text(pDX, IDC_LIGHT_DIR_Z, z);
	m_lights[m_currentLightIdx].dir = { x, y, z };

	//NOTE:Add more dialog controls which are associated with the structure below this line		
	//...

	//the following class members can't be updated directly through DDX
	//using a helper variable for type-casting to solve the compilation error

	int helper=m_lights[m_currentLightIdx].enabled;
	DDX_Check(pDX,IDC_LIGHT_ENABLED,helper);
	m_lights[m_currentLightIdx].enabled = (bool)helper;

	helper = m_lights[m_currentLightIdx].type;
	DDX_CBIndex(pDX,IDC_LIGHT_TYPE,helper);
	m_lights[m_currentLightIdx].type = (LightType)helper;

	helper = m_lights[m_currentLightIdx].space;
	DDX_CBIndex(pDX,IDC_LIGHT_SPACE,helper);
	m_lights[m_currentLightIdx].space = (LightSpace)helper;

	temp_d = m_lights[m_currentLightIdx].diffuse_coeff;
	DDX_Text(pDX, IDC_DIFFUSE_COEFF, temp_d);
	m_lights[m_currentLightIdx].diffuse_coeff = temp_d;

	temp_d = m_lights[m_currentLightIdx].specular_coeff;
	DDX_Text(pDX, IDC_SPECULAR_COEFF, temp_d);
	m_lights[m_currentLightIdx].specular_coeff = temp_d;

	temp_d = m_lights[m_currentLightIdx].cone_angle;
	DDX_Text(pDX, IDC_CONE_ANGLE, temp_d);
	m_lights[m_currentLightIdx].cone_angle = temp_d;

	DDX_Text(pDX, IDC_SPECULAR_EXP, specular_exp);

	DDX_Control(pDX, IDC_CONE_ANGLE, m_ConeEditBox);
	m_ConeEditBox.EnableWindow(m_lights[m_currentLightIdx].type == LIGHT_TYPE_SPOT);

	DDX_Control(pDX, IDC_LIGHT_POS_X, m_PosXEditBox);
	m_PosXEditBox.EnableWindow(m_lights[m_currentLightIdx].type != LIGHT_TYPE_DIRECTIONAL);
	DDX_Control(pDX, IDC_LIGHT_POS_Y, m_PosYEditBox);
	m_PosYEditBox.EnableWindow(m_lights[m_currentLightIdx].type != LIGHT_TYPE_DIRECTIONAL);
	DDX_Control(pDX, IDC_LIGHT_POS_Z, m_PosZEditBox);
	m_PosZEditBox.EnableWindow(m_lights[m_currentLightIdx].type != LIGHT_TYPE_DIRECTIONAL);

	DDX_Control(pDX, IDC_LIGHT_DIR_X, m_DirXEditBox);
	m_DirXEditBox.EnableWindow(m_lights[m_currentLightIdx].type != LIGHT_TYPE_POINT);
	DDX_Control(pDX, IDC_LIGHT_DIR_Y, m_DirYEditBox);
	m_DirYEditBox.EnableWindow(m_lights[m_currentLightIdx].type != LIGHT_TYPE_POINT);
	DDX_Control(pDX, IDC_LIGHT_DIR_Z, m_DirZEditBox);
	m_DirZEditBox.EnableWindow(m_lights[m_currentLightIdx].type != LIGHT_TYPE_POINT);
}


BEGIN_MESSAGE_MAP(CLightDialog, CDialog)
    ON_BN_CLICKED(IDC_RADIO_LIGHT1, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT2, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT3, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT4, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT5, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT6, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT7, &CLightDialog::OnBnClickedRadioLight)
    ON_BN_CLICKED(IDC_RADIO_LIGHT8, &CLightDialog::OnBnClickedRadioLight)
	ON_CBN_SELCHANGE(IDC_LIGHT_TYPE, UpdateDisabled)
END_MESSAGE_MAP()

void CLightDialog::UpdateDisabled()
{
	UpdateData();
}

void CLightDialog::SetDialogData( LightID id, const Light& light )
{    
    if (id<=LIGHT_ID_AMBIENT)
	m_ambiant = light;
    else
	m_lights[id]=light;
}

Light CLightDialog::GetDialogData( LightID id )
{
	if (id == LIGHT_ID_AMBIENT)
		return m_ambiant;
	else
		return m_lights[id];
}
// CLightDialog message handlers

//this callback function is called when each of the radio buttons on the dialog is clicked
void CLightDialog::OnBnClickedRadioLight()
{
    //save the dialog state into the data variables
    UpdateData(TRUE);
    //get the newly selected light index from the radio buttons
    m_currentLightIdx=GetCheckedRadioButton(IDC_RADIO_LIGHT1,IDC_RADIO_LIGHT8)-IDC_RADIO_LIGHT1;
    //Update all dialog fields according to the new light index
    UpdateData(FALSE);
    Invalidate();
}

BOOL CLightDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    //Set the radio button of the current light to be selected
    CheckRadioButton(IDC_RADIO_LIGHT1,IDC_RADIO_LIGHT8,m_currentLightIdx+IDC_RADIO_LIGHT1);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
