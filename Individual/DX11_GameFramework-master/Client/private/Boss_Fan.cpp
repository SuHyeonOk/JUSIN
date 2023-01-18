#include "stdafx.h"
#include "..\public\Boss_Fan.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Effect_Manager.h"

CBoss_Fan::CBoss_Fan(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CBoss_Fan::CBoss_Fan(const CBoss_Fan & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBoss_Fan::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Fan::Initialize(void * pArg)
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

	m_eState	= DANCE;
	m_fHP		= 30.0f;

	return S_OK;
}

void CBoss_Fan::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_Y))
	{
		CGameObject::Set_Dead();
	}

	RELEASE_INSTANCE(CGameInstance);

	Monster_Tick(TimeDelta);
}

void CBoss_Fan::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CBoss_Fan::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (CBoss_Fan::STATE::DIE == m_eState)
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

			if (i == 0)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
		}

		return S_OK;	// 죽었다면, 여기까지 진행하고 return
	}

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 0)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
		{
			if (m_bShader_Hit)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
		}
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

void CBoss_Fan::On_Collision(CGameObject * pOther)
{
	if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return;

	if (L"Player_Weapon" == pOther->Get_Tag())
		m_eState = HIT;
}

HRESULT CBoss_Fan::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_M_Skeleton_1"), TEXT("Com_Model"), 
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 1.2f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Fan::SetUp_ShaderResources()
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

	if (STATE::DIE == m_eState)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
			return E_FAIL;
	}

	return S_OK;
}

void CBoss_Fan::Monster_Tick(const _double & TimeDelta)
{
	if (0 >= m_fHP)
		m_eState = CBoss_Fan::DIE;

	switch (m_eState)
	{
	case Client::CBoss_Fan::DANCE:
		m_pModelCom->Set_AnimIndex(2);
		Dance_Tick(TimeDelta);
		break;
	case Client::CBoss_Fan::HIT:
		m_pModelCom->Set_AnimIndex(5);
		Hit_Tick(TimeDelta);
		break;
	case Client::CBoss_Fan::DIE:
		m_pModelCom->Set_AnimIndex(8);
		Die(TimeDelta);
		break;
	}
}

void CBoss_Fan::Dance_Tick(const _double& TimeDelta)
{
	// 이펙트
	m_bEffect_TimeAcc += TimeDelta;
	if (0.3 < m_bEffect_TimeAcc)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4Pos, vPos);

		CEffect_Manager::GetInstance()->Effect_Color_Boss_Smoke_Create(_float3(f4Pos.x, f4Pos.y + 1.0f, f4Pos.z), _float3(0.6f, 0.5f, 1.0f));
		CEffect_Manager::GetInstance()->Effect_Color_Skeleeton_Create(_float3(f4Pos.x, f4Pos.y + 1.0f, f4Pos.z), _float3(0.7f, 0.1f, 1.0f));

		m_bEffect_TimeAcc = 0;
	}
}

void CBoss_Fan::Hit_Tick(const _double & TimeDelta)
{
	m_bShader_Hit = true;

	if (0 == m_dShader_Hit_TimeAcc)
		m_fHP -= CObj_Manager::GetInstance()->Get_Current_Player().fAttack;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (0.7 < m_dShader_Hit_TimeAcc)
	{
		m_dShader_Hit_TimeAcc = 0;
		m_bShader_Hit = false;
		m_eState = DANCE;
	}
}

void CBoss_Fan::Die(const _double & TimeDelta)
{
	if (0.0f >= m_fAlpha)
	{
		// 알파값이 다 사라지면 죽음
		CGameObject::Set_Dead();
	}

	if (0.0f < m_fAlpha)															// 알파값 줄어 들도록
		m_fAlpha -= _float(TimeDelta);

	if (5 != m_iDieEffect_Count)													// 이펙트 5개
	{
		++m_iDieEffect_Count;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CE_DieCenter::DIECENTERINFO tDieCenterInfo;
		tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::GRAY;
		tDieCenterInfo.f3Pos = _float3(vf4MyPos.x, vf4MyPos.y + 1.0f, vf4MyPos.z - 0.7f);
		CEffect_Manager::GetInstance()->DieCenter_Create(tDieCenterInfo);
	}
}

CBoss_Fan * CBoss_Fan::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Fan*		pInstance = new CBoss_Fan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Fan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_Fan::Clone(void * pArg)
{
	CBoss_Fan*		pInstance = new CBoss_Fan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Fan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Fan::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
