#pragma once

/* 내 게임의 초기화 */
/* 내 게임의 진행. */
/* 내 게임의 정리까지 수행. */

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();	

public:
	HRESULT	Resize_BackBuffer();

private:
	CGameInstance*			m_pGameInstance = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	HRESULT Start_Level(LEVEL eLevelID);	
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_GameObject();

#ifdef _DEBUG
private: // FPS 띄우기 위한 변수
	_tchar	m_szFPS[17] = L"";
	_int	m_iNumDraw = 0;
	_double	m_dTimeAcc = 0;
#endif

private:
	_float4	m_PickingPos;

public:
	static CMainApp* Create();
	virtual void Free() override;
};	

END