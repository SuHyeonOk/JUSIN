#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CFrame : public CBase
{
private:
	explicit CFrame();
	virtual ~CFrame();

public:
	_bool		IsPermit_Call(const _float& fTimeDelta);
	HRESULT		Ready_Frame(const _float& fCallLimit);

private: // 프레임의 제한을 두기 위한 변수(60프레임 👉 60초에 한 번)
	_float				m_fCallLimit;		// 몇 프레임인지
	_float				m_fAccTimeDelta;	// 값을 중첩시키기 위한 용도

public:
	static CFrame*		Create(const _float& fCallLimit);
	virtual void		Free(void);
};

END