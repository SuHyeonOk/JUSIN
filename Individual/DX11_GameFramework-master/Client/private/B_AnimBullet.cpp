#include "stdafx.h"
#include "..\public\B_AnimBullet.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CB_AnimBullet::CB_AnimBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CB_AnimBullet::CB_AnimBullet(const CB_AnimBullet & rhs)
	: CGameObject(rhs)
{

}

HRESULT CB_AnimBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_AnimBullet::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(ANIMBULLETINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS)	// 요기 (검색해서 각기 설정해 주면 된다.)
	{
		m_wsTag = L"3DBullet_Roots";
		GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Start_Pos.x, m_tBulletInfo.f3Start_Pos.y, m_tBulletInfo.f3Start_Pos.z);
	}


	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS)
		m_pTransformCom->Rotation(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(-90.f));

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	// 처음 한 번만 플레이어를 향하는 벡터를 구한다.
	_vector vPlayerPos = XMVectorSet(m_tBulletInfo.f3Target_Pos.x, m_tBulletInfo.f3Target_Pos.y, m_tBulletInfo.f3Target_Pos.z, 1.f);
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = vPlayerPos - vMyPos;
	XMStoreFloat4(&m_f4Distance, vDistance);
	
	return S_OK;
}

void CB_AnimBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CB_AnimBullet::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_pModelCom->Get_Finished())		// 플레이어랑 충돌하면 애니메이션이 끝나고 사라진다.
		CGameObject::Set_Dead();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CB_AnimBullet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

void CB_AnimBullet::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		CObj_Manager::GetInstance()->Set_Player_MinusHp(m_tBulletInfo.iMonsterAttack);

		if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::HIT);
	}
}

HRESULT CB_AnimBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROOTS) // 요기
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Tree_Witch_Roots_FX"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;

		ColliderDesc.vSize = _float3(3.f, 3.f, 3.f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_AnimBullet::SetUp_ShaderResources()
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

CB_AnimBullet * CB_AnimBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_AnimBullet*		pInstance = new CB_AnimBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_AnimBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_AnimBullet::Clone(void * pArg)
{
	CB_AnimBullet*		pInstance = new CB_AnimBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_AnimBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_AnimBullet::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}
