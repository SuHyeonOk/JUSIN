#pragma once
#include "Engine_Include.h"
#include "Base.h"

// 씬은 아니지만 씬과 관련된 요소들을 관리하는 클래스
// 멀티스레드 구현 => window.h  &	process.h 필요
class CLoading : public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	CRITICAL_SECTION	Get_Crt()		{ return m_Crt; }
	_bool				Get_Finish()	{ return m_bFinish; }
	LOADINGID			Get_LoadingID() { return m_eID; }
	const _tchar*		Get_String(void) { return m_szLoading; }

public:
	HRESULT		Ready_Loading(LOADINGID eID);
	_uint		Loading_ForStage(void);
	_uint		Loading_ForBoss(void);

public:
					// 스레드 함수는 반드시 stdcall 방식으로 지정
	static unsigned	int CALLBACK Thread_Main(void* pArg);
			// Argument(인자)		: 실제로 함수가 호출될 때 넘기는 변수값
			// Parameter(매개변수)	: 함수를 정의할 때 사용되는 변수


private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LOADINGID			m_eID;
	_bool				m_bFinish;
	_tchar				m_szLoading[128];


public:
	static CLoading*	Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);
	virtual void		Free(void);
};

