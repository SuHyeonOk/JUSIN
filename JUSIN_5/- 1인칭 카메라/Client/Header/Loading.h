#pragma once
#include "Engine_Include.h"
#include "Base.h" 

class CLoading : public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	CRITICAL_SECTION		Get_Crt()		{ return m_Crt;	}
	_bool					Get_Finish()	{ return m_bFinish; }	// 로딩이 종료 되었는지
	LOADINGID				Get_LoadingID() { return m_eID; }		// ID에 맞게 로딩

public:
	HRESULT		Ready_Loading(LOADINGID eID);

	// 각 씬 컴포넌트를 추가하는 함수
	_uint		Loading_ForStage(void);
	_uint		Loading_ForBoss(void);

public: // 쓰레드 함수
	static unsigned int	CALLBACK Thread_Main(void* pArg);

private:
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_Crt;			// ﻿제공되는 자료형이고, 구조체 타입이고 각종 핸들, 포인터 정보로 채워져 있다
	LPDIRECT3DDEVICE9		m_pGraphicDev;  // Base가 디바이스 컴객체를 가지고 있지 않기에 새로 할당
	LOADINGID				m_eID;			// enum
	_bool					m_bFinish;		// 로딩이 끝났는지

public:
	static CLoading*		Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);
	virtual void			Free(void);

};

