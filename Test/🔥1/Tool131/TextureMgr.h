#pragma once

#include "Single.h"
#include "Multi.h"

class CTextureMgr
{
	DECLARE_SINGLETON(CTextureMgr)

private:
	CTextureMgr();
	~CTextureMgr();

public:
		HRESULT	Insert_Texture(TEXTYPE eType, const TCHAR* pFilePath,		// ���� ���
								const TCHAR* pObjKey, 
								const TCHAR* pStateKey = L"",	// ���� Ű��
								const int& iCount = 0);	    // �̹��� ����

		void	Release(void);

		const TEXINFO*		Get_Texture(const TCHAR* pObjKey,
										const TCHAR* pStateKey = L"",
										const int& iCount = 0);

private:
	map<wstring, CTexture*>			m_MapTex;
};

