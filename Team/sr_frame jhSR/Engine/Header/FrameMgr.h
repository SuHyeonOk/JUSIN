#pragma once

#include "Frame.h"
#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CFrameMgr : public CBase
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	virtual ~CFrameMgr();

public:
	_bool		IsPermit_Call(const wstring& pFrameTag, const _float& fTimeDelta);
	HRESULT		Ready_Frame(const wstring& pFrameTag, const _float& fCallLimit);

private:
	CFrame*		Find_Frame(const wstring& pFrameTag);
private:
	map<wstring, CFrame*>			m_mapFrame;

public:
	virtual void	Free(void);
};

END