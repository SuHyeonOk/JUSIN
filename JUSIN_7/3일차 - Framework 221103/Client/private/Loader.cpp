#include "stdafx.h"
#include "..\public\Loader.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// typedef unsigned (__stdcall* _beginthreadex_proc_type)(void*);
_uint APIENTRY LoadingThread(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlay();
		break;
	}
	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 로딩을 하기위한 추가적인 흐름을 만든다 (Thread).*/
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	for (_uint i = 0; i < 999999999; ++i)
		int a = 10;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));
	for (_uint i = 0; i < 999999999; ++i)
		int a = 10;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	for (_uint i = 0; i < 999999999; ++i)
		int a = 10;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));
	for (_uint i = 0; i < 999999999; ++i)
		int a = 10;

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;	

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);	
}
