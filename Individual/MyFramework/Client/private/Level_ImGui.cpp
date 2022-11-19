#include "stdafx.h"
#include "..\public\Level_ImGui.h"

#include "GameInstance.h"

#if defined(USE_IMGUI)
#include "ImGui_Manager.h"
#endif

CLevel_ImGui::CLevel_ImGui(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_ImGui::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

#if defined (USE_IMGUI)
	// ImGui_Manager 초기화
	CImGui_Manager::GetInstance()->Ready_Imgui();
#endif

	return S_OK;
}

void CLevel_ImGui::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

#if defined (USE_IMGUI)
	// ImGui_Manager 업데이트
	CImGui_Manager::GetInstance()->Tick_Imgui();
#endif
}

void CLevel_ImGui::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_ImGui::Render()
{
#if defined(USE_IMGUI)
	// ImGui_Manager 렌더링
	CImGui_Manager::GetInstance()->Render_Imgui();
#endif

	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : IMGUI"));

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_ImGui * CLevel_ImGui::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_ImGui*		pInstance = new CLevel_ImGui(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_ImGui");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_ImGui::Free()
{
	__super::Free();

#if defined(USE_IMGUI)
	// ImGui_Manager Free
	CImGui_Manager::GetInstance()->DestroyInstance();
#endif
}
