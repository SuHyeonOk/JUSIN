#pragma once
#include "Texture.h"
class CSingle :
	public CTexture
{
public:
	CSingle();
	virtual ~CSingle();
	
public:
	virtual const TEXINFO* Get_Texture(const TCHAR* pStateKey = L"", const int& iCount = 0) override
	{
		return m_pTexInfo;
	}

	virtual HRESULT Insert_Texture(const TCHAR* pFilePath, const TCHAR* pStateKey = L"", const int& iCount = 0) override;
	virtual void Release(void) override;

private:
	TEXINFO*		m_pTexInfo;

};

