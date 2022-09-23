#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine) // namespace 감싸기

class ENGINE_DLL CFrame : public CBase
{
private:
	explicit CFrame();
	virtual ~CFrame();

public:
	_bool		IsPermit_Call(const _float& fTimeDelta);
	HRESULT		Ready_Frame(const _float& fCallLimit);

private:
	_float		m_fCallLimit;		// 1초당 몇 프레임? 
	_float		m_fAccTimeDelta;	// 누적되는 시간 측정

public:
	static CFrame*	Create(const _float& fCallLimit);
	virtual	void	Free(void);

};
END
