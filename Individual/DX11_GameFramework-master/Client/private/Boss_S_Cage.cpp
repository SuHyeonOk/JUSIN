#include "stdafx.h"
#include "..\public\Boss_S_Cage.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "M_Monster.h"
#include "M_Skeleton_Archer.h"
#include "M_Skeleton_Shield.h"

CBoss_S_Cage::CBoss_S_Cage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CBoss_S_Cage::CBoss_S_Cage(const CBoss_S_Cage & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBoss_S_Cage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Cage::Initialize(void * pArg)
{	
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(15.0f));

	return S_OK;
}

void CBoss_S_Cage::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (false == m_bDead)
	{
		_int iDead_Count = 0;
		for (_int i = 0; i < 5; ++i)
		{
			if (true == m_arrSkeleton_Dead_Count[i])
				++iDead_Count;
		}

		if (5 <= iDead_Count)
			m_bDead = true;
	}
	else
	{
		if (0.0f < m_fAlpha)
			m_fAlpha -= _float(TimeDelta);
		else
			CGameObject::Set_Dead();
	}

	Skeleton_Create();
	Skeleton_Dead_Check();
}

void CBoss_S_Cage::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject::Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CBoss_S_Cage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 1);
	}

	return S_OK;
}

void CBoss_S_Cage::On_Collision(CGameObject * pOther)
{

}

HRESULT CBoss_S_Cage::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_B_Boss_Death_Cage"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Cage::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Cage::Skeleton_Create()
{
	if (5 == m_iSkeleton_Create_Count)
		return S_OK;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CM_Monster::MONSTERDESC tMonsterDesc;

	tMonsterDesc.eMonsterKind = CM_Monster::MONSTERDESC::SKELETON_ARCHER_1;
	tMonsterDesc.f3Pos = _float3(7.0f, 0.0f, 17.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Monster_Skeleton_Archer_1"), TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	++m_iSkeleton_Create_Count;

	tMonsterDesc.eMonsterKind = CM_Monster::MONSTERDESC::SKELETON_SHIELD_1;
	tMonsterDesc.f3Pos = _float3(5.0f, 0.0f, 14.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Monster_Skeleton_Shield_1"), TEXT("Prototype_GameObject_M_Skeleton_Shield"), &tMonsterDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	++m_iSkeleton_Create_Count;

	tMonsterDesc.eMonsterKind = CM_Monster::MONSTERDESC::SKELETON_ARCHER_2;
	tMonsterDesc.f3Pos = _float3(4.0f, 0.0f, 16.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Monster_Skeleton_Archer_2"), TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	++m_iSkeleton_Create_Count;

	tMonsterDesc.eMonsterKind = CM_Monster::MONSTERDESC::SKELETON_SHIELD_2;
	tMonsterDesc.f3Pos = _float3(2.5f, 0.0f, 18.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Monster_Skeleton_Shield_2"), TEXT("Prototype_GameObject_M_Skeleton_Shield"), &tMonsterDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	++m_iSkeleton_Create_Count;

	tMonsterDesc.eMonsterKind = CM_Monster::MONSTERDESC::SKELETON_ARCHER_3;
	tMonsterDesc.f3Pos = _float3(1.0f, 0.0f, 15.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Monster_Skeleton_Archer_3"), TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	++m_iSkeleton_Create_Count;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBoss_S_Cage::Skeleton_Dead_Check()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CM_Skeleton_Archer * pGameObject_Skeleton_Archer = dynamic_cast<CM_Skeleton_Archer*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS,
		TEXT("Layer_Monster_Skeleton_Archer_1"), TEXT("Prototype_GameObject_M_Skeleton_Archer"), 0));

	if (nullptr == pGameObject_Skeleton_Archer)		// nullptr 이 5가가 되면 cage 를 삭제한다.
		m_arrSkeleton_Dead_Count[0] = true;

	pGameObject_Skeleton_Archer = dynamic_cast<CM_Skeleton_Archer*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS,
		TEXT("Layer_Monster_Skeleton_Archer_2"), TEXT("Prototype_GameObject_M_Skeleton_Archer"), 0));

	if (nullptr == pGameObject_Skeleton_Archer)
		m_arrSkeleton_Dead_Count[1] = true;

	pGameObject_Skeleton_Archer = dynamic_cast<CM_Skeleton_Archer*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS,
		TEXT("Layer_Monster_Skeleton_Archer_3"), TEXT("Prototype_GameObject_M_Skeleton_Archer"), 0));

	if (nullptr == pGameObject_Skeleton_Archer)
		m_arrSkeleton_Dead_Count[2] = true;

	CM_Skeleton_Shield * pGameObject_Skeleton_Shield = dynamic_cast<CM_Skeleton_Shield*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS,
		TEXT("Layer_Monster_Skeleton_Shield_1"), TEXT("Prototype_GameObject_M_Skeleton_Shield"), 0));

	if (nullptr == pGameObject_Skeleton_Shield)
		m_arrSkeleton_Dead_Count[3] = true;

	pGameObject_Skeleton_Shield = dynamic_cast<CM_Skeleton_Shield*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS,
		TEXT("Layer_Monster_Skeleton_Shield_2"), TEXT("Prototype_GameObject_M_Skeleton_Shield"), 0));

	if (nullptr == pGameObject_Skeleton_Shield)
		m_arrSkeleton_Dead_Count[4] = true;

	RELEASE_INSTANCE(CGameInstance);
}

CBoss_S_Cage * CBoss_S_Cage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_S_Cage*		pInstance = new CBoss_S_Cage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_S_Cage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_S_Cage::Clone(void * pArg)
{
	CBoss_S_Cage*		pInstance = new CBoss_S_Cage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_S_Cage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_S_Cage::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
