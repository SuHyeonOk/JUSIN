#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)
private:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	// 폰트를 생성하면서, map 에 보관하기
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor);

private:
	map<const _tchar*, class CCustomFont*>			m_Fonts;	// 탐색을 많이 해야해서 map

private:
	class CCustomFont* Find_Font(const _tchar* pFontTag);		// 넣은 폰트를 찾기 위해서

public:
	virtual void Free() override;
};

END