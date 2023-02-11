#include "stdafx.h"
#include "..\public\Korean_Food.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CKorean_Food::CKorean_Food(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CKorean_Food::CKorean_Food(const CKorean_Food & rhs)
	: CGameObject(rhs)
{

}

HRESULT CKorean_Food::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKorean_Food::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_ObjectInfo, pArg, sizeof(OBJECTINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_ObjectInfo.f3Position;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (IDLE == m_ObjectInfo.eType)		// 플레이어가 삭제할 객체
	{
		m_wsTag = L"Korean_Food";
	}
	else								// 플레이어를 따라 다니다가 다람쥐에게 줄 객체
	{
		m_wsTag = L"Korean_Food_Squirrel";
	}

	return S_OK;
}

void CKorean_Food::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (PLAYER == m_ObjectInfo.eType)
	{
		Player_Follow(TimeDelta);
	}
}

void CKorean_Food::Late_Tick(_double TimeDelta)
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

HRESULT CKorean_Food::Render()
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

void CKorean_Food::On_Collision(CGameObject * pOther)
{
	if (IDLE == m_ObjectInfo.eType)				// 플레이어와 충돌하고 나면 삭제된다. 그리고 플레이어 머리로 가서 플레이어를 따라다닌다.
	{
		if (L"Jake" == pOther->Get_Tag())
		{
			CGameObject::Set_Dead();
		}
	}
	else if(PLAYER == m_ObjectInfo.eType)		// 몬스터와 충돌하는 순간 삭제 되면서 몬스터 앞에 음식이 놓이게 된다.
	{
		if (L"Squirrel" == pOther->Get_Tag())
		{
			CGameObject::Set_Dead();
		}
	}
}

HRESULT CKorean_Food::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Korean_Food"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	
	if (IDLE == m_ObjectInfo.eType)
	{
		ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}
	else
	{
		ColliderDesc.vSize = _float3(3.0f, 3.0f, 3.0f);
		ColliderDesc.vCenter = _float3(0.f, -2.5f, 1.0f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CKorean_Food::SetUp_ShaderResources()
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

void CKorean_Food::Player_Follow(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pPlayer_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_MINIGAME, TEXT("Layer_FinnAndJake"), TEXT("Com_Transform"), 0));
	_vector vPlayerPosition = pPlayer_TransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	_float4	f4Player_Position = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4Player_Position, vPlayerPosition);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&_float4(f4Player_Position.x, 2.7f, f4Player_Position.z, 1.0f)));
}

CKorean_Food * CKorean_Food::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKorean_Food*		pInstance = new CKorean_Food(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CKorean_Food");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKorean_Food::Clone(void * pArg)
{
	CKorean_Food*		pInstance = new CKorean_Food(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CKorean_Food");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKorean_Food::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
