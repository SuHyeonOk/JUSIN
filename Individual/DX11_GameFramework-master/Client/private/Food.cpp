#include "stdafx.h"   
#include "..\public\Food.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Effect_Manager.h"

#include "E_Burst.h"
#include "Utilities_Manager.h"
#include "ItemManager.h"

CFood::CFood(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CFood::CFood(const CFood & rhs)
	: CGameObject(rhs)
{

}

HRESULT CFood::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFood::Initialize(void * pArg)
{	
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (nullptr != pArg)
		memcpy(&m_tFoodInfo, pArg, sizeof(FOODINFO));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tFoodInfo.fPos.x, m_tFoodInfo.fPos.y, m_tFoodInfo.fPos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CFood::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);

	Effect_Create(TimeDelta);

	if (true == m_bNewFood)
	{
		if (FOODINFO::CHEWED == m_tFoodInfo.eFoodKind)
			Chewed_Food(TimeDelta);
		else if (FOODINFO::SOY_PEOPLE == m_tFoodInfo.eFoodKind)
			Soy_People(TimeDelta);
	}















	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_B))
	{
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4Pos;
		XMStoreFloat4(&f4Pos, vPlayerPos);

		CEffect_Manager::GetInstance()->Effect_Star_Create(_float3(f4Pos.x, f4Pos.y + 1.0f, f4Pos.z - 1.0f));
		CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4Pos.x, f4Pos.y + 1.0f, f4Pos.z - 1.1f));
	}

	if (pGameInstance->Key_Pressing(DIK_V))
	{
		m_dKeyDown_TimeAcc += TimeDelta;
		cout << m_dKeyDown_TimeAcc << endl;

		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4Pos;
		XMStoreFloat4(&f4Pos, vPlayerPos);
	}
	else
		m_dKeyDown_TimeAcc = 0;

	RELEASE_INSTANCE(CGameInstance);
}

void CFood::Late_Tick(_double TimeDelta)
{
	if (m_bPlayer_Collider)
		return;

	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_ITME, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CFood::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 1)
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 1);
		else
			m_pModelCom->Render(m_pShaderCom, i, nullptr);
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

void CFood::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		if (CObj_Manager::GetInstance()->Get_Current_Player().fHP >= CObj_Manager::GetInstance()->Get_Current_Player().fHPMax)
			return;	// 체력이 가득찬 상태에서는 아이템을 먹을 수 없다.

		if (!m_bPlayer_Collider)
		{
			m_bPlayer_Collider = true;

			if (FOODINFO::APPLE_PIE == m_tFoodInfo.eFoodKind)		// 그냥 체력 30 추가
				CObj_Manager::GetInstance()->Set_Player_PlusHP(30.0f);
			else if (FOODINFO::ROYAL_TART == m_tFoodInfo.eFoodKind)	// 전체 체력의 반 추가
				CObj_Manager::GetInstance()->Set_Player_PlusHP(CObj_Manager::GetInstance()->Get_Current_Player().fHPMax / 2.0f);
			else if (FOODINFO::BURRITO == m_tFoodInfo.eFoodKind)	// 전체 체력 추가
				CObj_Manager::GetInstance()->Set_Player_PlusHP(CObj_Manager::GetInstance()->Get_Current_Player().fHPMax);
			else
				m_bNewFood = true;
		}
	}
}

HRESULT CFood::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tFoodInfo.eFoodKind == FOODINFO::ROYAL_TART)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Royal_Tart"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tFoodInfo.eFoodKind == FOODINFO::BURRITO)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Burrito"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tFoodInfo.eFoodKind == FOODINFO::APPLE_PIE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Apple_Pie"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tFoodInfo.eFoodKind == FOODINFO::CHEWED)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chewed_Food"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tFoodInfo.eFoodKind == FOODINFO::SOY_PEOPLE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Soy_People"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

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

HRESULT CFood::SetUp_ShaderResources()
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

void CFood::Chewed_Food(const _double & TimeDelta)
{
	// 10초 동안 플레이어 체력 1씩 증가.

	m_dDead_TimeAcc += TimeDelta;
	if (10 < m_dDead_TimeAcc)
		CGameObject::Set_Dead();

	m_Food_TimeAcc += TimeDelta;
	if (1 < m_Food_TimeAcc)
	{
		CObj_Manager::GetInstance()->Set_Player_PlusHP(5.0f);
		m_Food_TimeAcc = 0;
	}

	// 이펙트
	m_dFoodHp_TimeAcc += TimeDelta;
	if (0.5 < m_dFoodHp_TimeAcc)
	{
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);
		CEffect_Manager::GetInstance()->Food_Hp(_float3(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z));
		m_dFoodHp_TimeAcc = 0;
	}
}

void CFood::Soy_People(const _double & TimeDelta)
{
	// 10초 동안 플레이어 체력 1씩 감소.

	m_dDead_TimeAcc += TimeDelta;
	if (10 < m_dDead_TimeAcc)
		CGameObject::Set_Dead();

	m_Food_TimeAcc += TimeDelta;
	if (1 < m_Food_TimeAcc)
	{
		CObj_Manager::GetInstance()->Set_Player_PlusHP(-5.0f);
		m_Food_TimeAcc = 0;
	}

	// 이펙트
	m_dFoodHp_TimeAcc += TimeDelta;
	if (0.5 < m_dFoodHp_TimeAcc)
	{
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);
		CEffect_Manager::GetInstance()->Food_MinusHp(_float3(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z));
		m_dFoodHp_TimeAcc = 0;
	}
}

void CFood::Effect_Create(const _double & TimeDelta)
{
	if (true == m_bNewFood)
		return;

	if (false == m_bPlayer_Collider)
		return;

	m_dDead_TimeAcc += TimeDelta;
	if (2 < m_dDead_TimeAcc)
		CGameObject::Set_Dead();

	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
	_float4 f4PlayerPos;
	XMStoreFloat4(&f4PlayerPos, vPlayerPos);

	// 링
	if (0 == m_dFoodUp_TimeAcc)	// 맨 처음 한 번 하려고
	{
		CEffect_Manager::GetInstance()->Food_Up(_float3(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z));
	}
	
	m_dFoodUp_TimeAcc += TimeDelta;
	if (0.3 < m_dFoodUp_TimeAcc)
	{
		CEffect_Manager::GetInstance()->Food_Up(_float3(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z));
		m_dFoodUp_TimeAcc = 0;
	}

	// 십자가
	m_dFoodHp_TimeAcc += TimeDelta;
	if (0.2 < m_dFoodHp_TimeAcc)
	{
		CEffect_Manager::GetInstance()->Food_Hp(_float3(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z));
		m_dFoodHp_TimeAcc = 0;
	}

	// 바닥에 이미지 변하는
	if (!m_bBeneficial)
	{
		m_bBeneficial = true;
		CEffect_Manager::GetInstance()->Beneficial(_float3(f4PlayerPos.x, 0.7f, f4PlayerPos.z), _float3(0.5f, 0.75f, 0.27f));
	}
}

CFood * CFood::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFood*		pInstance = new CFood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFood::Clone(void * pArg)
{
	CFood*		pInstance = new CFood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFood::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
