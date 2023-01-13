#include "stdafx.h"
#include "..\public\S_Jake_Son.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

CS_Jake_Son::CS_Jake_Son(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Jake_Son::CS_Jake_Son(const CS_Jake_Son & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Jake_Son::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Jake_Son::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (nullptr != pArg)
		memcpy(&m_tJakeSonInfo, pArg, sizeof(m_tJakeSonInfo));
	
	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = m_tJakeSonInfo.fPos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_wsTag = L"Jake_Son";
	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CS_Jake_Son::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CS_Jake_Son::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_NPC, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CS_Jake_Son::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CS_Jake_Son::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CEffect_Manager::GetInstance()->Effect_JakeSon_Create(_float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z - 0.5f));
		CUI_Manager::GetInstance()->Set_JakeSon_Count();
		CGameObject::Set_Dead();
	}
}

HRESULT CS_Jake_Son::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	_tchar	m_szTextureName[MAX_PATH] = L"";

	if (JAKESONINFO::JAKE_SON_A == m_tJakeSonInfo.eJakeSon)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Model_S_Jake_son_A"));
	else if (JAKESONINFO::JAKE_SON_B == m_tJakeSonInfo.eJakeSon)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Model_S_Jake_son_B"));
	else if (JAKESONINFO::JAKE_SON_C == m_tJakeSonInfo.eJakeSon)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Model_S_Jake_son_C"));
	else if (JAKESONINFO::JAKE_SON_D == m_tJakeSonInfo.eJakeSon)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Model_S_Jake_son_D"));
	else if (JAKESONINFO::JAKE_SON_E == m_tJakeSonInfo.eJakeSon)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Model_S_Jake_son_E"));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Jake_Son::SetUp_ShaderResources()
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

CS_Jake_Son * CS_Jake_Son::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Jake_Son*		pInstance = new CS_Jake_Son(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Jake_Son");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Jake_Son::Clone(void * pArg)
{
	CS_Jake_Son*		pInstance = new CS_Jake_Son(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Jake_Son");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Jake_Son::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
