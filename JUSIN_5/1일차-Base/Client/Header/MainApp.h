#pragma once

#include "Include.h"
#include "Base.h"

class CMainApp : public CBase
{
private:			// 생성자, 소멸자
	explicit CMainApp(); // explicit 묵시적 형변환을 막음 (명시적 형변환만ㅇ)
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp(void);
	_int			Update_MainApp(const _float& fTimeDelta);
	void			LateUpdate_MainApp(void);
	void			Render_MainApp(void);

public:			// 객체 생성 함수
	static		CMainApp*		Create(void);

private:		// 객체 소멸 함수
	virtual		void			Free(void);
	
	
};

