#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject ||
		eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugRenderGroup(CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	m_DebugObject.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_XRayBlend()))
		return E_FAIL;
	//if (FAILED(Render_ShadowDepth()))
	//	return E_FAIL;
	if (FAILED(Render_Map_NonAlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	/* 셰이드 타겟을 바인딩 하고,
	셰이드 타겟에다가 명암을 그릴수 있도록 처리를 한다. */
	/* 명암을 그리기위해서는 빛의 정보와 노멀의 정보가 필요하다. */
	/*노멀벡터의 경우 노멀 렌더타겟으로부터 얻어온다.
	빛의 정보의 경우 라이트객체로부터 얻어온다. */
	/* 셰이드타겟을 가득 채우고 그려줄 수있는 정점버퍼를 그린다. 이용하는 셰이더에게 노멀타겟과 빛 정보를 전역변수로
	던져서 연산할 수 있도록 하겠다. */
	if (FAILED(Render_LightAcc()))	// 조명 함수 호출
		return E_FAIL;

	/* 디퓨즈타겟(색상) * 셰이드타겟(명암)을 곱하여 최종적으로 백버퍼에 그려내는 작업을 수행한다. */
	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;

#ifdef _DEBUG

	if (FAILED(Render_DebugObject()))
		return E_FAIL;

	if (nullptr != m_pTarget_Manager)
	{
		m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_XRay"));
	}
#endif

	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* 렌터타겟들을 생성한다. */

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */	// 원하면 월드 pos를 넘겨줘도 되는데 Depth 을 넘기는 것이 보다 활용도가 좋다. 그 때는 DXGI_FORMAT_R32G32B32A32_FLOAT 을 사용하면 된다.
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// For.Target_XRay
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth_XRay"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	_uint		iShadowMapCX = 8000;	// TODO
	_uint		iShadowMapCY = 6000;

	// For.Target_ShadowDepth // 광원이 바라보는 플레이어의 깊이를 저장한다. (광원이 바라본 상태 에서의 깊이)
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"), _int(ViewportDesc.Width), _int(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.MRT_Deferred */ /* 디퍼드 렌더링(빛)을 수행하기위해 필요한 데이터들을 저장한 렌더타겟들. */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* For.MRT_LightAcc */ /* 빛 연산의 결과를 저장할 렌더타겟들.  */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))	// 조명 후에 값을 더 해주는 것 으로 MRT_LightAcc 에 추가한다.
		return E_FAIL;

	// For.MRT_ShadowDepth(Shadow)
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;
	/* For.MRT_XRay */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_XRay"), TEXT("Target_Depth_XRay"))))	
		return E_FAIL;


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));



#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 80.0f, 80.0f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 80.0f, 230.0f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 80.0f, 380.0f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth_XRay"), 80.0f, 530.0f, 150.f, 150.f)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 230.0f, 80.0f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 230.0f, 230.0f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), 230.0f, 380.0f, 150.f, 150.f)))
		return E_FAIL;
#endif

	/*LPDIRECT3DDEVICE9		pDevice = nullptr;

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0 ~255);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);*/

	/*pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	_float4			vSourColor, vDestColor;

	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
	*/

	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_ShadowDepth();
		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOWDEPTH].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_XRayBlend()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_XRay"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_XRAYBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_XRay();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_XRAYBLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_XRay"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Map_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Diffuse + Normal */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_MAP_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_MAP_NONALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	//m_pContext->CopyResource(m_pTarget_Manager->Get_Texture2D(TEXT("Target_Depth_Copy")), m_pTarget_Manager->Get_Texture2D(TEXT("Target_Depth")));

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	/* 직교행렬  */
	/*transpose()
	XMMatrixInverse();
	float3x3*/
	/*XMMatrixTranspose();*/

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	/* 빛 갯수만큼 사각형 버퍼(셰이드타겟의 전체 픽셀을 갱신할 수 있는 사이즈로 그려지는 정점버퍼. )를 그린다. */
	m_pLight_Manager->Render_Light(m_pVIBuffer, m_pShader);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTextureXRay", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth_XRay")))))
		return E_FAIL;
	//if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadowDepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ShadowDepth")))))
	//	return E_FAIL;

	////_vector vLightEye = XMVectorSet(-5.0f, 15.0f, -5.0f, 0.0f);
	////_vector vLightAt = XMVectorSet(60.0f, 0.0f, 60.0f, 0.0f);
	////_vector vLightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//_vector vLightEye = XMLoadFloat4(&m_LightDesc.f4LightEye);
	//_vector vLightAt = XMLoadFloat4(&m_LightDesc.f4LightAt);
	//_vector vLightUp = XMLoadFloat4(&m_LightDesc.f4LightUp);

	//_matrix	LightViewMatrix;
	//LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	//_float4x4	f4LightViewMatrix;
	//XMStoreFloat4x4(&f4LightViewMatrix, LightViewMatrix);

	//if (FAILED(m_pShader->Set_Matrix("g_LightViewMatrix", &f4LightViewMatrix)))
	//	return E_FAIL;

	//CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	//if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;

	//RELEASE_INSTANCE(CPipeLine);

	//if (FAILED(m_pShader->Set_RawValue("g_fTemp", &m_LightDesc.fTemp, sizeof _float)))
	//	return E_FAIL;

	//if (FAILED(m_pShader->Set_RawValue("g_fFar", &m_LightDesc.fFar, sizeof _float)))
	//	return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)-> _bool
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_DebugObject()
{
#ifdef _DEBUG
	for (auto& pComponent : m_DebugObject)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
}

	m_DebugObject.clear();
#endif

	return S_OK;

}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRenderer * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}
void CRenderer::Free()
{
	__super::Free();

	for (auto& pComponent : m_DebugObject)
		Safe_Release(pComponent);

	m_DebugObject.clear();

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderObjects[i])
			Safe_Release(pGameObject);

		m_RenderObjects[i].clear();
	}

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

}
