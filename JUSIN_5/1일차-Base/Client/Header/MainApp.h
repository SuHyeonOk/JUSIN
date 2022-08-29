#pragma once

#include "Include.h"
#include "Base.h"

class CMainApp : public CBase
{
private:			// ������, �Ҹ���
	explicit CMainApp(); // explicit ������ ����ȯ�� ���� (����� ����ȯ����)
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp(void);
	_int			Update_MainApp(const _float& fTimeDelta);
	void			LateUpdate_MainApp(void);
	void			Render_MainApp(void);

public:			// ��ü ���� �Լ�
	static		CMainApp*		Create(void);

private:		// ��ü �Ҹ� �Լ�
	virtual		void			Free(void);
	
	
};

