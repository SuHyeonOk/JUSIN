#include "stdafx.h"
#include "..\public\O_TextureObject.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "PipeLine.h"

CO_TextureObject::CO_TextureObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CO_TextureObject::CO_TextureObject(const CO_TextureObject & rhs)
	: CGameObject(rhs)
{

}

HRESULT CO_TextureObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CO_TextureObject::Initialize(void * pArg)
{	
	m_wsTag = L"Texture_Object";

	if (nullptr != pArg)
		memcpy(&m_TextureObject, pArg, sizeof(TEXTUREOBJECT));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_TextureObject.f3Pos.x, m_TextureObject.f3Pos.y + 1.4f, m_TextureObject.f3Pos.z);
	}
	else if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::MOVE_PORTAL)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_TextureObject.f3Pos.x, m_TextureObject.f3Pos.y, m_TextureObject.f3Pos.z);
	}

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL)
		m_pTransformCom->Set_Scaled(_float3(2.f, 2.f, 1.f));
	else if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::MOVE_PORTAL)
		m_pTransformCom->Set_Scaled(_float3(1.5f, 1.5f, 1.f));

	return S_OK;
}

void CO_TextureObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//// 카메라를 바라본다.
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	//_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//RELEASE_INSTANCE(CGameInstance);

	//m_pTransformCom->LookAt(vCameraPos, true);	

	//// 회전한다.
	m_pTransformCom->Turn(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), TimeDelta);

	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::MOVE_PORTAL)	// 일정 시간 뒤 삭제
		MovePortal(TimeDelta);

	// 충돌 처리
	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

void CO_TextureObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CO_TextureObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);	// pass

	m_pVIBufferCom->Render();

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

void CO_TextureObject::On_Collision(CGameObject * pOther)
{
	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL)
	{
		if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			if (pGameInstance->Key_Down(DIK_RETURN))
			{
				if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
				{
					CGameObject::Set_Dead();
					CObj_Manager::GetInstance()->Set_NextLevel(true);
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

HRESULT CO_TextureObject::SetUp_Components()
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

	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL ||
		m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::MOVE_PORTAL)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_O_TextureObject"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(3.0f, 3.0f, 3.0f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CO_TextureObject::SetUp_ShaderResources()
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

	return S_OK;
}

void CO_TextureObject::MovePortal(const _double & TimeDelta)
{
	m_bMovePortal += TimeDelta;
	if (1 < m_bMovePortal)
	{
		CGameObject::Set_Dead();
		m_bMovePortal = 0;
	}
}

CO_TextureObject * CO_TextureObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CO_TextureObject*		pInstance = new CO_TextureObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CO_TextureObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CO_TextureObject::Clone(void * pArg)
{
	CO_TextureObject*		pInstance = new CO_TextureObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CO_TextureObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CO_TextureObject::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
