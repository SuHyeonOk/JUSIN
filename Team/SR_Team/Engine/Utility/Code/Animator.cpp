#include "Animator.h"

USING(Engine)
#include "Export_Function.h"

CAnimator::CAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_fFrame(0.f)
{
}


Engine::CAnimator::CAnimator(const CAnimator& rhs)
	:CComponent(rhs)
{
	_uint iSize = rhs.m_vecTexture.size();
	m_vecTexture.reserve(iSize);

	m_vecTexture = rhs.m_vecTexture;

	for (_uint i = 0; i < iSize; ++i)
		m_vecTexture[i]->AddRef();

}

CAnimator::~CAnimator()
{
}

HRESULT CAnimator::Ready_Animator(const _tchar * pPath, TEXTUREID eType, const _uint & iCnt)
{
	m_vecTexture.reserve(iCnt);

	IDirect3DBaseTexture9*			pTexture = nullptr;

	for (_uint i = 0; i < iCnt; ++i)
	{
		TCHAR		szFileName[256] = L"";

		wsprintf(szFileName, pPath, i);

		switch (eType)
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

void CAnimator::Play_Animation(const _float & fTimeDelta)
{
	m_fFrame += m_pTextureCom->Get_FrameEnd() * fTimeDelta;

	if (m_fFrame >= m_pTextureCom->Get_FrameEnd())
		m_fFrame = 0;
}

CAnimator * CAnimator::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pPath, TEXTUREID eType, const _uint & iCnt)
{
	CAnimator *		pInstance = new CAnimator(pGraphicDev);

	//if (FAILED(pInstance->Ready_Texture(pPath, eType, iCnt)))
	//{
	//	MSG_BOX("Texture Create Failed");
	//	Safe_Release(pInstance);
	//	return nullptr;
	//}

	return pInstance;
}

CComponent * CAnimator::Clone(void)
{
	return new CAnimator(*this);
}

void CAnimator::Free(void)
{
	CComponent::Free();

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
