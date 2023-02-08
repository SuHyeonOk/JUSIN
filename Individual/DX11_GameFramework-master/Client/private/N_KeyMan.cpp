#include "stdafx.h"
#include "..\public\N_KeyMan.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

#include "UI_3DTexture.h"
#include "UI_Talk.h"

CN_KeyMan::CN_KeyMan(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CN_NPC(pDevice, pContext)
{

}

CN_KeyMan::CN_KeyMan(const CN_KeyMan & rhs)
	: CN_NPC(rhs)
{

}

HRESULT CN_KeyMan::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_KeyMan::Initialize(void * pArg)
{
	m_wsTag = L"NPC_KeyMan";

	CN_NPC::NPCDESC		NpcDesc;
	ZeroMemory(&NpcDesc, sizeof(NPCDESC));

	if (nullptr != pArg)
		memcpy(&NpcDesc, pArg, sizeof(NPCDESC));

	NpcDesc.eNpcType = NpcDesc.KEYMAN;
	NpcDesc.TransformDesc.fSpeedPerSec = 0.f;
	NpcDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	NpcDesc.TransformDesc.f3Pos = _float3(NpcDesc.TransformDesc.f3Pos.x, NpcDesc.TransformDesc.f3Pos.y, NpcDesc.TransformDesc.f3Pos.z);

	if (FAILED(CN_NPC::Initialize(&NpcDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(220.f));
	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CN_KeyMan::Tick(_double TimeDelta)
{
	CN_NPC::Tick(TimeDelta);

	Help_UI();
	UI_Dead();

	if (m_bInteraction)
	{
		m_pModelCom->Set_AnimIndex(1);

		switch (m_Script_Count)
		{
		case 0:
			CUI_Manager::GetInstance()->Set_Text(TEXT("열쇠 친구 : \n핀님! 제이크님! 큰일났어요.!!"));
			break;

		case 1:
			CUI_Manager::GetInstance()->Set_Text(TEXT("열쇠 친구 : \n제가 가지고 있던 열쇠를 어떤 마법사가 빼앗아 갔어요.!!"));
			break;

		case 2:
			CUI_Manager::GetInstance()->Set_Text(TEXT("열쇠 친구 : \n부탁드려요 꼭 열쇠를 찾아서 저희 왕국을 지켜주세요.!"));
			break;

		case 3:
			CUI_Manager::GetInstance()->Set_Text(TEXT("열쇠 친구 : \n마법 요정을 혼내주셔서 감사합니다. \n그래서 제가 좋은 정보 하나 드릴게요!"));
			break;

		case 4:
			CUI_Manager::GetInstance()->Set_Text(TEXT("열쇠 친구 : \n추즈 거위씨가 \n왕국을지키고있다는 증거로 열쇠 3개를 모아오면 \n특별한 물건을 판매하신다고 하셨어요!"));
			break;

		default:
			CUI_Manager::GetInstance()->Set_Text(TEXT("열쇠 친구 : \n핀님, 제이크님 정말 감사합니다!"));
			break;
		}
	}
	else
		m_pModelCom->Set_AnimIndex(0);
}

void CN_KeyMan::Late_Tick(_double TimeDelta)
{
	CN_NPC::Late_Tick(TimeDelta);
	m_pModelCom->Play_Animation(TimeDelta);
}

HRESULT CN_KeyMan::Render()
{
	if (FAILED(CN_NPC::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 0)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CN_KeyMan::On_Collision(CGameObject * pOther)
{
	CN_NPC::On_Collision(pOther);
}

HRESULT CN_KeyMan::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_KeyPerGarden"), TEXT("Com_Model"),
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

HRESULT CN_KeyMan::SetUp_ShaderResources()
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

void CN_KeyMan::Help_UI()
{
	if (m_bHelp_UI)	
		return;

	m_bHelp_UI = true;

	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_TALK;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 1.7f, f4MyPos.z - 0.5f);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Help_1"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CN_KeyMan::UI_Dead()
{
	if (!m_bHelp_UI)
		return S_OK;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 물음표 : 한 번 생성되고, 대화를 했다면 삭제된다.
	if (m_bInteraction)
	{
		CUI_3DTexture * pGameObject_UI_3DTexture = dynamic_cast<CUI_3DTexture*>(pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Help_1"), TEXT("Prototype_GameObject_UI_3DTexture"), 0));
		if (nullptr != pGameObject_UI_3DTexture)
			pGameObject_UI_3DTexture->Set_Dead();
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CN_KeyMan * CN_KeyMan::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CN_KeyMan*		pInstance = new CN_KeyMan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CN_KeyMan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CN_KeyMan::Clone(void * pArg)
{
	CN_KeyMan*		pInstance = new CN_KeyMan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CN_KeyMan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CN_KeyMan::Free()
{
	__super::Free();


}