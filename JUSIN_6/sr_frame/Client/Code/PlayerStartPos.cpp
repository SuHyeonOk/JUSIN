#include "stdafx.h"
#include "..\Header\PlayerStartPos.h"

CPlayerStartPos::CPlayerStartPos(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev)
{
}

CPlayerStartPos::CPlayerStartPos(const CPlayerStartPos& rhs): CGameObject(rhs)
{
}

CPlayerStartPos::~CPlayerStartPos()
{
}

HRESULT CPlayerStartPos::Ready_Object()
{
	CGameObject::Ready_Object();
	m_pTransform = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	return S_OK;
}

_int CPlayerStartPos::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);


	return OBJ_NOEVENT;
}

void CPlayerStartPos::Free()
{
	CGameObject::Free();
}

CPlayerStartPos* CPlayerStartPos::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerStartPos* inst = new CPlayerStartPos(pGraphicDev);
	if (FAILED(inst->Ready_Object()))
	{
		return nullptr;
	}
	return inst;
}
