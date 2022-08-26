#pragma once
#include "Obj.h"

class CUI_InventoryDregBar : public CObj
{
public:
	CUI_InventoryDregBar();
	virtual ~CUI_InventoryDregBar();

public:
	virtual HRESULT Initialize(void) override; // ���⼭ �巹�� �κ� ����
	virtual int Update(void) override; // �巹�� �κ��� ��ġ�� �ٲٸ� �� Ŭ���� ��ġ�� �ٲ��
	virtual void Late_Update(void) override;
	virtual void Render() override;
	virtual void Release(void) override;

private:
	void		Key_Input(void);

private:
	bool		m_bOnInven;

};

