#pragma once
#include "Base.h"
#include "Engine_Include.h"

class ENGINE_DLL CMyFont : public CBase
{
private:
	explicit CMyFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMyFont();
};

