#include "stdafx.h"
#include "..\public\N_NPC.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CN_NPC::CN_NPC(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CN_NPC::CN_NPC(const CN_NPC & rhs)
	: CGameObject(rhs)
{

}

HRESULT CN_NPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_NPC::Initialize(void * pArg)
{
	NPCDESC		NpcInfo;
	ZeroMemory(&NpcInfo, sizeof(NPCDESC));

	if (nullptr != pArg)
		memcpy(&NpcInfo, pArg, sizeof(NPCDESC));

	if (FAILED(__super::Initialize(&NpcInfo)))
		return E_FAIL;
	
	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CN_NPC::Tick(const _double& TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_G))
		++m_Script_Count;

	RELEASE_INSTANCE(CGameInstance);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_NPC, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CN_NPC::Late_Tick(const _double& TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CN_NPC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

void CN_NPC::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_bInteraction = CObj_Manager::GetInstance()->Get_Interaction();
	}
}

void CN_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
