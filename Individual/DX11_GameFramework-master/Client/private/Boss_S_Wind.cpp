#include "stdafx.h"
#include "..\public\Boss_S_Wind.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CBoss_S_Wind::CBoss_S_Wind(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CBoss_S_Wind::CBoss_S_Wind(const CBoss_S_Wind & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBoss_S_Wind::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Wind::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tSkillInfo, pArg, sizeof(m_tSkillInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tSkillInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	_vector vLook = XMLoadFloat4(&m_tSkillInfo.f4Look) * -1.0f;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = m_pTransformCom->Get_Scaled();	
	_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(90.0f));

	vRight = vRight * vScale.x;
	vUp = vUp * vScale.y;		
	vLook = vRight * vScale.z;	

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));

	m_fAlpha = 1.0f;
	m_fSizeX = 1.0f;
	m_fSizeY = 1.0f;

	return S_OK;
}

void CBoss_S_Wind::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fSizeX += _float(TimeDelta) * 5.0f;
	m_fSizeY += _float(TimeDelta) * 5.0f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (3.0f < m_fSizeY)
		m_fAlpha -= _float(TimeDelta);

	if (5.0f < m_fSizeY)
		CGameObject::Set_Dead();
}

void CBoss_S_Wind::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CBoss_S_Wind::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);

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

void CBoss_S_Wind::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		//CObj_Manager::GetInstance()->Set_Interaction(true);
		//CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::HIT);
	} 
}

HRESULT CBoss_S_Wind::SetUp_Components()
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

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Texture_Wind"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Wind::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	_float3 f3Color = { 0.0f, 0.0f, 0.0f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &f3Color, sizeof _float3)))
		return E_FAIL;

	return S_OK;
}

CBoss_S_Wind * CBoss_S_Wind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_S_Wind*		pInstance = new CBoss_S_Wind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_S_Wind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_S_Wind::Clone(void * pArg)
{
	CBoss_S_Wind*		pInstance = new CBoss_S_Wind(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_S_Wind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_S_Wind::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
