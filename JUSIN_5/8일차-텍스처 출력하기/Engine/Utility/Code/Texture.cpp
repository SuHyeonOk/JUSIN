#include "..\..\Header\Texture.h"

USING(Engine)
#include "Export_Function.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}


Engine::CTexture::CTexture(const CTexture& rhs)
	:CComponent(rhs)
{
	_uint iSize = rhs.m_vecTexture.size();
	m_vecTexture.reserve(iSize); // 크기 예약(capacity를 재할당)

	m_vecTexture = rhs.m_vecTexture; // 얕은 복사

	// ﻿대입을 하면서 복사를 하기에 레퍼런스 카운터를 올려준다
	for (_uint i = 0; i < iSize; ++i)
		m_vecTexture[i]->AddRef();

}

CTexture::~CTexture()
{
}

HRESULT CTexture::Ready_Texture(const _tchar * pPath, TEXTUREID eType, const _uint & iCnt)
{
	m_vecTexture.reserve(iCnt);

	IDirect3DBaseTexture9*			pTexture = nullptr;

	// iCnt 텍스처의 개수 만큼 컴 객체를 만든다
	for (_uint i = 0; i < iCnt; ++i)
	{
		TCHAR		szFileName[256] = L"";

		wsprintf(szFileName, pPath, i); // ﻿szFileName기준으로 컴객체 생성

		switch (eType) // 순수히 컴 객체만 생성
		{ 
		case TEX_NORMAL: 
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;
			break;

		case TEX_CUBE:
			if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;
			break;
		}		
		m_vecTexture.push_back(pTexture);
	}	

	return S_OK;
}

void CTexture::Set_Texture(const _uint & iIndex)
{
	if (m_vecTexture.size() <= iIndex) // size보다 큰 index는 없다
		return;

	// SetTexture 텍스처를 출력하는 함수라고 생각해도 ok
	m_pGraphicDev->SetTexture(0, m_vecTexture[iIndex]);
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pPath, TEXTUREID eType, const _uint & iCnt)
{
	CTexture *		pInstance = new CTexture(pGraphicDev);
	
	if (FAILED(pInstance->Ready_Texture(pPath, eType, iCnt)))
	{
		MSG_BOX("Texture Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CComponent * CTexture::Clone(void)
{
	return new CTexture(*this);
}

void CTexture::Free(void)
{
	CComponent::Free();

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
