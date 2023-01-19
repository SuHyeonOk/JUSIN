#include "stdafx.h"
#include "..\public\E_Alpha_Rotation.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "PipeLine.h"
#include "Utilities_Manager.h"

CE_Alpha_Rotation::CE_Alpha_Rotation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Alpha_Rotation::CE_Alpha_Rotation(const CE_Alpha_Rotation & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Alpha_Rotation::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Alpha_Rotation::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tEffectInfo, pArg, sizeof(m_tEffectInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tEffectInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (CE_Alpha_Rotation::EFFECTINFO::TEXTURETYPE::POTAL_STAR_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Alpha_Rotation::EFFECTINFO::TEXTURETYPE::POTAL_STARCOLOR_TEXTURE == m_tEffectInfo.eTextureType)
	{
		// ������ ũ�� �ֱ�
		_float fRandomSize = CUtilities_Manager::GetInstance()->Get_Random(0.1f, 0.3f);
		m_pTransformCom->Set_Scaled(_float3(fRandomSize, fRandomSize, 1.f));

		// ������ ���İ� �ֱ�
		_float fRandomAlpha = CUtilities_Manager::GetInstance()->Get_Random(0.5f, 1.0f);
		m_fAlpha = fRandomAlpha;
	}

	return S_OK;
}

void CE_Alpha_Rotation::Tick(_double TimeDelta)
{
	// ����� Ŀ���鼭, ���� ����� �ǰ� ���İ��� �پ��� �������.

	__super::Tick(TimeDelta);

	m_pTransformCom->Turn(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), TimeDelta);

	m_fAlpha -= _float(TimeDelta) * 0.5f;

	if (0 >= m_fAlpha)
		CGameObject::Set_Dead();	// ���İ��� �� ������� ����
}

void CE_Alpha_Rotation::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Alpha_Rotation::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// �� ����
	if (CE_Alpha_Rotation::EFFECTINFO::TEXTURETYPE::POTAL_STARCOLOR_TEXTURE == m_tEffectInfo.eTextureType)
		m_pShaderCom->Begin(4);
	// �̹�����
	else
		m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Alpha_Rotation::SetUp_Components()
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

	switch (m_tEffectInfo.eTextureType)
	{
	case CE_Alpha_Rotation::EFFECTINFO::SHIELD_TEXTURE:
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Jake_Shield"));
		break;

	case CE_Alpha_Rotation::EFFECTINFO::POTAL_STAR_TEXTURE:
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Potal_Star"));
		break;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (CE_Alpha_Rotation::EFFECTINFO::TEXTURETYPE::POTAL_STARCOLOR_TEXTURE == m_tEffectInfo.eTextureType)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &m_tEffectInfo.f3Color, sizeof _float3)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CE_Alpha_Rotation::SetUp_ShaderResources()
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
	
	//_float3 f3Color = _float3(CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f), CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f), CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f));

	//if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &f3Color, sizeof _float3)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Alpha_Rotation * CE_Alpha_Rotation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Alpha_Rotation*		pInstance = new CE_Alpha_Rotation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Alpha_Rotation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Alpha_Rotation::Clone(void * pArg)
{
	CE_Alpha_Rotation*		pInstance = new CE_Alpha_Rotation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Alpha_Rotation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Alpha_Rotation::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
