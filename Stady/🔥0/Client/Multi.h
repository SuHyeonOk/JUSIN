#pragma once
#include "Texture.h"
class CMulti :
	public CTexture
{
public:
	CMulti();
	virtual ~CMulti();

public:
	virtual HRESULT Insert_Texture(const TCHAR* pFilePath, const TCHAR* pStateKey = L"", const int& iCount = 0) override;
	virtual void Release(void) override;
	virtual const TEXINFO* Get_Texture(const TCHAR* pStateKey = L"", const int& iCount = 0) override;

private:
	map<wstring, vector<TEXINFO*>>			m_MapMultiTex;
};

