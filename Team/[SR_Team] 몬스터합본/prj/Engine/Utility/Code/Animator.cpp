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
	m_fFrame += m_pCurAnim->Get_FrameEnd() * fTimeDelta;

	if (m_fFrame >= m_pCurAnim->Get_FrameEnd())
		m_fFrame = 0;
}

void CAnimator::Change_Animation(const _tchar * pTectureTag)
{
	// auto
	map<const _tchar*, CTexture*>::iterator iter = find_if(m_mapAnimation.begin(), m_mapAnimation.end(), CTag_Finder(pTectureTag));

	if (iter == m_mapAnimation.end())
		return;

	m_pCurAnim = (*iter).second;
}

void CAnimator::Set_Texture()
{
	m_pCurAnim->Set_Texture((_ulong)m_fFrame);
}

HRESULT CAnimator::Add_Component(const _tchar * pTectureTag)
{
	m_pCurAnim = dynamic_cast<CTexture*>(Engine::Clone_Proto(pTectureTag));
	NULL_CHECK_RETURN(m_pCurAnim, E_FAIL);
	m_mapAnimation.insert({ pTectureTag, m_pCurAnim });
	
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
	for_each(m_mapAnimation.begin(), m_mapAnimation.end(), CDeleteMap());
	m_mapAnimation.clear();

	Safe_Release(m_pCurAnim);

	CComponent::Free();
}
