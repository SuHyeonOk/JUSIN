#pragma once

/* �̱���.  */
/* Ŭ���̾�Ʈ�� ������ ������ ���� ������� Ŭ����. */

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