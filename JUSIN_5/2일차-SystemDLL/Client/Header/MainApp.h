#pragma once

#include "Engine_Include.h"
#include "Base.h"
#include "Export_Function.h"

BEGIN(Engine)

class CGraphicDev;
class CTimerMgr;

END

class CMainApp : public CBase
{
private:			// 생성자, 소멸자
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp(void);
	Engine::_int			Update_MainApp(const Engine::_float& fTimeDelta);
	void			LateUpdate_MainApp(void);
	void			Render_MainApp(void);


private:
	// 자료형만 필요한 경우이니 전방선언
	// 엔진에 선언되어 있으니 Engine::
	// 엔진 안에 있는 자료형을 전방선언 하겠다!
	Engine::CGraphicDev*			m_pDeviceClass = nullptr; 
	Engine::CTimerMgr*				m_pTimerMgr = nullptr;
	
public:			// 객체 생성 함수
	static		CMainApp*		Create(void);

private:		// 객체 소멸 함수
	virtual		void			Free(void);
};

