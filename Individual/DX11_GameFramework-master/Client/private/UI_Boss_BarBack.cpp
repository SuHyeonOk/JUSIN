#include "stdafx.h"
#include "..\public\UI_Boss_BarBack.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "M_Gary_Boss.h"

CUI_Boss_BarBack::CUI_Boss_BarBack(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_Boss_BarBack::CUI_Boss_BarBack(const CUI_Boss_BarBack & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_Boss_BarBack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Boss_BarBack::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 417.0f / 1.5f;
	m_fSizeY = 22.0f / 1.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(10.0f, 261.0f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_Boss_BarBack::Tick(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CM_Gary_Boss * pGameObject = dynamic_cast<CM_Gary_Boss*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_Gary_Boss"), TEXT("Prototype_GameObject_M_Gary_Boss"), 0));
	if (nullptr == pGameObject)
	{
		MSG_BOX("보스 체력을 가져오지 못했습니다.");
		return;
	}
	RELEASE_INSTANCE(CGameInstance);

	_float fHPGauge = pGameObject->Get_BossHp() / pGameObject->Get_BossMaxHp();

	if (fHPGauge < m_fHPGauge)
		m_fHPGauge -= _float(TimeDelta) * 0.1f;
	else
		m_fHPGauge = fHPGauge;
}

void CUI_Boss_BarBack::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Boss_BarBack::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Boss_BarBack::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss_HealthBar_BarBack"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Boss_BarBack::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fHPGauge", &m_fHPGauge, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CUI_Boss_BarBack * CUI_Boss_BarBack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_Boss_BarBack*		pInstance = new CUI_Boss_BarBack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Boss_BarBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Boss_BarBack::Clone(void * pArg)
{
	CUI_Boss_BarBack*		pInstance = new CUI_Boss_BarBack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Boss_BarBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Boss_BarBack::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
