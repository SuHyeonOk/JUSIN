#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CUtilities_Manager : public CBase
{
	DECLARE_SINGLETON(CUtilities_Manager)

public:
	CUtilities_Manager();
	virtual ~CUtilities_Manager() = default;

public:	// ·£´ý°ª »Ì´Â ÇÔ¼ö
	_int Get_Random(_int iStart, _int iEnd);
	_float Get_Random(_float fStart, _float fEnd);

public:	// ·£´ýÇÑ ÁÂÇ¥ »Ì´Â ÇÔ¼ö
	_bool	Get_RandomPos(CTransform* pTransform, _float4 f4FirstPos, _float fRange, _double TimeDelta, _float fStart = 0.f, _float fEnd = 5.f);

private:	// Get_RandomPos()
	_float4				m_f4RandomPos;
	_bool				m_bRandomPos = false;

public:
	virtual void Free() override;	
};

END