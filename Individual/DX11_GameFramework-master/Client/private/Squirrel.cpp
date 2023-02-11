#include "stdafx.h"
#include "..\public\Squirrel.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "Korean_Food.h"
#include "UI_3DTexture.h"

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

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.5f;
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
	if (true == m_bEat) // 음식을 받았다면 더 이상 움직이지 않고, 음식만 먹는다.
	{
		Eat_Tick(TimeDelta);
		return;
	}

	__super::Tick(TimeDelta);

	State_Tick(TimeDelta);
	AnimatedMovie_Tick();
}

void CSquirrel::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (false == m_bEat)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}

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
	if (L"Korean_Food_Squirrel" == pOther->Get_Tag())
	{
		m_eState = EAT;
	}
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
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.5f);

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
		m_pModelCom->Set_AnimIndex(1, false);
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
		Find_Tick();
		m_eState = ATTACK;
	}
}

void CSquirrel::Find_Tick()
{
	if (true == m_bFindUI)
		return;

	m_bFindUI = true;

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_FIND;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z - 0.5f);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Find_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CSquirrel::Move_Tick(const _double & TimeDelta)
{
	m_dFind_TImeAcc += TimeDelta;
	if (1.0 > m_dFind_TImeAcc)
		return;

	m_bFindUI = false;
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
		m_dFind_TImeAcc = 0.0;
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
		m_eAnimState = MOVE;

		_vector	vStartDistance = XMLoadFloat4(&_float4(m_f4Start_Position.x, m_f4Start_Position.y, m_f4Start_Position.z, m_f4Start_Position.w)) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float fStartDistance = XMVectorGetX(XMVector3Length(vStartDistance));

		if (3.0f < fStartDistance)											// 시작점에서 몬스터가 멀어지면 다시
		{
			m_eState = MOVE;												// 시작 위치로 돌아간다.
		}
		else
			m_pTransformCom->Chase(vPlayerPosition, TimeDelta);				// 플레이어에게 다가간다.
	}
}

void CSquirrel::Eat_Tick(const _double & TimeDelta)
{
	if (false == m_bEat)
	{
		m_bEat = true;

		//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		//_float4 f4Position = { 0.0f, 0.0f, 0.0f, 1.0f };
		//XMStoreFloat4(&f4Position, vPosition);

		//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		//XMVector3Normalize(vLook * 0.1f);
		//_float4 f4Look = { 0.0f, 0.0f, 0.0f, 1.0f };
		//XMStoreFloat4(&f4Look, vLook);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 내 Look 을 기준으로 거리 1만큼을 더 한 위치에 생성 시키고 싶다.
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float4 f4Look;
		XMStoreFloat4(&f4Look, XMVector3Normalize(vLook) * 0.3f);

		CKorean_Food::OBJECTINFO	m_Korean_Food;
		m_Korean_Food.eType = CKorean_Food::SQUIRREL;
		m_Korean_Food.f3Position = _float3(f4MyPos.x + f4Look.x, f4MyPos.y + f4Look.y, f4MyPos.z + f4Look.z);
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Korean_Food"), TEXT("Prototype_GameObject_Korean_Food"), &m_Korean_Food)))
			return;
		RELEASE_INSTANCE(CGameInstance);
	}


	if (3 > m_iDance_Count)
	{
		m_eAnimState = DANCE;

		if (1 == m_pModelCom->Get_AnimIndex() && true == m_pModelCom->Get_Finished())
		{
			++m_iDance_Count;
		}
	}
	else
	{
		m_pModelCom->Set_AnimIndex(5);
	}
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
