#include "stdafx.h"
#include "..\public\Player_Talk.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CPlayer_Talk::CPlayer_Talk(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPlayer_Talk::CPlayer_Talk(const CPlayer_Talk & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayer_Talk::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Talk::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tPlayerTalkInfo, pArg, sizeof(m_tPlayerTalkInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(0.0f, 0.0f, 0.0f);
;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(1.5f, 2.0f, 1.f));

	// 알파 값
	m_fAlpha = 1.0f;

	m_iRandom_Texture = CUtilities_Manager::GetInstance()->Get_Random(0, 7);
	
	return S_OK;
}

void CPlayer_Talk::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_vector vFollwPostion = m_tPlayerTalkInfo.pTarget_TransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4FollwPostion = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4FollwPostion, vFollwPostion);
	if (CPlayer_Talk::PLAYERTALKINFO::TYPE::FINN == m_tPlayerTalkInfo.eTextureType)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4FollwPostion.x, f4FollwPostion.y + 1.5f, f4FollwPostion.z, f4FollwPostion.w));
	else if (CPlayer_Talk::PLAYERTALKINFO::TYPE::JAKE == m_tPlayerTalkInfo.eTextureType)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4FollwPostion.x, f4FollwPostion.y + 0.8f, f4FollwPostion.z, f4FollwPostion.w));

	// 일정 시간이 지나면 알파값이 빠진다.
	m_dDead_TimeAcc += TimeDelta;
	if (2 < m_dDead_TimeAcc)
	{
		// 알파값이 다 사라지면 죽음
		m_fAlpha -= _float(TimeDelta);

		if (0 >= m_fAlpha)
			CGameObject::Set_Dead();
	}
}

void CPlayer_Talk::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_Talk::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CPlayer_Talk::SetUp_Components()
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

	_tchar	m_szTextureName[MAX_PATH] = L"";

	if (CPlayer_Talk::PLAYERTALKINFO::TYPE::FINN == m_tPlayerTalkInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_PlayerTalk_Finn"));
	else if (CPlayer_Talk::PLAYERTALKINFO::TYPE::JAKE == m_tPlayerTalkInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_PlayerTalk_Jake"));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Talk::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iRandom_Texture)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CPlayer_Talk * CPlayer_Talk::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Talk*		pInstance = new CPlayer_Talk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Talk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Talk::Clone(void * pArg)
{
	CPlayer_Talk*		pInstance = new CPlayer_Talk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Talk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Talk::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
