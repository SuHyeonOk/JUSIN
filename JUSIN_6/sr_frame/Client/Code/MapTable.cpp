#include "stdafx.h"
#include "..\Header\MapTable.h"
#include "StaticCamera.h"
#include "Player.h"

CMapTable::CMapTable(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev)
{
}

CMapTable::CMapTable(const CMapTable & rhs) : CGameObject(rhs)
{
}


CMapTable::~CMapTable()
{
}

HRESULT CMapTable::Ready_Object()
{
	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<Engine::CCubeTex>(L"Proto_CubeTexCom", L"Proto_CubeTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<Engine::CTexture>(L"Proto_MinecraftCubeTexture", L"Proto_MinecraftCubeTexture", ID_STATIC);

	return S_OK;
}

_int CMapTable::Update_Object(const _float & fTimeDelta)
{
	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CMapTable::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CMapTable::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(193);
	m_pBufferCom->Render_Buffer();	
}

void CMapTable::Free()
{	
	CGameObject::Free();
}

CMapTable * CMapTable::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMapTable* pInstance = new CMapTable(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}
