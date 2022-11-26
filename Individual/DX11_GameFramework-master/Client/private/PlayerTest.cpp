	#include "stdafx.h"
#include "..\public\PlayerTest.h"

#include "GameInstance.h"

CPlayerTest::CPlayerTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPlayerTest::CPlayerTest(const CPlayerTest & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayerTest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayerTest::Initialize(void * pArg)
{
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;		

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CPlayerTest::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//Key_Input(TimeDelta);
	

}

void CPlayerTest::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CPlayerTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CPlayerTest::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerTest::SetUp_ShaderResources()
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

	return S_OK;
}

void CPlayerTest::Key_Input(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), 0.f);
		m_pTransformCom->Go_Straight(TimeDelta);

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * 25.f);
			//m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), 0.5f);
			m_pTransformCom->Go_Straight(TimeDelta / 2);
		}
		if (pGameInstance->Key_Pressing(DIK_LEFT))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * -25.f);
			//m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), -0.5f);
			m_pTransformCom->Go_Straight(TimeDelta / 2);
		}
	}
	//if (pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), -1.f);
	//	m_pTransformCom->Go_Backward(TimeDelta);
	//}

	//if (pGameInstance->Key_Pressing(DIK_UP) && pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), 45.f);
	//	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * 1.f);
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), -80.f);
		//m_pTransformCom->Go_Straight(TimeDelta);
		m_pTransformCom->Go_Right(TimeDelta);
	}
	//if (pGameInstance->Key_Pressing(DIK_RIGHT) && pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	//m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), 135.f);
	//	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -90.f * 0.05f);
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), 180.f);
		m_pTransformCom->Go_Backward(TimeDelta);
	}
	//if (pGameInstance->Key_Pressing(DIK_DOWN) && pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	//m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), 225.f);
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), 80.f);
		//m_pTransformCom->Go_Straight(TimeDelta);
		m_pTransformCom->Go_Left(TimeDelta);
	}
	//if (pGameInstance->Key_Pressing(DIK_LEFT) && pGameInstance->Key_Pressing(DIK_UP))
	//{
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), -45.f);
	//	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -1.f);
	//	m_pTransformCom->Go_Straight(TimeDelta);
	//}

	//_long			MouseMove = 0;
	//_float			fSensitivity = 0.05f;

	//if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
	//{
	//	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * fSensitivity);
	//}

//#ifdef _DEBUG
//	_vector a = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
//
//	a.m128_f32[0];
//	//cout << MouseMove << " / " << a.m128_f32[0] << endl;
//
//	if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
//		cout << "////////////////////////" << endl;
//
//	//cout << a.x << " / " << a.y << " / " << a.z << endl;
//#endif

	RELEASE_INSTANCE(CGameInstance);
}

CPlayerTest * CPlayerTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerTest*		pInstance = new CPlayerTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerTest::Clone(void * pArg)
{
	CPlayerTest*		pInstance = new CPlayerTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerTest::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
