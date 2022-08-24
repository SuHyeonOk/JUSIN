#pragma once
#include "Obj.h"

class CMonster : public CObj
{
public:
	CMonster();
	~CMonster();

public:
	virtual void OnCollision(CObj * pOther) override;

public:
	virtual HRESULT Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render() override;
	virtual void Release(void) override;


};

