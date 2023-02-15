#include "stdafx.h"
#include "..\public\Cake.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CCake::CCake(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CCake::CCake(const CCake & rhs)
	: CGameObject(rhs)
{

}

HRESULT CCake::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCake::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_ObjectInfo, pArg, sizeof(OBJECTINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_ObjectInfo.f3Position;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled({ 2.0f, 2.0f, 2.0f });

	switch (m_ObjectInfo.eType)
	{
	case Client::CCake::BLUE:
		m_wsTag = L"Cake_Blue";
		break;
	case Client::CCake::MAGENTA:
		m_wsTag = L"Cake_Magenta";
		break;
	case Client::CCake::YELLOW:
		m_wsTag = L"Cake_Yellow";
		break;
	}

	return S_OK;
}

void CCake::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CCake::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CCake::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderCom, 0, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderCom, 0);

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CCake::On_Collision(CGameObject * pOther)
{
	if (L"Jake" == pOther->Get_Tag())
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_Sound(TEXT("sfx_enchiridion_food_good.ogg"), 1.0f);
		RELEASE_INSTANCE(CGameInstance);

		CGameObject::Set_Dead();
	}
}

HRESULT CCake::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	_tchar	m_szModelName[MAX_PATH] = L"";


	switch (m_ObjectInfo.eType)
	{
	case Client::CCake::BLUE:
		wsprintf(m_szModelName, TEXT("Prototype_Component_Model_Cake_Blue"));
		break;
	case Client::CCake::MAGENTA:
		wsprintf(m_szModelName, TEXT("Prototype_Component_Model_Cake_Magenta"));
		break;
	case Client::CCake::YELLOW:
		wsprintf(m_szModelName, TEXT("Prototype_Component_Model_Cake_Yellow"));
		break;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, m_szModelName, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCake::SetUp_ShaderResources()
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

CCake * CCake::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCake*		pInstance = new CCake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCake::Clone(void * pArg)
{
	CCake*		pInstance = new CCake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCake::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
