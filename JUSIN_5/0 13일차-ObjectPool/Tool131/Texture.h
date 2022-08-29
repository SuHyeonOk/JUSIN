#pragma once

#include "Include.h"

class CTexture
{
public:
	CTexture();
	virtual ~CTexture();

public:
	virtual		HRESULT	Insert_Texture(const TCHAR* pFilePath,		// ���� ���
									const TCHAR* pStateKey = L"",	// ���� Ű��
									const int& iCount = 0)PURE;	    // �̹��� ����
	
	virtual		void	Release(void)PURE;

	virtual	const TEXINFO*		Get_Texture(const TCHAR* pStateKey = L"",	
											const int& iCount = 0)PURE;

};

