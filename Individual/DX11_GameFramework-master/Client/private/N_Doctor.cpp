#include "stdafx.h"
#include "..\public\N_Doctor.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "UI_3DTexture.h"
#include "UI_Talk.h"

CN_Doctor::CN_Doctor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CN_NPC(pDevice, pContext)
{

}

CN_Doctor::CN_Doctor(const CN_Doctor & rhs)
	: CN_NPC(rhs)
{

}

HRESULT CN_Doctor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_Doctor::Initialize(void * pArg)
{
	m_wsTag = L"NPC_Dector";

	CN_NPC::NPCDESC		NpcDesc;
	ZeroMemory(&NpcDesc, sizeof(NPCDESC));

	if (nullptr != pArg)
		memcpy(&NpcDesc, pArg, sizeof(NPCDESC));

	NpcDesc.eNpcType = NpcDesc.DOCTOR;
	NpcDesc.TransformDesc.fSpeedPerSec = 0.f;
	NpcDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	NpcDesc.TransformDesc.f3Pos = _float3(NpcDesc.TransformDesc.f3Pos.x, NpcDesc.TransformDesc.f3Pos.y, NpcDesc.TransformDesc.f3Pos.z);

	if (FAILED(CN_NPC::Initialize(&NpcDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(-45.f));
	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CN_Doctor::Tick(_double TimeDelta)
{
	CN_NPC::Tick(TimeDelta);

	Help_UI();
	Talk_UI();
	UI_Dead();

	if (m_bInteraction)
	{
		m_pModelCom->Set_AnimIndex(1);

		switch (m_Script_Count)
		{
		case 0:
			lstrcpy(m_szScript, TEXT("��! ������� ������ ����߾�"));
			break;

		case 1:
			lstrcpy(m_szScript, TEXT("�� �ʿ��� �ַ��� �غ��� Į�� �ִµ� \n�ذ� ���Ͱ� �ϼ��� ���⸦ ������ ���Ⱦ�"));
			break;

		case 2:
			lstrcpy(m_szScript, TEXT("�� �ڽ��� ã�Ƽ� �� Į�� ��ã��, \n�츮 ������ �̷��� ���� �༮�� ȥ����!!"));
			break;

		default:
			lstrcpy(m_szScript, TEXT(""));
			break;
		}
	}
	else
		m_pModelCom->Set_AnimIndex(0);
}

void CN_Doctor::Late_Tick(_double TimeDelta)
{
	CN_NPC::Late_Tick(TimeDelta);
}

HRESULT CN_Doctor::Render()
{
	if (FAILED(CN_NPC::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����Ѵ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	// �뺻
	if (m_bInteraction)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), m_szScript, _float2(500.f, 100.f), 0.f, _float2(0.3f, 0.3f));
		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

void CN_Doctor::On_Collision(CGameObject * pOther)
{
	CN_NPC::On_Collision(pOther);
}

HRESULT CN_Doctor::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_N_Doctor_Princess"), TEXT("Com_Model"),
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

HRESULT CN_Doctor::SetUp_ShaderResources()
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

void CN_Doctor::Help_UI()
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

void CN_Doctor::Talk_UI()
{
	if (m_bTalk_UI || !m_bInteraction)
		return;

	m_bTalk_UI = true;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Talk_0"), TEXT("Prototype_GameObject_UI_Talk"))))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CN_Doctor::UI_Dead()
{
	if (!m_bTalk_UI)
		return S_OK;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ����ǥ : �� �� �����ǰ�, ��ȭ�� �ߴٸ� �����ȴ�.
	if (m_bHelp_UI && m_bInteraction)
	{
		CUI_3DTexture * pGameObject_UI_3DTexture = dynamic_cast<CUI_3DTexture*>(pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Help_0"), TEXT("Prototype_GameObject_UI_3DTexture"), 0));
		if (nullptr != pGameObject_UI_3DTexture)
			pGameObject_UI_3DTexture->Set_Dead();
	}

	// ��ȭâ : �� �� �����ǰ�, �÷��̾ �Ÿ��� �־����ų�, B Ű�� ������ �����Ǿ�� �Ѵ�.
	if (m_bTalk_UI && !m_bInteraction)
	{
		CUI_Talk * pGameObject_UI_Talk = dynamic_cast<CUI_Talk*>(pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Talk_0"), TEXT("Prototype_GameObject_UI_Talk"), 0));
		if (nullptr != pGameObject_UI_Talk)
			pGameObject_UI_Talk->Set_Dead();
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CN_Doctor * CN_Doctor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CN_Doctor*		pInstance = new CN_Doctor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CN_Doctor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CN_Doctor::Clone(void * pArg)
{
	CN_Doctor*		pInstance = new CN_Doctor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CN_Doctor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CN_Doctor::Free()
{
	__super::Free();


}