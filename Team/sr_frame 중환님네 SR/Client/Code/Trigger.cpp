#include "stdafx.h"
#include "..\Header\Trigger.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev)
{
}

CTrigger::CTrigger(const CGameObject& rhs): CGameObject(rhs)
{
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::Ready_Object()
{
	m_pTransform = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	return S_OK;
}

_int CTrigger::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	CGameObject::Update_Object(fTimeDelta);

	if (m_fCurTriggerFreq < m_fTriggerFreq)
		m_fCurTriggerFreq += fTimeDelta;

	return OBJ_NOEVENT;
}

void CTrigger::LateUpdate_Object()
{
	if (m_fCurTriggerFreq >= m_fTriggerFreq)
	{
		set<CGameObject*> setObj;
		Engine::GetOverlappedObject(setObj, m_pTransform->m_vInfo[INFO_POS], m_fRadius);
		if (m_pTriggerFunction(setObj))
			m_bDelete = true;
		m_fCurTriggerFreq = 0.f;
	}
}

void CTrigger::Free()
{
	CGameObject::Free();
}

CTrigger* CTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrigger* Inst = new CTrigger(pGraphicDev);

	if (FAILED(Inst->Ready_Object()))
	{
		return nullptr;
	}

	return Inst;
}
