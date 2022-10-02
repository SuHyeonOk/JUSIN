#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine) // namespace ���α�

class ENGINE_DLL CFrame : public CBase
{
private:
	explicit CFrame();
	virtual ~CFrame();

public:
	_bool		IsPermit_Call(const _float& fTimeDelta);
	HRESULT		Ready_Frame(const _float& fCallLimit);

private:
	_float		m_fCallLimit;		// 1�ʴ� �� ������? 
	_float		m_fAccTimeDelta;	// �����Ǵ� �ð� ����

public:
	static CFrame*	Create(const _float& fCallLimit);
	virtual	void	Free(void);

};
END
