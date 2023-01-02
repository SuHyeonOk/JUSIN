#include "stdafx.h"
#include "..\public\S_PaintWork.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

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
	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(PAINTWORKINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	m_wsTag = L"Skill_PaintWork";
	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Pos.x, m_tBulletInfo.f3Pos.y, m_tBulletInfo.f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	_vector vPlayerPos = XMVectorSet(m_tBulletInfo.f3Pos.x, m_tBulletInfo.f3Pos.y, m_tBulletInfo.f3Pos.z, 1.f);
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = vPlayerPos - vMyPos;
	XMStoreFloat4(&m_f4Distance, vDistance);

	return S_OK;
}

void CS_PaintWork::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//if (m_tBulletInfo.ePaintWork == m_tBulletInfo.BLUE)
	//{
	//	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(-20.f));
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}
	//else if (m_tBulletInfo.ePaintWork == m_tBulletInfo.MAGENTA)
	//{
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}
	//else if (m_tBulletInfo.ePaintWork == m_tBulletInfo.YELLOW)
	//{
	//	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(20.f));
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}
}

void CS_PaintWork::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	//// 일정시간 뒤 삭제
	//m_dBullet_TimeAcc += TimeDelta;
	//if (1 < m_dBullet_TimeAcc)
	//{
	//	CGameObject::Set_Dead();
	//	m_dBullet_TimeAcc = 0;
	//}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
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

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

void CS_PaintWork::On_Collision(CGameObject * pOther)
{

}

HRESULT CS_PaintWork::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tBulletInfo.ePaintWork == m_tBulletInfo.BLUE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Blue"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tBulletInfo.ePaintWork == m_tBulletInfo.MAGENTA)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Magenta"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tBulletInfo.ePaintWork == m_tBulletInfo.YELLOW)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Yellow"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	
	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

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
