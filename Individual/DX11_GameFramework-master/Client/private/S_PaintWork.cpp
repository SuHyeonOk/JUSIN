#include "stdafx.h"
#include "..\public\S_PaintWork.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

CS_PaintWork::CS_PaintWork(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CS_PaintWork::CS_PaintWork(const CS_PaintWork & rhs)
	: CGameObject(rhs)
{
}

HRESULT CS_PaintWork::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_PaintWork::Initialize(void * pArg)
{
	m_wsTag = L"Skill_Paint";

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(PAINTWORKINFO));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Pos.x, m_tBulletInfo.f3Pos.y, m_tBulletInfo.f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	_vector vLook = XMLoadFloat4(&m_tBulletInfo.f4Look);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	// 총알 공격력 다르게 설정
	CObj_Manager::GetInstance()->Set_Player_Attack(CObj_Manager::GetInstance()->Get_Current_Player().fAttack * 1.25f);

	return S_OK;
}

void CS_PaintWork::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (!m_bMove)
	{
		m_pTransformCom->Go_Right(TimeDelta * 0.4);

		m_dMove_TimeAcc += TimeDelta;
		if (0.4 < m_dMove_TimeAcc)
		{
			m_bMove = true;

			m_dMove_TimeAcc = 0;
		}
	}
	else
	{
		m_pTransformCom->Go_Left(TimeDelta * 0.4);

		m_dMove_TimeAcc += TimeDelta;
		if (0.4 < m_dMove_TimeAcc)
		{
			m_bMove = false;

			m_dMove_TimeAcc = 0;
		}
	}
	m_pTransformCom->Go_Straight(TimeDelta);

	if (CObj_Manager::PLAYERINFO::IDLE == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		CUI_Manager::GetInstance()->Set_Ui_Monster(false);		// 몬스터 UI 끄기
}

void CS_PaintWork::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 일정시간 뒤 삭제
	m_dBullet_TimeAcc += TimeDelta;
	if (1.25 < m_dBullet_TimeAcc)
	{
		// 이펙트
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::BLUE == m_tBulletInfo.ePaintWork)
		{
			CEffect_Manager::GetInstance()->Effect_Paint_Circle_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z), _float3(0.3f, 0.8f, 1.0f));
			CEffect_Manager::GetInstance()->Effect_Paint_Firecracker_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.1f), _float3(0.3f, 1.0f, 1.0f));
			CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.2f), _float3(0.3f, 1.0f, 1.0f));
		}
		else if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::MAGENTA == m_tBulletInfo.ePaintWork)
		{
			CEffect_Manager::GetInstance()->Effect_Paint_Circle_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z), _float3(0.9f, 0.5f, 1.0f));
			CEffect_Manager::GetInstance()->Effect_Paint_Firecracker_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.1f), _float3(0.6f, 0.0f, 0.9f));
			CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.2f), _float3(0.6f, 0.0f, 0.9f));
		}
		else if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::YELLOW == m_tBulletInfo.ePaintWork)
		{
			CEffect_Manager::GetInstance()->Effect_Paint_Circle_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z), _float3(0.9f, 1.0f, 0.0f));
			CEffect_Manager::GetInstance()->Effect_Paint_Firecracker_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.1f), _float3(0.9f, 1.0f, 0.6f));
			CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.2f), _float3(0.9f, 1.0f, 0.6f));
		}

		// 죽을 때 원래 공격력으로 변경
		CObj_Manager::GetInstance()->Set_Player_Attack(m_tBulletInfo.fAttack);

		CGameObject::Set_Dead();
		m_dBullet_TimeAcc = 0;
	}

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CS_PaintWork::Render()
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

void CS_PaintWork::On_Collision(CGameObject * pOther)
{
	if (L"Gary_Boss" != pOther->Get_Tag())
	{
		CUI_Manager::GetInstance()->Set_Ui_Monster(true);
		CUI_Manager::GetInstance()->UI_Monster_Index(pOther);
	}

	// 이펙트
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::BLUE == m_tBulletInfo.ePaintWork)
	{
		CEffect_Manager::GetInstance()->Effect_Paint_Circle_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z), _float3(0.3f, 0.8f, 1.0f));
		CEffect_Manager::GetInstance()->Effect_Paint_Firecracker_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.1f), _float3(0.3f, 1.0f, 1.0f));
		CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.2f), _float3(0.3f, 1.0f, 1.0f));
	}
	else if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::MAGENTA == m_tBulletInfo.ePaintWork)
	{
		CEffect_Manager::GetInstance()->Effect_Paint_Circle_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z), _float3(0.9f, 0.5f, 1.0f));
		CEffect_Manager::GetInstance()->Effect_Paint_Firecracker_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.1f), _float3(0.6f, 0.0f, 0.9f));
		CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.2f), _float3(0.6f, 0.0f, 0.9f));
	}
	else if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::YELLOW == m_tBulletInfo.ePaintWork)
	{
		CEffect_Manager::GetInstance()->Effect_Paint_Circle_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z), _float3(0.9f, 1.0f, 0.0f));
		CEffect_Manager::GetInstance()->Effect_Paint_Firecracker_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.1f), _float3(0.9f, 1.0f, 0.6f));
		CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.2f), _float3(0.9f, 1.0f, 0.6f));
	}

	CGameObject::Set_Dead();
}

HRESULT CS_PaintWork::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::BLUE == m_tBulletInfo.ePaintWork)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Blue"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::MAGENTA == m_tBulletInfo.ePaintWork)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Magenta"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (CS_PaintWork::PAINTWORKINFO::PAINTWORK::YELLOW == m_tBulletInfo.ePaintWork)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Yellow"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	
	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y - 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_PaintWork::SetUp_ShaderResources()
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

CS_PaintWork * CS_PaintWork::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_PaintWork*		pInstance = new CS_PaintWork(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_PaintWork");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_PaintWork::Clone(void * pArg)
{
	CS_PaintWork*		pInstance = new CS_PaintWork(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_PaintWork");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_PaintWork::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
