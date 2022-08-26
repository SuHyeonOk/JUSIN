#include "stdafx.h"
#include "TextureMgr.h"

IMPLEMENT_SINGLETON(CTextureMgr)

CTextureMgr::CTextureMgr()
{
}


CTextureMgr::~CTextureMgr()
{
	Release();
}

HRESULT CTextureMgr::Insert_Texture(TEXTYPE eType, const TCHAR * pFilePath, const TCHAR * pObjKey, const TCHAR * pStateKey, const int & iCount)
{
	auto	iter = find_if(m_MapTex.begin(), m_MapTex.end(),
		[&](auto& MyPair)->bool
	{
		if (MyPair.first == pObjKey)
			return true;

		return false;
	});

	if (iter == m_MapTex.end())
	{
		CTexture*		pTexture = nullptr;

		switch (eType)
		{
		case TEX_SINGLE:
			pTexture = new CSingle;
			break;

		case TEX_MULTI:
			pTexture = new CMulti;
			break;
		}

		if (FAILED(pTexture->Insert_Texture(pFilePath, pStateKey, iCount)))
		{
			AfxMessageBox(pFilePath);
			return E_FAIL;
		}

		m_MapTex.emplace(pObjKey, pTexture);

	}

	return S_OK;
}

void CTextureMgr::Release(void)
{
	for_each(m_MapTex.begin(), m_MapTex.end(), [](auto& MyPair) { Safe_Delete(MyPair.second); });
	m_MapTex.clear();
}

const TEXINFO * CTextureMgr::Get_Texture(const TCHAR * pObjKey, const TCHAR * pStateKey, const int & iCount)
{
	// auto	iter = find_if(m_MapTex.begin(), m_MapTex.end(), CTag_Finder(pObjKey));
	
	auto	iter = find_if(m_MapTex.begin(), m_MapTex.end(), 
	[&](auto& MyPair)->bool 
	{
		if (MyPair.first == pObjKey)
			return true;

		return false;
	});


	if(iter == m_MapTex.end())
		return nullptr;

	return iter->second->Get_Texture(pStateKey, iCount);
}
