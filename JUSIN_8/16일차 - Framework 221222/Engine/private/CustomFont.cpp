#include "..\public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)	
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar * pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	m_pBatch = new SpriteBatch(m_pContext);

	return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _float fRadian, _float2 vScale, _fvector vColor)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);	// 지오메트릭 셰이더를 사용하지 않는다.!

	m_pBatch->Begin();

	// 요기서 Font 에 대한 여러가지를 인자값으로 넣어 사용할 수 있다. 넣어주지 않은 값은 Default 로 사용한다.
	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRadian, _float2(0.f, 0.f), vScale);
	// 1 인자 : 그리기 위한 
	// 2 인자 : 넣을 텍스트
	// 3 인자 : 위치
	// 4 인자 : 색상
	// 5 인자 : 각도 (원래는 Default 로 사용하는데 7인자 를 사용하기 위해서 넣는다.)
	// 6 인자 : 원점
	// 7 인자 : 크기

	m_pBatch->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontFilePath)
{
	CCustomFont*		pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pFont);
	Safe_Delete(m_pBatch);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
