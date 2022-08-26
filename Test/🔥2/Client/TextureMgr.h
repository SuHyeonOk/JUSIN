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
	wstring&		Get_String(void) { return m_wstrFullPath; }
	void			Set_String(const wstring& wstrString) { m_wstrFullPath = wstrString; }

public:
		HRESULT	Read_ImgPath(const wstring& wstrPath);

		HRESULT	Insert_Texture(TEXTYPE eType, const TCHAR* pFilePath,		// 파일 경로
								const TCHAR* pObjKey, 
								const TCHAR* pStateKey = L"",	// 상태 키값
								const int& iCount = 0);	    // 이미지 개수

		void	Release(void);

		const TEXINFO*		Get_Texture(const TCHAR* pObjKey,
										const TCHAR* pStateKey = L"",
										const int& iCount = 0);

private:
	map<wstring, CTexture*>			m_MapTex;
	wstring							m_wstrFullPath;
};

