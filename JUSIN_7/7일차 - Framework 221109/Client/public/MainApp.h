#pragma once

/* �� ������ �ʱ�ȭ */
/* �� ������ ����. */
/* �� ������ �������� ����. */

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
	HRESULT Ready_Prototype_Component();	// ������Ʈ ���� �߰� (ex_ Renderer������Ʈ)
	HRESULT Ready_Prototype_GameObject();	// ������Ʈ ���� �߰� (ex_ ���콺 Ŀ��, UI)

public:
	static CMainApp* Create();
	virtual void Free() override;
};	

END