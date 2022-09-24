#include "Animator.h"

#include "Export_Function.h"

USING(Engine)
CAnimator::CAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_fFrame(0.f)
{
}


Engine::CAnimator::CAnimator(const CAnimator& rhs)
	:CComponent(rhs)
	, m_fFrame(0.f)
{


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

void CAnimator::Set_Text()
{
	m_pTextureCom->Set_Texture((_ulong)m_fFrame);
}

HRESULT CAnimator::Add_Com(const _tchar * pTectureTag, COMPONENTID eID)
{
	m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(pTectureTag));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);

	//m_pTextureCom = dynamic_cast<CTexture*>(m_pGameObject->Find_Component(pTectureTag, eID));

	return S_OK;
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
	Safe_Release(m_pTextureCom);

	CComponent::Free();

}
