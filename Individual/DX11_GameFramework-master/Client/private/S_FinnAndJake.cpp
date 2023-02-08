#include "stdafx.h"
#include "..\public\S_FinnAndJake.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Skill_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

CS_FinnAndJake::CS_FinnAndJake(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_FinnAndJake::CS_FinnAndJake(const CS_FinnAndJake & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_FinnAndJake::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_FinnAndJake::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_wsTag = L"Finn";
	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(7);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Stop_Sound(0);
	pGameInstance->Play_Sound(TEXT("Scroll_fiona.mp3"), 0.5f, false, 3);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CS_FinnAndJake::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	KeyInput(TimeDelta);

	// [0] ¿Ã¶ó°¡±â
	// [1] ¼Õ Á¡ÇÁ
	// [2] ÇÉ ¼ÕÈçµé±â
	// [3] °¡¸¸È÷
	// [4] Á¡ÇÁ
	// [5] ´Þ¸®±â
	// [6] ³»·Á°¡±â
	// [7] ¶Ë¶¥¶Ë¶¥ °È±â
}

void CS_FinnAndJake::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);		// Ãæµ¹Ã³¸®
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CS_FinnAndJake::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (3 == i)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			if (m_bShader_Hit)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
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

void CS_FinnAndJake::On_Collision(CGameObject * pOther)
{

}

HRESULT CS_FinnAndJake::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_FinnAndJake"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 2.0f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_FinnAndJake::SetUp_ShaderResources()
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

void CS_FinnAndJake::KeyInput(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_OnMove)
	{
		pGameInstance->Stop_Sound(5);
		m_pTransformCom->Go_Straight(TimeDelta);
		m_pTransformCom->PlayerMove(XMVectorSet(m_f4NewLook.x, m_f4NewLook.y, m_f4NewLook.z, m_f4NewLook.w), TimeDelta);
	}
		
#pragma region ÀÌµ¿
	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_OnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, 1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_OnMove = true;
		m_f4NewLook = { 1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_OnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, -1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_OnMove = true;
		m_f4NewLook = { -1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}
#pragma endregion

	if (pGameInstance->Key_Up(DIK_UP) || pGameInstance->Key_Up(DIK_RIGHT) || pGameInstance->Key_Up(DIK_DOWN) || pGameInstance->Key_Up(DIK_LEFT))
	{
		pGameInstance->Stop_Sound(7);
		m_OnMove = false;
	}

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		pGameInstance->Play_Sound(TEXT("sfx_finn_sword_1.ogg"), 0.7f, false, 1);
	}

	RELEASE_INSTANCE(CGameInstance);
}

CS_FinnAndJake * CS_FinnAndJake::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_FinnAndJake*		pInstance = new CS_FinnAndJake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_FinnAndJake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_FinnAndJake::Clone(void * pArg)
{
	CS_FinnAndJake*		pInstance = new CS_FinnAndJake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_FinnAndJake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_FinnAndJake::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
