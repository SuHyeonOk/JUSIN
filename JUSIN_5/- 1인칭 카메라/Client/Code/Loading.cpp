#include "stdafx.h"
#include "..\Header\Loading.h"

#include "Export_Function.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
	, m_eID(LOADING_END)	
{
	m_pGraphicDev->AddRef();
}


CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	// 크리티컬 섹션 객체 만들기
	InitializeCriticalSection(&m_Crt);

	// 쓰레드 생성
	// 1인자) 보안 모드
	// 2인자) 스텍 사이즈
	// 3인자) 실제 쓰레드를 구동하는 함수 (호출은 반드시 stdcall)
	// 4인자) 현재 로딩 클래스에 있는 데이터를 3인자 함수를 통해 가공하겠다
	// 5인자) 플래그
	// 6인자) 쓰레드 주소 (지금은 하나만 사용하니 nullptr)
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	m_eID = eID;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColCom", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcColCom", CRcCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexCom", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Terrain0.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Ma.jpg", TEX_NORMAL)), E_FAIL);
	
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformCom", CTransform::Create()), E_FAIL);
	
	m_bFinish = true;

	return _uint();
}

_uint CLoading::Loading_ForBoss(void)
{
	return _uint();
}

unsigned int CLoading::Thread_Main(void * pArg)
{
	// pArgs는 void 이니 CLoading로 형변환
	CLoading*		pLoading = (CLoading*)pArg;

	_uint iFlag = 0;

	EnterCriticalSection(&(pLoading->Get_Crt())); // 진입

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		//iFlag = pLoading->Loading_ForBoss();
		break;
	}

	LeaveCriticalSection(&(pLoading->Get_Crt())); // 끝

	return iFlag;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading*		pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		MSG_BOX("Loading Create Failed"); // 로딩시 오류 메시지
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);	// 쓰레드가 종료될 때 까지 대기
	CloseHandle(m_hThread);						// 쓰레드 삭제
	DeleteCriticalSection(&m_Crt);				// CriticalSection 삭제

	Safe_Release(m_pGraphicDev);				// 컴 객체 해제
}
