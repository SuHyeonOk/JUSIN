#include "stdafx.h"
#include "..\public\N_Bubblegum.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "UI_3DTexture.h"
#include "UI_Talk.h"

CN_Bubblegum::CN_Bubblegum(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CN_NPC(pDevice, pContext)
{

}

CN_Bubblegum::CN_Bubblegum(const CN_Bubblegum & rhs)
	: CN_NPC(rhs)
{

}

HRESULT CN_Bubblegum::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_Bubblegum::Initialize(void * pArg)
{
	m_wsTag = L"NPC_Bubblegum";

	CN_NPC::NPCDESC		NpcDesc;
	ZeroMemory(&NpcDesc, sizeof(NPCDESC));

	if (nullptr != pArg)
		memcpy(&NpcDesc, pArg, sizeof(NPCDESC));

	NpcDesc.eNpcType = NpcDesc.BUBBLEGUM;
	NpcDesc.TransformDesc.fSpeedPerSec = 0.f;
	NpcDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	NpcDesc.TransformDesc.f3Pos = _float3(NpcDesc.TransformDesc.f3Pos.x, NpcDesc.TransformDesc.f3Pos.y, NpcDesc.TransformDesc.f3Pos.z);

	if (FAILED(CN_NPC::Initialize(&NpcDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(270.f));
	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CN_Bubblegum::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Help_UI();
	Talk_UI();
	UI_Dead();
}

void CN_Bubblegum::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CN_Bubblegum::Render()
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

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CN_Bubblegum::On_Collision(CGameObject * pOther)
{
	__super::On_Collision(pOther);
}

HRESULT CN_Bubblegum::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_Princess_Bubblegum"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_Bubblegum::SetUp_ShaderResources()
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

void CN_Bubblegum::Help_UI()
{
	if (m_bHelp_UI)	
		return;

	m_bHelp_UI = true;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_TALK;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 2.f, f4MyPos.z - 0.5f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Help_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CN_Bubblegum::Talk_UI()
{
	if (!m_bIsTalk || m_bTalk_UI)
		return;

	m_bTalk_UI = true;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Talk_0"), TEXT("Prototype_GameObject_UI_Talk"))))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CN_Bubblegum::UI_Dead()
{
	if (!m_bTalk_UI)
		return S_OK;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 물음표 : 한 번 생성되고, 대화를 했다면 삭제된다.
	if (m_bHelp_UI && !m_bIsTalk)
	{
		CUI_3DTexture * pGameObject_UI_3DTexture = dynamic_cast<CUI_3DTexture*>(pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Help_0"), TEXT("Prototype_GameObject_UI_3DTexture"), 0));
		if (nullptr != pGameObject_UI_3DTexture)
			pGameObject_UI_3DTexture->Set_Dead();
	}


	CUI_Talk * pGameObject_UI_Talk = dynamic_cast<CUI_Talk*>(pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Talk_0"), TEXT("Prototype_GameObject_UI_Talk"), 0));
	if (nullptr != pGameObject_UI_Talk)
		pGameObject_UI_Talk->Set_Dead();
	
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CN_Bubblegum * CN_Bubblegum::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CN_Bubblegum*		pInstance = new CN_Bubblegum(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CN_Bubblegum");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CN_Bubblegum::Clone(void * pArg)
{
	CN_Bubblegum*		pInstance = new CN_Bubblegum(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CN_Bubblegum");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CN_Bubblegum::Free()
{
	__super::Free();


}