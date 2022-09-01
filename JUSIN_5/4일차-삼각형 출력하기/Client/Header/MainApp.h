#pragma once

#include "Engine_Include.h"
#include "Base.h"
#include "Export_Function.h"

BEGIN(Engine)

class CGraphicDev;
class CTimerMgr;
class CTriCol;

END

class CMainApp : public CBase
{
private:			// ������, �Ҹ���
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp(void);
	Engine::_int	Update_MainApp(const Engine::_float& fTimeDelta);
	void			LateUpdate_MainApp(void);
	void			Render_MainApp(void);


private:
	Engine::CGraphicDev*			m_pDeviceClass = nullptr;
	Engine::CTimerMgr*				m_pTimerMgr = nullptr;
	Engine::CTriCol*				m_pTriCol = nullptr;

	LPDIRECT3DDEVICE9				m_pGraphicDev;

public:			// ��ü ���� �Լ�
	static		CMainApp*		Create(void);

private:		// ��ü �Ҹ� �Լ�
	virtual		void			Free(void);
};

