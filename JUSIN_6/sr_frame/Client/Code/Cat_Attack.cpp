#include "stdafx.h"
#include "Cat_Attack.h"
#include "Skeleton.h"
#include "StaticCamera.h"


CCat_Attack::CCat_Attack(LPDIRECT3DDEVICE9 pGraphicDev) :CSkeletalCube(pGraphicDev)
{
}

CCat_Attack::CCat_Attack(const CCat_Attack& rhs) : CSkeletalCube(rhs)
{
}

CCat_Attack::~CCat_Attack()
{
}

HRESULT CCat_Attack::Ready_Object(const wstring& wstrPath)
{
	CSkeletalCube::Ready_Object();
	if (wstrPath.empty() == false)
		LoadSkeletal(wstrPath);

	if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/bori.cube")
	{
		m_Idle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/walk_stacato.anim");
		m_fSpeed = 7.f;
	}
	else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/rui.cube")
	{
		m_Idle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/run.anim");
		m_fSpeed = 6.f;
	}
	else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/hoddeuk.cube")
	{
		m_Idle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/run.anim");
		m_fSpeed = 9.f;

	}
	m_pIdleAnim = &m_Idle;

	m_fLifeTime = 0.f;
	m_fTime = 2.3f;
	PlayAnimationLoop(m_pIdleAnim);
	return S_OK;
}

_int CCat_Attack::Update_Object(const _float& fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

	if(m_fLifeTime>= m_fTime)
	{
		return OBJ_DEAD;
	}

	m_fLifeTime += fTimeDelta;

	Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
		->PlayShake(0.1f, 0.1f);
	


	m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * fTimeDelta * m_fSpeed;


	return OBJ_NOEVENT;
}

void CCat_Attack::LateUpdate_Object()
{
	CSkeletalCube::LateUpdate_Object();
}

void CCat_Attack::Render_Object()
{
	CSkeletalCube::Render_Object();
}

CCat_Attack* CCat_Attack::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	CCat_Attack* pInstance = new CCat_Attack(pGraphicDev);


	if (FAILED(pInstance->Ready_Object(wstrPath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}


