#include "stdafx.h"
#include "..\public\S_Jake_Son_Twister.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CS_Jake_Son_Twister::CS_Jake_Son_Twister(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Jake_Son_Twister::CS_Jake_Son_Twister(const CS_Jake_Son_Twister & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Jake_Son_Twister::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Jake_Son_Twister::Initialize(void * pArg)
{	
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	m_pJakeSon_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_S_Change_JakeSonTransform"), TEXT("Com_Transform"), 0));
	m_pBoss_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_Gary_Boss"), TEXT("Com_Transform"), 0));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CS_Jake_Son_Twister::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// JakeSonTeansform 이동시키기
	m_pJakeSon_TransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	// 기본 공격
	m_pTransformCom->Turn(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), TimeDelta * 5.0f);
	m_pTransformCom->Chase(m_pBoss_TransformCom->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
}

void CS_Jake_Son_Twister::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CS_Jake_Son_Twister::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 0);
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

void CS_Jake_Son_Twister::On_Collision(CGameObject * pOther)
{
	if (L"Gary_Boss" == pOther->Get_Tag())
		CGameObject::Set_Dead();
}

HRESULT CS_Jake_Son_Twister::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_S_JakeSonsTwister"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 1.2f, 1.2f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Jake_Son_Twister::SetUp_ShaderResources()
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

CS_Jake_Son_Twister * CS_Jake_Son_Twister::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Jake_Son_Twister*		pInstance = new CS_Jake_Son_Twister(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Jake_Son_Twister");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Jake_Son_Twister::Clone(void * pArg)
{
	CS_Jake_Son_Twister*		pInstance = new CS_Jake_Son_Twister(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Jake_Son_Twister");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Jake_Son_Twister::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
