#include "stdafx.h"
#include "..\public\Level_Tool.h"

#include "GameInstance.h"

#include "Coin.h"
#include "Page.h"

CLevel_Tool::CLevel_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Tool::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
		
}

void CLevel_Tool::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//SetWindowText(g_hWnd, TEXT("Level : TOOL"));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_M_PigWarrior_BEE"), &_float3(-5.f, 0.f, 0.f))))
		return E_FAIL;

	CCoin::COININFO					tObjInfo;
	tObjInfo.fPos = _float3(2.f, 0.f, 0.f);
	tObjInfo.eState = tObjInfo.IDLE;
	tObjInfo.eCoinKind = tObjInfo.COIN_BRONZE;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Coin"), &tObjInfo)))
		return E_FAIL;

	CPage::PAGEINFO					tPageInfo;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.isEnable = true;
	///*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	//LightDesc.vPosition = _float4(5.f, 3.f, 5.f, 1.f);
	//LightDesc.fRange = 10.0f;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Tool * CLevel_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Tool*		pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();


}

