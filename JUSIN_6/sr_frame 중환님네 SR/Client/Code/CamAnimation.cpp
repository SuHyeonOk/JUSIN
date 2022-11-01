#include "stdafx.h"
#include "..\Header\CamAnimation.h"
#include "Player.h"

CCamAnimation::CCamAnimation(LPDIRECT3DDEVICE9 pGraphicDev) : CSkeletalCube(pGraphicDev)
{
}

CCamAnimation::CCamAnimation(const CCamAnimation& rhs) : CSkeletalCube(rhs)
{
}

CCamAnimation::~CCamAnimation()
{
}

HRESULT CCamAnimation::Ready_Object()
{
	CSkeletalCube::Ready_Object();


	return S_OK;
}

_int CCamAnimation::Update_Object(const _float& fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

	if (m_bFinish) return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CCamAnimation::AnimationEvent(const string& strEvent)
{
	if (strEvent == "AnimStopped")
	{
		// anim finish
		m_bFinish = true;
	}
	else if (strEvent == "LandPlayer")
	{
		Get_GameObject<CPlayer>(LAYER_PLAYER, L"Player")->PlayerSpawn();
	}
	else if (strEvent == "RedStoneBossStop")
	{

	}
}

void CCamAnimation::Render_Object()
{
}

CCamAnimation* CCamAnimation::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrAnimPath)
{
	auto pInst = new CCamAnimation(pGraphicDev);
	if (FAILED(pInst->Ready_Object()))
	{
		Safe_Release(pInst);
		return nullptr;
	}
	pInst->LoadSkeletal(L"../Bin/Resource/SkeletalCube/Object/cam.cube");
	pInst->m_AnimFrame = CubeAnimFrame::Load(wstrAnimPath);


	
	pInst->PlayAnimationOnce(&pInst->m_AnimFrame, true);

	return pInst;
}

void CCamAnimation::Free()
{
	CSkeletalCube::Free();
}

void CCamAnimation::GetCamWorld(_matrix& matWorld)
{
	matWorld = m_mapParts.find("body")->second->GetWorldMat();
}
