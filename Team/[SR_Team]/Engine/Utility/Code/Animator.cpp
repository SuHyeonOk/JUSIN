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

HRESULT CAnimator::Ready_Animator()
{
	return S_OK;
}

void CAnimator::Play_Animation(const _float & fTimeDelta)
{
	m_fFrame += m_pTextureCom->Get_FrameEnd() * fTimeDelta;

	if (m_fFrame >= m_pTextureCom->Get_FrameEnd())
		m_fFrame = 0;
}

void CAnimator::Change_Animation(const _tchar * pTectureTag, COMPONENTID eID)
{
}

CAnimator * CAnimator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAnimator *		pInstance = new CAnimator(pGraphicDev);

		if (FAILED(pInstance->Ready_Animator()))
		{
			MSG_BOX("Animator Create Failed");
			Safe_Release(pInstance);
			return nullptr;
		}

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
