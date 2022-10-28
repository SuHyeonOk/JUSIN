#include "stdafx.h"
#include "..\Header\Cat.h"
#include "Player.h"
#include "StatComponent.h"
#include "TerrainCubeMap.h"

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

	
	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/idle_b.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/walk_stacato.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 2.5f;

	return S_OK;
}

_int CCat::Update_Object(const _float & fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

	/*_vec3& vCat2Pos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
	IM_BEGIN("cat");

	ImGui::Text("x : %f, y : %f,  z : %f", vCat2Pos.x, vCat2Pos.y, vCat2Pos.z);
	IM_END;*/

	if (m_pCubeMap == nullptr)
	{
		m_pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
		m_pCubeMap->AddRef();
	}

	_vec3& vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
	vPos.y = m_pCubeMap->GetHeight(vPos.x, vPos.z);

	if (m_pCurAnim == m_pIdleAnim)
		m_bCanPlayAnim = true;

	//_vec3& vPlayerPos = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC)
	//	->m_vInfo[INFO_LOOK];


	StateChange();

	switch (m_eState)
	{
	case CCat::IDLE:
		break;
	case CCat::WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		break;
	case CCat::STATE_END:
		break;
	default:
		break;
	}
	
	_float fDist = D3DXVec3Length(&(vPos - vEndPos));

	/*if (fDist <= 1.5f)
	{
		m_bGo = false;
	}*/

	/*if (vPos.y == 9.f)
	{
		m_bGo = false;
	}*/

	if (vPos.z >= 30.f)
	{
		m_bGo = false;
	}

	if (m_bGo)
	{
		m_bMove = true;
		m_pRootPart->pTrans->SetRotation(_vec3(0.f, 0.f, 0.f));
		m_pRootPart->pTrans->m_vInfo[INFO_LOOK] += m_pRootPart->pTrans->m_vInfo[INFO_POS];
	}

	else
	{
		m_bMove = true;
		m_pRootPart->pTrans->Rotation(ROT_Y, -0.4f);
		m_pRootPart->pTrans->m_vInfo[INFO_LOOK] -= m_pRootPart->pTrans->m_vInfo[INFO_POS];
	}
	
	if (vPos.y == 7.f)
	{
		m_bGo = true;
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
	Safe_Release(m_pCubeMap);
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
		RotateToTargetPos(m_vTargetPos);
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

void CCat::RotateToTargetPos(const _vec3 & vTargetPos, bool bReverse)
{
	_vec3 vLook = vTargetPos - m_pRootPart->pTrans->m_vInfo[INFO_POS];
	if (bReverse) vLook = -vLook;

	D3DXVec3Normalize(&vLook, &vLook);

	const _vec2 v2Look{ 0.f, 1.f };
	_vec2 v2ToDest{ vLook.x, vLook.z };

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		m_pRootPart->pTrans->m_vAngle.y = -acosf(fDot);
	else
		m_pRootPart->pTrans->m_vAngle.y = acosf(fDot);
}

