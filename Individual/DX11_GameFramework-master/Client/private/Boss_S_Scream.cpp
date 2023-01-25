#include "stdafx.h"
#include "..\public\Boss_S_Scream.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Effect_Manager.h"

CBoss_S_Scream::CBoss_S_Scream(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CBoss_S_Scream::CBoss_S_Scream(const CBoss_S_Scream & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBoss_S_Scream::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Scream::Initialize(void * pArg)
{	
	m_wsTag = L"Gary_Boss";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(90.f));

	m_fAlpha = 1.0f;
	m_fSizeX = 1.0f;
	m_fSizeY = 1.0f;

	return S_OK;
}

void CBoss_S_Scream::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fSizeX += _float(TimeDelta) * 20.f;
	m_fSizeY += _float(TimeDelta) * 20.f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	m_fAlpha -= _float(TimeDelta) * 1.1f;

	if (20.f < m_fSizeX)
		CGameObject::Set_Dead();
}

void CBoss_S_Scream::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CBoss_S_Scream::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);	// pass

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

void CBoss_S_Scream::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		CObj_Manager::GetInstance()->Set_Interaction(true);
		CObj_Manager::GetInstance()->Set_Player_MinusHP(10.0f);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STUN);
	} 
}

HRESULT CBoss_S_Scream::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Texture_Scream"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.0f, 1.0f, 1.0f);
	ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_S_Scream::SetUp_ShaderResources()
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

	return S_OK;
}

CBoss_S_Scream * CBoss_S_Scream::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_S_Scream*		pInstance = new CBoss_S_Scream(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_S_Scream");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_S_Scream::Clone(void * pArg)
{
	CBoss_S_Scream*		pInstance = new CBoss_S_Scream(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_S_Scream");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_S_Scream::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
