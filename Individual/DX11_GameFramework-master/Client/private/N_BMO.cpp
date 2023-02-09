#include "stdafx.h"
#include "..\public\N_BMO.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

#include "UI_3DTexture.h"
#include "UI_Talk.h"

CN_BMO::CN_BMO(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CN_NPC(pDevice, pContext)
{

}

CN_BMO::CN_BMO(const CN_BMO & rhs)
	: CN_NPC(rhs)
{

}

HRESULT CN_BMO::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_BMO::Initialize(void * pArg)
{
	m_wsTag = L"NPC_BMO";

	CN_NPC::NPCDESC		NpcDesc;
	ZeroMemory(&NpcDesc, sizeof(NPCDESC));

	if (nullptr != pArg)
		memcpy(&NpcDesc, pArg, sizeof(NPCDESC));

	NpcDesc.eNpcType = NpcDesc.BMO;
	NpcDesc.TransformDesc.fSpeedPerSec = 0.f;
	NpcDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	NpcDesc.TransformDesc.f3Pos = NpcDesc.TransformDesc.f3Pos;

	if (FAILED(CN_NPC::Initialize(&NpcDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(0.1f, 0.1f, 0.1f));

	return S_OK;
}

void CN_BMO::Tick(_double TimeDelta)
{
	CN_NPC::Tick(TimeDelta);

	Talk_Tick();
}

void CN_BMO::Late_Tick(_double TimeDelta)
{
	CN_NPC::Late_Tick(TimeDelta);
}

HRESULT CN_BMO::Render()
{
	if (FAILED(CN_NPC::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i);
	}
	return S_OK;
}

void CN_BMO::On_Collision(CGameObject * pOther)
{
	//CN_NPC::On_Collision(pOther);

	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		m_bInteraction = CObj_Manager::GetInstance()->Get_Interaction();

		// 플레이어와 충돌하고 있는 상태에서 'G' 를 누르면 대화가 넘어간다.
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Down(DIK_G))
			++m_Script_Count;

		RELEASE_INSTANCE(CGameInstance);

		// Talk 창 띄우기, 띄위지 않기
		if (m_bInteraction)
			CUI_Manager::GetInstance()->Set_Talk(true);
		else
			CUI_Manager::GetInstance()->Set_Talk(false);
	}
}

HRESULT CN_BMO::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_BMO"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(30.f, 30.f, 30.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CN_BMO::SetUp_ShaderResources()
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

void CN_BMO::Talk_Tick()
{
	if (m_bInteraction)
	{
		switch (m_Script_Count)
		{
		case 0:
			CUI_Manager::GetInstance()->Set_Text(TEXT("비모 : \n핀! 제이크! 너내 나 빼고 뭐하러 다니는거야!?"));
			break;

		case 1:
			CUI_Manager::GetInstance()->Set_Text(TEXT("비모 : \n얼른 나랑 같이 게임 한 판하고 가!!"));
			break;

		case 2:
			CUI_Manager::GetInstance()->Set_Text(TEXT("비모 : \n게임은 간단해! 시간안에 케이크 5개를 찾으면 돼!"));
			break;

		case 3:
			CUI_Manager::GetInstance()->Set_Text(TEXT("비모 : \n성공하면 내가 아~ 주~ 좋은걸 줄게 \n얘들아 화이팅!"));
			break;

		case 4:
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Stop_Sound(0);
			RELEASE_INSTANCE(CGameInstance);
			CObj_Manager::GetInstance()->Set_NextLevel(true);
			CObj_Manager::GetInstance()->Set_Loading_Count();	// 로딩 화면을 위해서
			CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);
		}
			break;

		default:
			CUI_Manager::GetInstance()->Set_Text(TEXT("비모 : \n핀, 제이크 화이팅!"));
			break;
		}
	}
}

CN_BMO * CN_BMO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CN_BMO*		pInstance = new CN_BMO(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CN_BMO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CN_BMO::Clone(void * pArg)
{
	CN_BMO*		pInstance = new CN_BMO(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CN_BMO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CN_BMO::Free()
{
	__super::Free();


}