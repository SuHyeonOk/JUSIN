#include "stdafx.h"
#include "..\public\Level_Logo.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
		
}

void CLevel_Logo::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : LOGO"));

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();


}
