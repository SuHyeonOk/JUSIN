#include "stdafx.h"
#include "..\Header\Loading.h"

#include "Export_Function.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	,m_bFinish(false)
	,m_eID(LOADING_END)
{
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	m_eID = eID;
	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	// �׸��� ������Ʈ
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColCom", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcColCom", CRcCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexCom", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTexCom", CCubeTex::Create(m_pGraphicDev)), E_FAIL);

	// �ؽ�ó
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Ma.jpg", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4)), E_FAIL);
		
	// ������Ʈ
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformCom", CTransform::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorCom", CCalculator::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AnimatorCom", CAnimator::Create(m_pGraphicDev)), E_FAIL);

	// �÷��̾� ���ݹ��� - wand1 �ؽ���
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WandTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Wand/wand%d.png", TEX_NORMAL,3)), E_FAIL);
		
	// GreenSlime_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Idle/GreenSlime%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Attack/GreenSlime%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Hit/GreenSlime%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Die/GreenSlime%d.png", TEX_NORMAL, 3)), E_FAIL);

	// PinkSlime_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Idle/PinkSlime%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Attack/PinkSlime%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Hit/PinkSlime%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Die/PinkSlime%d.png", TEX_NORMAL, 12)), E_FAIL);

	// UI_HP
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_HealthBar_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/HealthBar/HealthBar%d.png", TEX_NORMAL, 1)), E_FAIL);



	m_bFinish = true;

	lstrcpy(m_szLoading, L"Loading Complete!!!!!!!!!!!");

	return 0;
}

_uint CLoading::Loading_ForBoss(void)
{
	return 0;
}

unsigned int CLoading::Thread_Main(void * pArg)
{
	CLoading*	pLoading = (CLoading*)pArg;
	_uint iFlag = 0;

	// ��� switch������ �и��ϰ� �б��ϰ� �ֱ� ������ �Ӱ迵������ ���� ������ �ʿ�� ����.
	EnterCriticalSection(&(pLoading->Get_Crt()));

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		// iFlag = pLoading->Loading_ForBoss();
		break;
	}

	LeaveCriticalSection(&(pLoading->Get_Crt()));

	return iFlag;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading*		pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		MSG_BOX("Loading Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}
