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
	

public:
	virtual void Free() override;
};	

END