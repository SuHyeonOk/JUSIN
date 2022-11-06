#include "stdafx.h"
#include "..\Header\Cat_Idle.h"

CCat_Idle::CCat_Idle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSkeletalCube(pGraphicDev)
{
}

CCat_Idle::CCat_Idle(const CCat_Idle & rhs)
	: CSkeletalCube(rhs)
{
}

CCat_Idle::~CCat_Idle()
{
}

HRESULT CCat_Idle::Ready_Object(const wstring & wstrPath)
{
	CSkeletalCube::Ready_Object();
	if (wstrPath.empty() == false)
		LoadSkeletal(wstrPath);


	if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/bori.cube")
	{
		m_iIdle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/cat_Applause.anim");
	}

	//else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/bori.cube")
	//{
	//	m_iMove = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/cat_Falling down.anim");
	//}

	else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/rui.cube")
	{
		m_iIdle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/love_jjh.anim");
	}

	//else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/rui.cube")
	//{
	//	m_iMove = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/idle.anim");
	//}

	else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/hoddeuk.cube")
	{
		m_iIdle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/cat_Stretch.anim");
	}

	//else if (wstrPath == L"../Bin/Resource/SkeletalCube/Object/hoddeuk.cube")
	//{
	//	m_iMove = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/cat_Scratching.anim");
	//}

	m_pIdleAnim = &m_iIdle;
	PlayAnimationLoop(&m_iIdle);

	return S_OK;
}

_int CCat_Idle::Update_Object(const _float & fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CCat_Idle::LateUpdate_Object()
{
	CSkeletalCube::LateUpdate_Object();
}

void CCat_Idle::Render_Object()
{
	CSkeletalCube::Render_Object();
}

void CCat_Idle::AnimationEvent(const string & strEnvet)
{
	if (strEnvet == "cat_Applause")
	{
		m_bMove = true;
	}

	else if (strEnvet == "cat_ApplauseEnd")
	{
		m_bCanPlayAnim = true;
	}
}

CCat_Idle * CCat_Idle::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring & wstrPath)
{
	CCat_Idle* pInstance = new CCat_Idle(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(wstrPath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
