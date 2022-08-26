#pragma once
#include "Obj.h"

class CInventory : public CObj
{
public:
	CInventory();
	virtual ~CInventory();

public:
	virtual HRESULT Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render() override;
	virtual void Release(void) override;

private:
	bool		m_bOnInven;

};

