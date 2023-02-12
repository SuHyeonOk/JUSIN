#include "stdafx.h"
#include "..\public\MiniGmae_Collider.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "Knives_Rain.h"
#include "O_TextureObject.h"

CMiniGame_Collider::CMiniGame_Collider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CMiniGame_Collider::CMiniGame_Collider(const CMiniGame_Collider & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMiniGame_Collider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Collider::Initialize(void * pArg)
{
	m_wsTag = L"Object_Collider";

	if (nullptr != pArg)
		memcpy(&m_ColliderInfo, pArg, sizeof(m_ColliderInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_ColliderInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CMiniGame_Collider::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (true == m_bKnivesRain)
	{
		KnivesRain();
	}
}

void CMiniGame_Collider::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CMiniGame_Collider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CMiniGame_Collider::On_Collision(CGameObject * pOther)
{
	switch (m_ColliderInfo.eType)
	{
	case KNIVESRAIN:
	{
		m_bKnivesRain = true;
	}
	break;
	}
}

HRESULT CMiniGame_Collider::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	if (KNIVESRAIN == m_ColliderInfo.eType)
	{
		ColliderDesc.vSize = _float3(7.0f, 1.0f, 1.0f);
		ColliderDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		ColliderDesc.vSize = _float3(1.0f, 1.0f, 1.0f);
		ColliderDesc.vCenter = _float3(0.0f, 0.0f/*ColliderDesc.vSize.y * 0.5f*/, 0.0f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Collider::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMiniGame_Collider::KnivesRain()
{
	for (_int i = 0; i < 20; ++i)
	{
		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);
		CKnives_Rain * pGameObject = dynamic_cast<CKnives_Rain*>(pGameInstance->Get_GameObjectPtr(LEVEL_MINIGAME, TEXT("Layer_Knives_Rain"), TEXT("Prototype_GameObject_Knives_Rain"), i));
		RELEASE_INSTANCE(CGameInstance);

		pGameObject->Set_Rain();

		if (i == 19)
			m_bKnivesRain = false;
	}
}

CMiniGame_Collider * CMiniGame_Collider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniGame_Collider*		pInstance = new CMiniGame_Collider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMiniGame_Collider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMiniGame_Collider::Clone(void * pArg)
{
	CMiniGame_Collider*		pInstance = new CMiniGame_Collider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMiniGame_Collider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMiniGame_Collider::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
