#pragma once

#include "Engine_Include.h"
#include "Base.h"
#include "Export_Function.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;
class CTimerMgr;

END

class CMainApp : public CBase
{
private:			// 생성자, 소멸자
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp(void);
	Engine::_int	Update_MainApp(const Engine::_float& fTimeDelta);
	void			LateUpdate_MainApp(void);
	void			Render_MainApp(void);

private:
	HRESULT			SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT			Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);

private:
	Engine::CGraphicDev*			m_pDeviceClass = nullptr;
	Engine::CManagement*			m_pManagementClass = nullptr;
	Engine::CTimerMgr*				m_pTimerMgr = nullptr;

	LPDIRECT3DDEVICE9				m_pGraphicDev;



public:			// 객체 생성 함수
	static		CMainApp*		Create(void);

private:		// 객체 소멸 함수
	virtual		void			Free(void);
};

// D3DXIntersectTri() 함수를 이용하여 마우스 클릭에 따른 플레이어 이동을 만들어와라
