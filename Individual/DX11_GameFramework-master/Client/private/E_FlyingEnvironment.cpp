#include "stdafx.h"
#include "..\public\E_FlyingEnvironment.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CE_FlyingEnvironment::CE_FlyingEnvironment(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_FlyingEnvironment::CE_FlyingEnvironment(const CE_FlyingEnvironment & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_FlyingEnvironment::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_FlyingEnvironment::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tEffectInfo, pArg, sizeof(m_tEffectInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tEffectInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 1.f));

	if (EFFECTINFO::TYPE::BUTTERFLIES_BLUE == m_tEffectInfo.eType ||
		EFFECTINFO::TYPE::BUTTERFLIES_RED == m_tEffectInfo.eType ||
		EFFECTINFO::TYPE::BUTTERFLIES_YELLOW == m_tEffectInfo.eType)
	{
		Ready_Butterflies();
	}

	return S_OK;
}

void CE_FlyingEnvironment::Tick(_double TimeDelta)
{
	// 플레이어가 가까이 있을 때만 실행된다.
	if (false == m_bFindPlayer)
	{
		_float dd = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		if (3.0f < dd)
			return;
		else
			m_bFindPlayer = true;
	}

	// 이미지를 원하는 만큼 돌리고, 랜덤한 방향으로 날아가며, 알파값이 줄어들면서 사라진다.
	__super::Tick(TimeDelta);

	m_pTransformCom->Go_Straight(TimeDelta);

	m_dChange_TimeAcc += TimeDelta;
	if (0.07 < m_dChange_TimeAcc)
	{
		++m_iTexture_Index;
		m_dChange_TimeAcc = 0;
	}
	if (4 <= m_iTexture_Index)
	{
		m_iTexture_Index = 0;
		++m_iTextureDead_Count;
	}

	if (10 < m_iTextureDead_Count)
		m_fAlpha -= _float(TimeDelta) * 0.5f;

	if(0.0f > m_fAlpha)
		CGameObject::Set_Dead();
}

void CE_FlyingEnvironment::Late_Tick(_double TimeDelta)
{
	if (false == m_bFindPlayer)
		return;

	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_FlyingEnvironment::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_FlyingEnvironment::SetUp_Components()
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

	if (CE_FlyingEnvironment::EFFECTINFO::TYPE::BUTTERFLIES_BLUE == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Bule"));
	else if (CE_FlyingEnvironment::EFFECTINFO::TYPE::BUTTERFLIES_RED == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Red"));
	else if (CE_FlyingEnvironment::EFFECTINFO::TYPE::BUTTERFLIES_YELLOW == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Yellow"));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_FlyingEnvironment::SetUp_ShaderResources()
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

	// 텍스처 넘기기
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexture_Index)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

void CE_FlyingEnvironment::Ready_Butterflies()
{
	//_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 90.0f);
	//_matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	//_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	//vLook = XMVector3TransformCoord(vLook, RotationMatrix);

	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_Pos(CUtilities_Manager::GetInstance()->Get_Random(1.0f, 2.0f));
}

CE_FlyingEnvironment * CE_FlyingEnvironment::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_FlyingEnvironment*		pInstance = new CE_FlyingEnvironment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_FlyingEnvironment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_FlyingEnvironment::Clone(void * pArg)
{
	CE_FlyingEnvironment*		pInstance = new CE_FlyingEnvironment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_FlyingEnvironment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_FlyingEnvironment::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
