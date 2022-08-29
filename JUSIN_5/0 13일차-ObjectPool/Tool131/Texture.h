#pragma once

#include "Include.h"

class CTexture
{
public:
	CTexture();
	virtual ~CTexture();

public:
	virtual		HRESULT	Insert_Texture(const TCHAR* pFilePath,		// 파일 경로
									const TCHAR* pStateKey = L"",	// 상태 키값
									const int& iCount = 0)PURE;	    // 이미지 개수
	
	virtual		void	Release(void)PURE;

	virtual	const TEXINFO*		Get_Texture(const TCHAR* pStateKey = L"",	
											const int& iCount = 0)PURE;

};

