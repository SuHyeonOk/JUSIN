#pragma once

/* 싱글톤.  */
/* 클라이언트와 엔진의 소통을 위해 만들어진 클래스. */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Graphic_Device */
	HRESULT Clear_Graphic_Device(const _float4* pColor);
	HRESULT Present();

private:
	class CGraphic_Device*			m_pGraphic_Device = nullptr;

public:
	virtual void Free() override;
};

END