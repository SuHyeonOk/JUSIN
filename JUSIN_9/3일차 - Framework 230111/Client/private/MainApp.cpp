#include "stdafx.h"
#include "..\public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);

	///* RasterizerState : D3D11_RASTERIZER_DESC*/
	///*m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);*/
	///*m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);*/	
	//m_pContext->RSSetState();

	///* DepthStencilState */
	///*m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);*/
	///*m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);*/
	//m_pContext->OMSetDepthStencilState();

	///* BlendState */
	///*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);*/
	///*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);*/
	/*m_pContext->OMSetBlendState();*/


}
HRESULT CMainApp::Initialize()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 게임엔진 초기화 */
	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHIC_DESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWindowMode = GRAPHIC_DESC::WINMODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

#ifdef _DEBUG
	m_TimeAcc += TimeDelta;
#endif 

	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance || 
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pGameInstance->Clear_Graphic_Device(&_float4(0.0f, 0.f, 1.f, 1.f));	

	m_pRenderer->Draw_RenderGroup();

	m_pGameInstance->Render_Level();

#ifdef _DEBUG

	++m_iNumCallDraw;

	if (m_TimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("에프피에스 : %d"), m_iNumCallDraw);

	
		m_iNumCallDraw = 0;
		m_TimeAcc = 0.f;
	}

	m_pGameInstance->Render_Font(TEXT("Font_Comic"), m_szFPS, _float2(100.f, 0.f), 0.f, _float2(1.f, 1.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));


#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID || 
		nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;



	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */	
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;


	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	ID3D11Texture2D*			pTexture2D = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
		return E_FAIL;

	_ulong*			pPixel = new _ulong[256 * 256];

	for (_uint i = 0; i < 256; ++i)
	{
		for (_uint j = 0; j < 256; ++j)
		{
			_uint		iIndex = i * 256 + j;

			if(j < 128)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);	

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pPixel, sizeof(_ulong) * 256 * 256);

	m_pContext->Unmap(pTexture2D, 0);


	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixel);

	Safe_Release(pTexture2D);


	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];

	vPoints[0] = _float3(0.0f, 0.f, 5.0f);
	vPoints[1] = _float3(5.0f, 0.f, 0.0f);
	vPoints[2] = _float3(0.0f, 0.f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.0f, 0.f, 5.0f);
	vPoints[1] = _float3(5.0f, 0.f, 5.0f);
	vPoints[2] = _float3(5.0f, 0.f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.0f, 0.f, 10.0f);
	vPoints[1] = _float3(5.0f, 0.f, 5.0f);
	vPoints[2] = _float3(0.0f, 0.f, 5.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(5.0f, 0.f, 5.0f);
	vPoints[1] = _float3(10.0f, 0.f, 0.0f);
	vPoints[2] = _float3(5.0f, 0.f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);
	
	// MakeSpriteFont "폰트이름" /FontSize:32 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 출력파일이름.spritefont
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Comic"), TEXT("../Bin/Resources/fonts/131.SpriteFont"))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}
