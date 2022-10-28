#include "stdafx.h"
#include "Cat.h"

CCat::CCat(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSkeletalCube(pGraphicDev)
{
}

CCat::CCat(const CCat& rhs)
	: CSkeletalCube(rhs)
{
}

CCat::~CCat()
{
}

HRESULT CCat::Ready_Object()
{
	CSkeletalCube::Ready_Object();

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/idle.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/walk.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 2.5f;

	//pController->SetOwner(this);

	return S_OK;
}

_int CCat::Update_Object(const _float & fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

	if (m_pCurAnim == m_pIdleAnim)
		m_bCanPlayAnim = true;

	StateChange();

	switch (m_eState)
	{
	case IDLE:
		break;
	case WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_vMoveDirNormal * m_fSpeed * fTimeDelta;
		break;
	case STATE_END:
		break;
	default:
		break;
	}

	return OBJ_NOEVENT;
}

void CCat::LateUpdate_Object()
{
	CSkeletalCube::LateUpdate_Object();
}

void CCat::Free()
{
	CSkeletalCube::Free();
}

CCat * CCat::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring & wstrPath)
{
	CCat* pInstance = new CCat(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CCat::StateChange()
{
	if (m_bMove && m_bCanPlayAnim)
	{
		m_eState = WALK;
		m_pIdleAnim = &m_arrAnim[ANIM_WALK];
		m_pCurAnim = &m_arrAnim[ANIM_WALK];
		return;
	}

	if (m_bCanPlayAnim)
	{
		m_eState = IDLE;
		m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
		m_pCurAnim = &m_arrAnim[ANIM_IDLE];
		return;
	}
}
