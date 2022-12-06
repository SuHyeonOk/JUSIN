#include "stdafx.h"
#include "..\public\B_Star.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CB_Star::CB_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CB_Star::CB_Star(const CB_Star & rhs)
	: CGameObject(rhs)
{

}

HRESULT CB_Star::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_Star::Initialize(void * pArg)
{	
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_f4Pos = _float4(f3Pos.x, f3Pos.y, f3Pos.z, 1.f);

	return S_OK;
}

void CB_Star::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 플레이어의 몇 틱 전의 좌표를 받아와서 총알을 날리고, 
	// 일정시간 후에 총알이 사라지도록 해야한다.

	// 플레이어의 좌표가 아닌 총알을 발사하는 몬스터의 Look 으로 총알을 발사하는게 나을 것 같다.
	_vector vPlayerPos;
	vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
	//_float4	f4PlayerPos;
	//XMStoreFloat4(&f4PlayerPos, vPlayerPos);

	m_pTransformCom->LookAt(vPlayerPos);
	m_pTransformCom->Go_Straight(TimeDelta);



	//m_pTransformCom->Chase(XMVectorSet(f4PlayerPos.x, f4PlayerPos.y + 0.7f, f4PlayerPos.z, 1.f), TimeDelta);
}

void CB_Star::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CB_Star::Render()
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

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CB_Star::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Star"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_Star::SetUp_ShaderResources()
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

CB_Star * CB_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_Star*		pInstance = new CB_Star(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_Star::Clone(void * pArg)
{
	CB_Star*		pInstance = new CB_Star(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_Star::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
