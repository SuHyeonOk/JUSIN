#include "stdafx.h"
#include "..\public\Finn_Change.h"

#include "GameInstance.h"
#include "Bone.h"

#include "Obj_Manager.h"
#include "Skill_Manager.h"

CFinn_Change::CFinn_Change(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CFinn_Change::CFinn_Change(const CFinn_Change & rhs)
	: CGameObject(rhs)
{

}

HRESULT CFinn_Change::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFinn_Change::Initialize(void * pArg)
{
	if (CHANGEDESC::CHANGE::MAGIC == m_tChangeDesc.eChange)
		m_wsTag = L"Finn_Magic";

	if (nullptr != pArg)
		memcpy(&m_tChangeDesc, pArg, sizeof(m_tChangeDesc));

	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_eState = IDLE;
	m_pModelCom->Set_AnimIndex(5);

	return S_OK;
}

void CFinn_Change::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CFinn_Change::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	_matrix			SocketMatrix = m_tChangeDesc.pSocket->Get_OffsetMatrix() *
		m_tChangeDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_tChangeDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = SocketMatrix * m_tChangeDesc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix() * SocketMatrix);


	//switch (CSkill_Manager::GetInstance()->Get_Player_Skill().eMagic)
	//{
	//case CSkill_Manager::PLAYERSKILL::IDLE:
	//	m_pModelCom->Set_AnimIndex(4);
	//	break;
	//case CSkill_Manager::PLAYERSKILL::RUN:
	//	m_pModelCom->Set_AnimIndex(5);
	//	break;
	//}

	

	//switch (CObj_Manager::GetInstance()->Get_Current_Player().eState)
	//{
	//case CObj_Manager::PLAYERINFO::STATE::IDLE:
	//	m_pModelCom->Set_AnimIndex(4);
	//	break;
	//case CObj_Manager::PLAYERINFO::STATE::RUN:
	//	m_pModelCom->Set_AnimIndex(5);
	//	break;
	//case CObj_Manager::PLAYERINFO::STATE::ATTACK:
	//	m_pModelCom->Set_AnimIndex(0, false);
	//	break;
	//case CObj_Manager::PLAYERINFO::STATE::HIT:
	//	m_pModelCom->Set_AnimIndex(2, false);
	//	break;
	//}







	if (CObj_Manager::PLAYERINFO::PLAYER::FINN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT   CFinn_Change::Render()
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

		m_pModelCom->Render(m_pShaderCom, i, nullptr, 1);
	}

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

void CFinn_Change::On_Collision(CGameObject * pOther)
{

}

HRESULT CFinn_Change::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	if (CHANGEDESC::CHANGE::MAGIC == m_tChangeDesc.eChange)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Magic_Man_Finn"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_SPHERE */
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(1.2f, 1.2f, 1.2f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	}


	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFinn_Change::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// g_WorldMatrix * SocketMatrix;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;


	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CFinn_Change * CFinn_Change::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFinn_Change*		pInstance = new CFinn_Change(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFinn_Change");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFinn_Change::Clone(void * pArg)
{
	CFinn_Change*		pInstance = new CFinn_Change(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFinn_Change");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFinn_Change::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_tChangeDesc.pSocket);
		Safe_Release(m_tChangeDesc.pTargetTransform);
	}

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
