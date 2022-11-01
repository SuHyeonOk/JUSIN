#pragma once

/* 내 게임의 초기화 */
/* 내 게임의 진행. */
/* 내 게임의 정리까지 수행. */

#include "Client_Defines.h"
#include "Base.h"

/*
class CMainApp
{
	private: or protected:
		생성자. 
		소멸자.= default;
	public:
		Getter, Setter	
	public:
		함수.
	protected:
		변수.
	protected:
		함수.
	private:
		변수.
	private:
		함수.

	public:
		생성관련된 함수. (Create, Clone)
		삭제관련된 함수. (Free)

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