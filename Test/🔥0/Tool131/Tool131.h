
// Tool131.h : Tool131 ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CTool131App:
// �� Ŭ������ ������ ���ؼ��� Tool131.cpp�� �����Ͻʽÿ�.
//

class CTool131App : public CWinAppEx
{
public:
	CTool131App();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTool131App theApp;
