#include "stdafx.h"
#include "..\public\Squirrel.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CSquirrel::CSquirrel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CSquirrel::CSquirrel(const CSquirrel & rhs)
	: CGameObject(rhs)
{

}

HRESULT CSquirrel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSquirrel::Initialize(void * pArg)
{	
	m_wsTag = L"Squirrel";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.5f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;
	m_f4Start_Position = { f3Pos.x, f3Pos.y, f3Pos.z, 1.0f };

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled({ 1.5f, 1.5f, 1.5f });
	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(270.0f));

	m_pModelCom->Set_AnimIndex(6);

	m_eState = IDLE;
	m_eAnimState = IDLE;

	return S_OK;
}

void CSquirrel::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	State_Tick(TimeDelta);
	AnimatedMovie_Tick();
}

void CSquirrel::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CSquirrel::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if(0 == i)
			m_pModelCom->Render(m_pShaderCom, 0, "g_BoneMatrices", 1);
		else
			m_pModelCom->Render(m_pShaderCom, 1, "g_BoneMatrices");
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

void CSquirrel::On_Collision(CGameObject * pOther)
{
	//if (L"Finn" == pOther->Get_Tag())
	//{
	//	m_eState = ATTACK;
	//}
}

HRESULT CSquirrel::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Squirrel"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.0f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSquirrel::SetUp_ShaderResources()
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

void CSquirrel::AnimatedMovie_Tick()
{
	// 0 : 공격
	// 1 : 춤
	// 2 : 죽는다
	// 3, 4 : 공격 받는다
	// 5 : 쿰척
	// 6 : 가만히
	// 7 : 달리기
	
	switch (m_eAnimState)
	{
	case Client::CSquirrel::IDLE:
		m_pModelCom->Set_AnimIndex(6);
		break;
	case Client::CSquirrel::ATTACK:
		m_pModelCom->Set_AnimIndex(0, false);
		break;
	case Client::CSquirrel::MOVE:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case Client::CSquirrel::DANCE:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case Client::CSquirrel::EAT:
		m_pModelCom->Set_AnimIndex(5);
		break;
	}
}

void CSquirrel::State_Tick(const _double & TimeDelta)
{
	switch (m_eState)
	{
	case Client::CSquirrel::IDLE:
		Idle_Tick();
		break;
	case Client::CSquirrel::MOVE:
		Move_Tick(TimeDelta);
		break;
	case Client::CSquirrel::ATTACK:
		Attack_Tick(TimeDelta);
		break;
	case Client::CSquirrel::EAT:
		Eat_Tick(TimeDelta);
		break;
	}
}

void CSquirrel::Idle_Tick()
{
	m_eAnimState = IDLE;

	// 주변에 플레이어가 있는지 확인한다.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pPlayer_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_MINIGAME, TEXT("Layer_FinnAndJake"), TEXT("Com_Transform"), 0));
	RELEASE_INSTANCE(CGameInstance);

	_vector	vDistance = pPlayer_TransformCom->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	// 거리 안 으로 들어왔다면 공격 상태로 들어간다.
	if (3.0f > fDistance)
	{
		m_eState = ATTACK;
	}
}

void CSquirrel::Move_Tick(const _double & TimeDelta)
{
	m_eAnimState = MOVE;

	// 시작지점으로 돌아간다.
	_vector vStart_Position = XMLoadFloat4(&m_f4Start_Position);
	m_pTransformCom->LookAt(vStart_Position);
	m_pTransformCom->Chase(vStart_Position, TimeDelta);

	_vector	vDistance = vStart_Position - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	// 시작 지점 근처로 오게되면 상태를 변경한다.
	if (0.1f > fDistance)
	{
		m_eState = IDLE;
	}
}

void CSquirrel::Attack_Tick(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pPlayer_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_MINIGAME, TEXT("Layer_FinnAndJake"), TEXT("Com_Transform"), 0));
	_vector vPlayerPosition = pPlayer_TransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);
	
	// 플레이어를 바라본다.
	m_pTransformCom->LookAt(vPlayerPosition);

	// 플레이어와 몬스터 사이의 거리를 구한다.
	_vector	vDistance = vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	// 플레이어를 향해...
	if (0.5f > fDistance)
	{
		// 플레이어를 향해 공격한다.
		m_eAnimState = ATTACK;

		// TODO 플레이어 hit 되는지 확인 이 몬스터 애니메이션 끝나는지 확인
		if (true == m_pModelCom->Get_Finished())
		{
			m_eState = MOVE;		// 시작 위치로 돌아간다.
		}
	}
	else
	{
		// 플레이어에게 다가간다.
		m_eAnimState = MOVE;
		m_pTransformCom->Chase(vPlayerPosition, TimeDelta);
	}
}

void CSquirrel::Eat_Tick(const _double & TimeDelta)
{

}

CSquirrel * CSquirrel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSquirrel*		pInstance = new CSquirrel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSquirrel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSquirrel::Clone(void * pArg)
{
	CSquirrel*		pInstance = new CSquirrel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSquirrel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSquirrel::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
