#include "stdafx.h"
#include "..\public\B_3DBullet.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Effect_Manager.h"

CB_3DBullet::CB_3DBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CB_3DBullet::CB_3DBullet(const CB_3DBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CB_3DBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_3DBullet::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(NONANIMBULLETINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	m_wsTag = L"3DBullet";
	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = m_tBulletInfo.f3Start_Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)
		m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 0.5f));
	else
	{
		_vector vPlayerPos = XMVectorSet(m_tBulletInfo.f3Target_Pos.x, m_tBulletInfo.f3Target_Pos.y, m_tBulletInfo.f3Target_Pos.z, 1.f);
		_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vDistance = vPlayerPos - vMyPos;
		XMStoreFloat4(&m_f4Distance, vDistance);
	}

	return S_OK;
}

void CB_3DBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Effect_Tick(TimeDelta);

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)	// 플레이어를 향해 회전하며 날아가는 총알
		Magic_Tick(TimeDelta);
	else
	{
		m_pTransformCom->Turn(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), TimeDelta * 15);

		_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vDistance = XMLoadFloat4(&m_f4Distance);
		vMyPos += XMVector3Normalize(vDistance) * 7.f * _float(TimeDelta);

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);	// 플레이어의 이전 프레임으로 날라간다.
	}
}

void CB_3DBullet::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 생성 후 일저시간 뒤 삭제
	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)	
		Magic_LateTick(TimeDelta);
	else
	{
		m_dBullet_TimeAcc += TimeDelta;
		if (1 < m_dBullet_TimeAcc)
		{
			if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROCK)
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float4 f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4(&f4MyPos, vMyPos);

				CEffect_Manager::GetInstance()->Effect_Boom_Fire_Create(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z - 0.5f));
				CEffect_Manager::GetInstance()->Effect_Smoke_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z - 0.5f), _float3(0.0f, 0.0f, 0.0f));
			}

			CGameObject::Set_Dead();
			m_dBullet_TimeAcc = 0;
		}
	}

	// 기본 적인 충돌 처리들
	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CB_3DBullet::Render()
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

		m_pModelCom->Render(m_pShaderCom, i);
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

void CB_3DBullet::On_Collision(CGameObject * pOther)
{
	if (L"Finn" != pOther->Get_Tag() || L"Jake" != pOther->Get_Tag())
	{
		CObj_Manager::GetInstance()->Set_Interaction(true);
		CGameObject::Set_Dead();

		if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)
		{
			if (CObj_Manager::PLAYERINFO::MAGIC != CObj_Manager::GetInstance()->Get_Current_Player().eState)
				CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::MAGIC);	// 플레이어 State 을 변경한다.
		}
		else if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROCK)
		{
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4(&f4MyPos, vMyPos);

			CEffect_Manager::GetInstance()->Effect_Boom_Fire_Create(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z - 0.5f));
			CEffect_Manager::GetInstance()->Effect_Color_Hit_Create(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z - 0.5f), _float3(1.0f, 0.0f, 0.0f));
			CEffect_Manager::GetInstance()->Effect_Smoke_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z - 0.5f), _float3(0.0f, 0.0f, 0.0f));

			CObj_Manager::GetInstance()->Set_Player_MinusHP(m_tBulletInfo.fMonsterAttack);
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::HIT);
		}
		else
		{
			CObj_Manager::GetInstance()->Set_Player_MinusHP(m_tBulletInfo.fMonsterAttack);
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::HIT);
		}
	}
}

HRESULT CB_3DBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (NONANIMBULLETINFO::BULLETTYPE::TYPE_MAGIC == m_tBulletInfo.eBulletType)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_FireDragon_Area_FX"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (NONANIMBULLETINFO::BULLETTYPE::TYPE_SKELETON == m_tBulletInfo.eBulletType)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_B_Bone"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (NONANIMBULLETINFO::BULLETTYPE::TYPE_ROCK == m_tBulletInfo.eBulletType)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_B_Boss_Fite_Rock"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	if (NONANIMBULLETINFO::BULLETTYPE::TYPE_MAGIC == m_tBulletInfo.eBulletType)
	{
		ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}
	else
	{
		ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_3DBullet::SetUp_ShaderResources()
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
void CB_3DBullet::Magic_Tick(const _double & TimeDelta)
{
	// Look 양의 방향으로 회전한다.
	m_pTransformCom->Go_Straight(TimeDelta * 2);
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta * 2);
}

void CB_3DBullet::Magic_LateTick(const _double & TimeDelta)
{
	// 10 초가 지나면 삭제한다.
	m_dBullet_TimeAcc += TimeDelta;
	if (10 < m_dBullet_TimeAcc)
	{
		CGameObject::Set_Dead();
		m_dBullet_TimeAcc = 0;
	}
}

void CB_3DBullet::Effect_Tick(const _double & TimeDelta)
{
	// 이펙트
	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_ROCK)
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4(&f4MyPos, vMyPos);

		m_dEffect_TimeAcc += TimeDelta;
		if (0.1 < m_dEffect_TimeAcc)
		{
			CEffect_Manager::GetInstance()->Effect_Small_Fire_Create(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z));
			m_dEffect_TimeAcc = 0;
		}
	}
}

CB_3DBullet * CB_3DBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_3DBullet*		pInstance = new CB_3DBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_3DBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_3DBullet::Clone(void * pArg)
{
	CB_3DBullet*		pInstance = new CB_3DBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_3DBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_3DBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
