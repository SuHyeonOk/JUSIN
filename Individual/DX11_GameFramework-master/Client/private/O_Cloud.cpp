#include "stdafx.h"
#include "..\public\O_Cloud.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CO_Cloud::CO_Cloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CO_Cloud::CO_Cloud(const CO_Cloud & rhs)
	: CGameObject(rhs)
{

}

HRESULT CO_Cloud::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CO_Cloud::Initialize(void * pArg)
{	
	_float3	f3Position = { 0.0f, 0.0f, 0.0f };

	if (nullptr != pArg)
		memcpy(&f3Position, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.1f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Position;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	Ready_CloudType();

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	static _int iSizeCount;
	if (5 > iSizeCount)
	{
		m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 0.3f));
		++iSizeCount;
	}
	else
	{
		m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 0.5f));
	}

	return S_OK;
}

void CO_Cloud::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Move_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_V))
	{
		Set_Dead();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CO_Cloud::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CO_Cloud::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderCom, 0, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderCom, 0);

	return S_OK;
}

void CO_Cloud::On_Collision(CGameObject * pOther)
{

}

HRESULT CO_Cloud::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	_tchar	szModelName[MAX_PATH] = L"";

	if (ONE == eCloudType)
		wsprintf(szModelName, TEXT("Prototype_Component_Model_Cloud_0"));
	else if (TWO == eCloudType)
		wsprintf(szModelName, TEXT("Prototype_Component_Model_Cloud_1"));
	else if (THREE == eCloudType)
		wsprintf(szModelName, TEXT("Prototype_Component_Model_Cloud_2"));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, szModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CO_Cloud::SetUp_ShaderResources()
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

void CO_Cloud::Ready_CloudType()
{
	// 구름 모양이 3가지 있다 순서대로 출력된다.

	static _int iCloudType;

	switch (iCloudType)
	{
	case 0:
	{
		eCloudType = ONE;
		++iCloudType;
	}
	break;
	case 1:
	{
		eCloudType = TWO;
		++iCloudType;
	}
	break;
	case 2:
	{
		eCloudType = THREE;
		iCloudType = 0;
	}
	break;
	}
}

void CO_Cloud::Move_Tick(const _double & TimeDelta)
{
	m_dMove_TimeAcc += TimeDelta;
	if (10.0 < m_dMove_TimeAcc)
	{
		m_bOpposition = !m_bOpposition;
		m_dMove_TimeAcc = 0.0;
	}

	if (false == m_bOpposition)
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}
	else
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}
}

CO_Cloud * CO_Cloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CO_Cloud*		pInstance = new CO_Cloud(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CO_Cloud");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CO_Cloud::Clone(void * pArg)
{
	CO_Cloud*		pInstance = new CO_Cloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CO_Cloud");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CO_Cloud::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
