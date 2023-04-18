#include "stdafx.h"
#include "..\Header\EditBox.h"


CEditBox::CEditBox(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}

CEditBox::~CEditBox()
{
}

HRESULT CEditBox::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_UI_Texture", L"Proto_UI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_iTexNum = 29;
	// m_strInputs = "sdfsd";

	return S_OK;
}

_int CEditBox::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;
	Engine::CGameObject::Update_Object(fTimeDelta);

	if (DIKeyDown(DIK_BACKSPACE) && m_strInputs.empty() == false)
	{
		m_strInputs.pop_back();
	}

	if (DIKeyPressing(DIK_LSHIFT) || DIKeyPressing(DIK_RSHIFT))
	{
		if (DIKeyDown(DIK_Q)) m_strInputs.push_back('Q');
		if (DIKeyDown(DIK_W)) m_strInputs.push_back('W');
		if (DIKeyDown(DIK_E)) m_strInputs.push_back('E');
		if (DIKeyDown(DIK_R)) m_strInputs.push_back('R');
		if (DIKeyDown(DIK_T)) m_strInputs.push_back('T');
		if (DIKeyDown(DIK_Y)) m_strInputs.push_back('Y');
		if (DIKeyDown(DIK_U)) m_strInputs.push_back('U');
		if (DIKeyDown(DIK_I)) m_strInputs.push_back('I');
		if (DIKeyDown(DIK_O)) m_strInputs.push_back('O');
		if (DIKeyDown(DIK_P)) m_strInputs.push_back('P');
		if (DIKeyDown(DIK_A)) m_strInputs.push_back('A');
		if (DIKeyDown(DIK_S)) m_strInputs.push_back('S');
		if (DIKeyDown(DIK_D)) m_strInputs.push_back('D');
		if (DIKeyDown(DIK_F)) m_strInputs.push_back('F');
		if (DIKeyDown(DIK_G)) m_strInputs.push_back('G');
		if (DIKeyDown(DIK_H)) m_strInputs.push_back('H');
		if (DIKeyDown(DIK_J)) m_strInputs.push_back('J');
		if (DIKeyDown(DIK_K)) m_strInputs.push_back('K');
		if (DIKeyDown(DIK_L)) m_strInputs.push_back('L');
		if (DIKeyDown(DIK_Z)) m_strInputs.push_back('Z');
		if (DIKeyDown(DIK_X)) m_strInputs.push_back('X');
		if (DIKeyDown(DIK_C)) m_strInputs.push_back('C');
		if (DIKeyDown(DIK_V)) m_strInputs.push_back('V');
		if (DIKeyDown(DIK_B)) m_strInputs.push_back('B');
		if (DIKeyDown(DIK_N)) m_strInputs.push_back('N');
		if (DIKeyDown(DIK_M)) m_strInputs.push_back('M');
	}
	else
	{
		if (DIKeyDown(DIK_Q)) m_strInputs.push_back('q');
		if (DIKeyDown(DIK_W)) m_strInputs.push_back('w');
		if (DIKeyDown(DIK_E)) m_strInputs.push_back('e');
		if (DIKeyDown(DIK_R)) m_strInputs.push_back('r');
		if (DIKeyDown(DIK_T)) m_strInputs.push_back('t');
		if (DIKeyDown(DIK_Y)) m_strInputs.push_back('y');
		if (DIKeyDown(DIK_U)) m_strInputs.push_back('u');
		if (DIKeyDown(DIK_I)) m_strInputs.push_back('i');
		if (DIKeyDown(DIK_O)) m_strInputs.push_back('o');
		if (DIKeyDown(DIK_P)) m_strInputs.push_back('p');
		if (DIKeyDown(DIK_A)) m_strInputs.push_back('a');
		if (DIKeyDown(DIK_S)) m_strInputs.push_back('s');
		if (DIKeyDown(DIK_D)) m_strInputs.push_back('d');
		if (DIKeyDown(DIK_F)) m_strInputs.push_back('f');
		if (DIKeyDown(DIK_G)) m_strInputs.push_back('g');
		if (DIKeyDown(DIK_H)) m_strInputs.push_back('h');
		if (DIKeyDown(DIK_J)) m_strInputs.push_back('j');
		if (DIKeyDown(DIK_K)) m_strInputs.push_back('k');
		if (DIKeyDown(DIK_L)) m_strInputs.push_back('l');
		if (DIKeyDown(DIK_Z)) m_strInputs.push_back('z');
		if (DIKeyDown(DIK_X)) m_strInputs.push_back('x');
		if (DIKeyDown(DIK_C)) m_strInputs.push_back('c');
		if (DIKeyDown(DIK_V)) m_strInputs.push_back('v');
		if (DIKeyDown(DIK_B)) m_strInputs.push_back('b');
		if (DIKeyDown(DIK_N)) m_strInputs.push_back('n');
		if (DIKeyDown(DIK_M)) m_strInputs.push_back('m');
	}

	Add_RenderGroup(RENDER_UI, this);

	return S_OK;
}

void CEditBox::Render_Object()
{
	CUI::Render_Object();

	if (m_strInputs.empty() == false)
	{
		wstring tmp;
		tmp.assign(m_strInputs.begin(), m_strInputs.end());

		const _vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
		const _vec3& vSize = m_pTransCom->m_vScale;
		const _float fX = vPos.x + WINCX / 2;
		const _float fY = -vPos.y + WINCY / 2;
		_vec2 vTextPos = {fX - vSize.x * 0.5f + 10.f, fY - vSize.y * 0.5f + 10.f};

		Render_Font(L"Gothic_Regular25", tmp.c_str(), &vTextPos, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
}

CEditBox* CEditBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEditBox* pInstance = new CEditBox(pGraphicDev);
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
