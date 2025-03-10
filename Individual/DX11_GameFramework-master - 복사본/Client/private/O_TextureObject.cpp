#include "stdafx.h"
#include "..\public\O_TextureObject.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "PipeLine.h"
#include "Effect_Manager.h"
#include "Skill_Manager.h"

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

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = m_TextureObject.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL)
		m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 1.f));

	return S_OK;
}

void CO_TextureObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (LEVEL_MINIGAME == CObj_Manager::GetInstance()->Get_Current_Level())
	{
		if (false == m_bPortal)
		{
			m_fAlpha = 0.5f;
			m_pTransformCom->Set_Pos(0.7f);
			m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 1.f));
		}
		else
		{
			m_fAlpha = 1.0f;
			m_pTransformCom->Set_Pos(1.5f);
			m_pTransformCom->Set_Scaled(_float3(2.0f, 2.0f, 1.f));
		}
	}
	else
	{
		if (3 > CObj_Manager::GetInstance()->Get_Current_Player().iKey)
		{
			m_fAlpha = 0.5f;
			m_pTransformCom->Set_Pos(0.7f);
			m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 1.f));
		}
		else
		{
			m_fAlpha = 1.0f;
			m_pTransformCom->Set_Pos(1.5f);
			m_pTransformCom->Set_Scaled(_float3(2.0f, 2.0f, 1.f));
		}
	}

	// 회전한다.
	m_pTransformCom->Turn(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), TimeDelta);

	// 이펙트
	m_bEffect_TimeAcc += TimeDelta;
	if (1.0 < m_bEffect_TimeAcc)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4Pos, vPos);
		CEffect_Manager::GetInstance()->Effect_Potal_StarColor_Create(_float3(f4Pos.x, f4Pos.y, f4Pos.z - 0.1f), _float3(0.019f, 0.91f, 1.0f));

		m_bEffect_TimeAcc = 0;
	}
}

void CO_TextureObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 충돌 처리
	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CO_TextureObject::Render()
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

void CO_TextureObject::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		if (LEVEL_MINIGAME == CObj_Manager::GetInstance()->Get_Current_Level())
			m_bPortal = true;

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Down(DIK_RETURN))
		{
			pGameInstance->Play_Sound(TEXT("sfx_portal_activate2.ogg"), 1.0f);

			if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
			{
				pGameInstance->Clear();
				pGameInstance->Stop_Sound(0);
				CObj_Manager::GetInstance()->Set_Key(-3);
				CGameObject::Set_Dead();
				CObj_Manager::GetInstance()->Set_NextLevel(true);
				CObj_Manager::GetInstance()->Set_Loading_Count();	// 로딩 화면을 위해서
			}
			else if (LEVEL_MINIGAME == CObj_Manager::GetInstance()->Get_Current_Level())
			{
				pGameInstance->Stop_Sound(0);
				CGameObject::Set_Dead();
				CObj_Manager::GetInstance()->Set_Loading_Count();
				CObj_Manager::GetInstance()->Set_NextLevel(true);
				CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);
				CObj_Manager::GetInstance()->Set_Camera(CObj_Manager::PLAYERINFO::PLAYER::JAKE);
				CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
			}
			else if (LEVEL_SKELETON == CObj_Manager::GetInstance()->Get_Current_Level())
			{
				pGameInstance->Clear();
				pGameInstance->Stop_Sound(0);
				CObj_Manager::GetInstance()->Set_Key(-3);
				CGameObject::Set_Dead();
				CObj_Manager::GetInstance()->Set_NextLevel(true);
				CObj_Manager::GetInstance()->Set_Loading_Count();
			}
		}

		RELEASE_INSTANCE(CGameInstance);
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

	if (m_TextureObject.eTextureType == TEXTUREOBJECT::TEXTURETYPE::PORTAL)
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
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
