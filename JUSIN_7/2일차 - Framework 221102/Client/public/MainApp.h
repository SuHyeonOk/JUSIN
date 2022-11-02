#pragma once

/* �� ������ �ʱ�ȭ */
/* �� ������ ����. */
/* �� ������ �������� ����. */

#include "Client_Defines.h"
#include "Base.h"

/*
class CMainApp
{
	private: or protected:
		������. 
		�Ҹ���.= default;
	public:
		Getter, Setter	
	public:
		�Լ�.
	protected:
		����.
	protected:
		�Լ�.
	private:
		����.
	private:
		�Լ�.

	public:
		�������õ� �Լ�. (Create, Clone)
		�������õ� �Լ�. (Free)

};
*/

BEGIN(Engine)
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

private: // CMainApp
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	
	
public:
	static CMainApp* Create();
	virtual void Free() override;
};	

END