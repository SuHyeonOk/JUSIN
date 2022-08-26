#pragma once
#include "Obj.h"

class CUI_InventoryDregBar : public CObj
{
public:
	CUI_InventoryDregBar();
	virtual ~CUI_InventoryDregBar();

public:
	virtual HRESULT Initialize(void) override; // 여기서 드레그 인벤 생성
	virtual int Update(void) override; // 드레그 인벤의 위치가 바꾸면 이 클래스 위치도 바뀌게
	virtual void Late_Update(void) override;
	virtual void Render() override;
	virtual void Release(void) override;

private:
	void		Key_Input(void);

private:
	bool		m_bOnInven;

};

