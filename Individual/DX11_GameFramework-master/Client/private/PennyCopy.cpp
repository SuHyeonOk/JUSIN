#include "stdafx.h"
#include "..\public\PennyCopy.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "M_Penny.h"

CPennyCopy::CPennyCopy(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPennyCopy::CPennyCopy(const CPennyCopy & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPennyCopy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPennyCopy::Initialize(void * pArg)
{
	m_wsTag = L"Not_UI";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.7f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;
	m_f4CenterPos = _float4(f3Pos.x, f3Pos.y, f3Pos.z, 1.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	m_eState = MOVE;

	return S_OK;
}

void CPennyCopy::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	PennyDead_Check();
	Anim_Change(TimeDelta);
}

void CPennyCopy::Late_Tick(_double TimeDelta)
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

HRESULT CPennyCopy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (DIE == m_eState)
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

void CPennyCopy::On_Collision(CGameObject * pOther)
{
	if (CObj_Manager::PLAYERINFO::STATE::ATTACK == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		if (L"Player_Weapon" == pOther->Get_Tag())
			m_eState = HIT;
	}

	if (L"Skill_Paint" == pOther->Get_Tag())
		m_eState = HIT;
}

HRESULT CPennyCopy::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Penny"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.7f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPennyCopy::SetUp_ShaderResources()
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

	if (DIE == m_eState)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
			return E_FAIL;
	}
	
	return S_OK;
}

void CPennyCopy::Anim_Change(const _double & TimeDelta)
{
	switch (m_eState)
	{
	case Client::CPennyCopy::IDLE:
	{
		m_pModelCom->Set_AnimIndex(2, false);

		if (m_pModelCom->Animation_Check(2) && m_pModelCom->Get_Finished())
			m_eState = MOVE;
	}
		break;
	case Client::CPennyCopy::MOVE:
	{
		m_pModelCom->Set_AnimIndex(5, false);
		Move_Tick(TimeDelta);
	}
		break;
	case Client::CPennyCopy::HIT:
	{
		m_pModelCom->Set_AnimIndex(3, false);

		if (m_pModelCom->Animation_Check(3) && m_pModelCom->Get_Finished())
			m_eState = MOVE;
	}
		break;
	case Client::CPennyCopy::DIE:
	{
		m_pModelCom->Set_AnimIndex(3, false);
		Die_Tick(TimeDelta);
	}
		break;
	}
}

void CPennyCopy::Move_Tick(const _double & TimeDelta)
{
	// 내 원점 거리와 내 위치가 멀다면! 무조건 원점으로 돌아간다.
	_vector	vCenterPos = XMLoadFloat4(&m_f4CenterPos);
	_vector vDistance = vCenterPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float	fDiatance = XMVectorGetX(XMVector3Length(vDistance));

	if (3.6f < fDiatance)
	{
		m_pTransformCom->Chase(vCenterPos, TimeDelta);
		m_pTransformCom->LookAt(vCenterPos);
	}
	else
	{
		if (!Random_Move(TimeDelta, 3.5f))
			m_eState = IDLE;
	}
}

void CPennyCopy::Die_Tick(const _double & TimeDelta)
{
	if (0.0f < m_fAlpha)
		m_fAlpha -= _float(TimeDelta);
	else
		CGameObject::Set_Dead();
}

_bool CPennyCopy::Random_Move(_double TimeDelta, _float fRange)
{
	if (!m_bRandomTime)
	{
		m_bRandomTime = true;
		m_bRandom_TimeAcc = _double(CUtilities_Manager::GetInstance()->Get_Random(1.5f, 3.0f));
	}

	// 이동 시킬 시간을 담는다.
	m_bRandomMove_TimeAcc += TimeDelta;
	if (m_bRandom_TimeAcc < m_bRandomMove_TimeAcc)
	{
		m_bRandomMove_TimeAcc = 0;
		m_bRandomTime = false;
		return false;
	}

	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vCenterPos = XMLoadFloat4(&m_f4CenterPos);
	_vector vDistance = vCenterPos - vMyPos;	// 내 원점 - 현재 위치
	_float	fDiatance = XMVectorGetX(XMVector3Length(vDistance));

	m_pTransformCom->Go_Straight(TimeDelta);

	if (fRange < fDiatance)	// 일정 범위를 나가면
	{
		m_pTransformCom->Chase(vCenterPos, TimeDelta);	// 원점으로 돌아가고
		_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), fRandomAxis);	// Look 을 변경한다.
	}

	return true;
}

void CPennyCopy::PennyDead_Check()
{
	if (DIE == m_eState)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CM_Penny * pGameObject = dynamic_cast<CM_Penny*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON, TEXT("Layer_Penny"), TEXT("Prototype_GameObject_M_Penny"), 0));

	if (nullptr == pGameObject)
		m_eState = DIE;

	RELEASE_INSTANCE(CGameInstance);
}

CPennyCopy * CPennyCopy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPennyCopy*		pInstance = new CPennyCopy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPennyCopy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPennyCopy::Clone(void * pArg)
{
	CPennyCopy*		pInstance = new CPennyCopy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPennyCopy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPennyCopy::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
