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

private:
	CGameInstance*			m_pGameInstance = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private: // CMainApp
	HRESULT Start_Level(LEVEL eLevelID);	
	HRESULT Ready_Prototype_Component();	// 컴포넌트 원형 추가 (ex_ Renderer컴포넌트)
	HRESULT Ready_Prototype_GameObject();	// 오브젝트 원형 추가 (ex_ 마우스 커서, UI)

public:
	static CMainApp* Create();
	virtual void Free() override;
};	

END