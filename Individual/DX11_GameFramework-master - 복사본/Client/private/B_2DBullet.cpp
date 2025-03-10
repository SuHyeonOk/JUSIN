#include "stdafx.h"
#include "..\public\B_2DBullet.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Obj_Manager.h"
#include "Effect_Manager.h"

#include "E_Pigs_Rainbow.h"

CB_2DBullet::CB_2DBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CB_2DBullet::CB_2DBullet(const CB_2DBullet & rhs)
	: CGameObject(rhs)
{

}

HRESULT CB_2DBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_2DBullet::Initialize(void * pArg)
{	
	m_wsTag = L"2DBullet";

	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(BULLETINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::STAR_BULLET)
	{
		//// Effect
		//CE_Pigs_Rainbow::EFFECTINFO tEffectInfo;
		//tEffectInfo.m_pTransformCom = m_pTransformCom;
		//tEffectInfo.f3Pos = _float3(m_tBulletInfo.f3Start_Pos.x, m_tBulletInfo.f3Start_Pos.y - 0.3f, m_tBulletInfo.f3Start_Pos.z); 
		//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_PigsEffect"), TEXT("Prototype_GameObject_E_Pigs_Rainbow"), &tEffectInfo)))
		//	return E_FAIL;

		pGameInstance->Play_Sound(TEXT("sfx_pigs_attack_1.ogg"), 0.5f);

		GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = m_tBulletInfo.f3Start_Pos;
	}
	else if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::CIRCLE_BULLET)
	{
		pGameInstance->Play_Sound(TEXT("sfx_enchiridion_paintworks.ogg"), 0.5f);

		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Start_Pos.x, m_tBulletInfo.f3Start_Pos.y, m_tBulletInfo.f3Start_Pos.z);
	}
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (BULLETINFO::TOODYBULLET::STAR_BULLET == m_tBulletInfo.eToodyBullet)
		m_pTransformCom->Set_Scaled(_float3(0.4f, 0.4f, 1.f));
	//else if (BULLETINFO::TOODYBULLET::CIRCLE_BULLET == m_tBulletInfo.eToodyBullet)
	//	m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 1.f));

	// 처음 한 번만 플레이어를 향하는 벡터를 구한다.
	_vector vPlayerPos = XMVectorSet(m_tBulletInfo.f3Target_Pos.x, m_tBulletInfo.f3Target_Pos.y, m_tBulletInfo.f3Target_Pos.z, 1.f);
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = vPlayerPos - vMyPos;
	XMStoreFloat4(&m_f4Distance, vDistance);

	return S_OK;
}

void CB_2DBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 플레이어의 몇 틱 전의 좌표를 받아와서 총알을 날린다.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);		// 카메라를 바라본다.

	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = XMLoadFloat4(&m_f4Distance);
	vMyPos += XMVector3Normalize(vDistance) * 4.f * _float(TimeDelta);

 	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);	// 플레이어의 이전 프레임으로 날라간다.
	
	// 이펙트
	if (BULLETINFO::TOODYBULLET::CIRCLE_BULLET == m_tBulletInfo.eToodyBullet)
	{
		m_dEffect_TimeAcc += TimeDelta;
		if (0.04 < m_dEffect_TimeAcc)
		{
			_float4 f4MyPos;
			XMStoreFloat4(&f4MyPos, vMyPos);

			if (0 == m_tBulletInfo.iCircle_Color)		// 파란색
				CEffect_Manager::GetInstance()->Effect_Ink(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(0.09f, 0.46f, 0.76f));
			else if (1 == m_tBulletInfo.iCircle_Color)	// 빨간색
				CEffect_Manager::GetInstance()->Effect_Ink(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(0.89f, 0.25f, 0.14f));
			else if (2 == m_tBulletInfo.iCircle_Color)	// 노란색
				CEffect_Manager::GetInstance()->Effect_Ink(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(1.0f, 0.67f, 0.0f));

			m_dEffect_TimeAcc = 0;
		}
	}
	else if (BULLETINFO::TOODYBULLET::STAR_BULLET == m_tBulletInfo.eToodyBullet)
	{
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		m_dEffect_TimeAcc += TimeDelta;
		if (0.1 < m_dEffect_TimeAcc)
		{
			CEffect_Manager::GetInstance()->Effect_Pigs_Star({ f4MyPos.x, f4MyPos.y, f4MyPos.z });
			m_dEffect_TimeAcc = 0;
		}
	}
}

void CB_2DBullet::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 총알이 생성된 뒤 1초가 지나면 삭제한다.
	m_dBullet_TimeAcc += TimeDelta;
	if (1 < m_dBullet_TimeAcc)
	{
		_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::CIRCLE_BULLET)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("sfx_enchiridion_paintworks2.ogg"), 0.5f);
			RELEASE_INSTANCE(CGameInstance);

			if (0 == m_tBulletInfo.iCircle_Color)		// 파란색
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(0.09f, 0.46f, 0.76f));
			else if (1 == m_tBulletInfo.iCircle_Color)	// 빨간색
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(0.89f, 0.25f, 0.14f));
			else if (2 == m_tBulletInfo.iCircle_Color)	// 노란색
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(1.0f, 0.67f, 0.0f));
		}

		CGameObject::Set_Dead();
		m_dBullet_TimeAcc = 0;
	}

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CB_2DBullet::Render()
{
	if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::CIRCLE_BULLET)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);	// pass

	m_pVIBufferCom->Render();

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CB_2DBullet::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		CGameObject::Set_Dead();

		if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::CIRCLE_BULLET)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("sfx_enchiridion_paintworks2.ogg"), 0.5f);
			RELEASE_INSTANCE(CGameInstance);

			_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 f4MyPos;
			XMStoreFloat4(&f4MyPos, vMyPos);

			if (0 == m_tBulletInfo.iCircle_Color)		// 파란색
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(0.09f, 0.46f, 0.76f));
			else if (1 == m_tBulletInfo.iCircle_Color)	// 빨간색
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(0.89f, 0.25f, 0.14f));
			else if (2 == m_tBulletInfo.iCircle_Color)	// 노란색
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), _float3(1.0f, 0.67f, 0.0f));


			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STUN);
		}
		else
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::HIT);

		CObj_Manager::GetInstance()->Set_Interaction(true);
		CObj_Manager::GetInstance()->Set_Player_MinusHP(m_tBulletInfo.fMonsterAttack);
	}
}

HRESULT CB_2DBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::STAR_BULLET)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Star"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	//else if (m_tBulletInfo.eToodyBullet == BULLETINFO::TOODYBULLET::CIRCLE_BULLET)
	//{
	//	/* For.Com_Texture */
	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Circle"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//		return E_FAIL;
	//}

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

HRESULT CB_2DBullet::SetUp_ShaderResources()
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
	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CB_2DBullet * CB_2DBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_2DBullet*		pInstance = new CB_2DBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_2DBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_2DBullet::Clone(void * pArg)
{
	CB_2DBullet*		pInstance = new CB_2DBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_2DBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_2DBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
