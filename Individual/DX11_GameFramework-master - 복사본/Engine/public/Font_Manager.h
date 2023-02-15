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
	// ��Ʈ�� �����ϸ鼭, map �� �����ϱ�
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor);

private:
	map<const _tchar*, class CCustomFont*>			m_Fonts;	// Ž���� ���� �ؾ��ؼ� map

private:
	class CCustomFont* Find_Font(const _tchar* pFontTag);		// ���� ��Ʈ�� ã�� ���ؼ�

public:
	virtual void Free() override;
};

END