#pragma once

/* ���� ȭ�鿡 ��������� ������ �ּҸ� ������ �ִ´�. */
/* �Ҵ�� ������ ƽ�� ������ ȣ���Ѵ�. */
/* ���������� ���ƴ� �ڿ��� �����Ѵ�. */
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public: // CLevel_Manager
	HRESULT Open_Level(class CLevel* pNewLevel);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	HRESULT Render();

private:
	class CLevel*				m_pCurrentLevel = nullptr;

public:
	virtual void Free() override;
};

END