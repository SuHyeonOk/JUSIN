#include "stdafx.h"
#include "..\public\B_3DAnimBullet.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CB_3DAnimBullet::CB_3DAnimBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CB_3DAnimBullet::CB_3DAnimBullet(const CB_3DAnimBullet & rhs)
	: CGameObject(rhs)
{

}

HRESULT CB_3DAnimBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_3DAnimBullet::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(ANIMBULLETINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS)	// 요기 (검색해서 각기 설정해 주면 된다.)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Pos.x, m_tBulletInfo.f3Pos.y, m_tBulletInfo.f3Pos.z);
	}

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS)
		m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CB_3DAnimBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CB_3DAnimBullet::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS) 
	{
		m_bPlayer_Collider = CObj_Manager::GetInstance()->Get_Player_Collider(&m_pColliderCom);

		m_bDead_TimeAcc += TimeDelta;
		if (1 < m_bDead_TimeAcc)
		{
			CGameObject::Set_Dead();
			m_bDead_TimeAcc = 0;
		}
	}

	//if (CObj_Manager::GetInstance()->Get_Player_Collider(&m_pColliderCom))
	//	CGameObject::Set_Dead();

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CB_3DAnimBullet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (1 == i) // 그림자 모델 없앰 (비중이 클 수록 모델 순서가 앞에 위치한다.)
			continue;

		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CB_3DAnimBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS) // 요기
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Tree_Witch_Roots_FX"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS)	// 요기
	{
		ColliderDesc.vSize = _float3(1.f, 0.5f, 1.f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_3DAnimBullet::SetUp_ShaderResources()
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

CB_3DAnimBullet * CB_3DAnimBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_3DAnimBullet*		pInstance = new CB_3DAnimBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_3DAnimBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_3DAnimBullet::Clone(void * pArg)
{
	CB_3DAnimBullet*		pInstance = new CB_3DAnimBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_3DAnimBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_3DAnimBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
