#pragma once
#include "Obj.h"

class CMouse : public CObj
{
public:
	CMouse();
	virtual ~CMouse();

public:
	virtual void OnCollision(CObj * pOther) override;

public:
	virtual HRESULT Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render() override;
	virtual void Release(void) override;


};

