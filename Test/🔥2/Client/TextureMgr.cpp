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

HRESULT CTextureMgr::Read_ImgPath(const wstring& wstrPath)
{
	wifstream		fin;

	fin.open(wstrPath, ios::in);

	if (!fin.fail())
	{
		TCHAR	szObjKey[MAX_STR] = L"";
		TCHAR	szStateKey[MAX_STR] = L"";
		TCHAR	szCount[MIN_STR] = L"";
		TCHAR	szPath[MAX_PATH] = L"";

		while (true)
		{
			//getline : '|' 단위로 끊어서 문자열 입력 가능
			fin.getline(szObjKey, MAX_STR, '|');
			fin.getline(szStateKey, MAX_STR, '|');
			fin.getline(szCount, MIN_STR, '|');
			fin.getline(szPath, MAX_PATH);

			if (fin.eof())
				break;

			int	iCount = _ttoi(szCount);

			if (FAILED(Insert_Texture(TEX_MULTI, szPath, szObjKey, szStateKey, iCount)))
				return E_FAIL;	
		}

		fin.close();
	}




	return S_OK;
}

HRESULT CTextureMgr::Insert_Texture(TEXTYPE eType, const TCHAR * pFilePath, const TCHAR * pObjKey, const TCHAR * pStateKey, const int & iCount)
{
	m_wstrFullPath = pFilePath;

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
			ERR_MSG(pFilePath);
			return E_FAIL;
		}

		m_MapTex.emplace(pObjKey, pTexture);
	}

	else if (eType == TEX_MULTI)
		iter->second->Insert_Texture(pFilePath, pStateKey, iCount);

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
