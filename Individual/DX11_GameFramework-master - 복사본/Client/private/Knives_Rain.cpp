#include "stdafx.h"
#include "..\public\Knives_Rain.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CKnives_Rain::CKnives_Rain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CKnives_Rain::CKnives_Rain(const CKnives_Rain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CKnives_Rain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKnives_Rain::Initialize(void * pArg)
{	
	m_wsTag = L"Knives_Rain";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	m_dRain_TimeAcc = _double(CUtilities_Manager::GetInstance()->Get_Random(0.0f, 2.0f));

	return S_OK;
}

void CKnives_Rain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (true == m_bRain)
	{
		Rain_Tick(TimeDelta);
	}
}

void CKnives_Rain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CKnives_Rain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderCom, 0, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderCom, 0);

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CKnives_Rain::On_Collision(CGameObject * pOther)
{

}

HRESULT CKnives_Rain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Knives_Rain"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CKnives_Rain::SetUp_ShaderResources()
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

void CKnives_Rain::Rain_Tick(const _double & TimeDelta)
{
	m_dTimeAcc += TimeDelta;
	if (m_dRain_TimeAcc > m_dTimeAcc)
		return;

	if (false == m_bSound)
	{
		m_bSound = true;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_Sound(TEXT("sfx_enchiridion_knife_rain.ogg"), 0.4f);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_pTransformCom->Go_Down(TimeDelta);

	_float4 f4Position = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMStoreFloat4(&f4Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (-1.0f > f4Position.y)
	{
		++m_iRain_Count;
		m_pTransformCom->Set_Pos(4.0f);
	}

	if (7 == m_iRain_Count)	// 칼이 떨어지는 개수는 여기서 정해주면 된다.
	{
		m_bSound = false;
		m_bRain = false;
		m_iRain_Count = 0;
		m_dTimeAcc = 0.0;
		m_dRain_TimeAcc = _double(CUtilities_Manager::GetInstance()->Get_Random(0.0f, 2.0f));
	}
}

CKnives_Rain * CKnives_Rain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKnives_Rain*		pInstance = new CKnives_Rain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CKnives_Rain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKnives_Rain::Clone(void * pArg)
{
	CKnives_Rain*		pInstance = new CKnives_Rain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CKnives_Rain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKnives_Rain::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
