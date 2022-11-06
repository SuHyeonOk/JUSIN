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
	wstring							m_wstrFullPath;
};

