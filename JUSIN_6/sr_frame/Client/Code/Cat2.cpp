#include "stdafx.h"
#include "..\Header\Cat2.h"
#include "Player.h"
#include "StatComponent.h"
#include "TerrainCubeMap.h"
#include "Box.h"

CCat2::CCat2(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSkeletalCube(pGraphicDev)
{
}

CCat2::CCat2(const CCat2 & rhs)
	: CSkeletalCube(rhs)
{
}

CCat2::~CCat2()
{
}

HRESULT CCat2::Ready_Object()
{
	CSkeletalCube::Ready_Object();
	
	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/cat_Scratching.anim");
	// m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/idle_b.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Cat/walk_e.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 3.5f;

	return S_OK;
}

_int CCat2::Update_Object(const _float & fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

	/*IM_BEGIN("cat");

	ImGui::Text("x : %f, z : %f", vCat2Pos.x, vCat2Pos.z);
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

	StateChange();

	switch (m_eState)
	{
	case CCat2::IDLE:
		break;
	case CCat2::WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		break;
	case CCat2::STATE_END:
		break;
	default:
		break;
	}

	_vec3& vPlayerPos = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC)
		->m_vInfo[INFO_POS];

	_vec3& vBoxPos = Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
		->m_vInfo[INFO_POS];

	_vec3& vCat2Pos = m_pRootPart->pTrans->m_vInfo[INFO_POS];

	_float fDist = D3DXVec3Length(&(vCat2Pos - vPlayerPos));
	if (fDist < 4.f)
	{
		m_bMove = true;
	}

	if (m_bMove)
	{
		vBoxPos + m_pRootPart->pTrans->m_vInfo[INFO_POS] * m_fSpeed * fTimeDelta;
		vBoxPos + m_pRootPart->pTrans->m_vInfo[INFO_RIGHT] * m_fSpeed * fTimeDelta;
		vBoxPos + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;

		//if (fRute < 1.f)
		//{
		//	// vPlayerPos -= m_pRootPart->pTrans->m_vInfo[INFO_POS];
		//	

		//}
			

		/*if (m_pRootPart->pTrans->m_vInfo[INFO_POS].z > 90)
		{
			m_pRootPart->pTrans->m_vInfo[INFO_POS].z = 88.f;
			m_pRootPart->pTrans->m_vInfo[INFO_POS].x = 22.f;
			m_bMove = false;
		}*/

	}

	// m_pRootPart->pTrans->Update_Component(0.f);

	return OBJ_NOEVENT;
}

void CCat2::LateUpdate_Object()
{
	CSkeletalCube::LateUpdate_Object();

	CBox* pBoxTarget = nullptr;
	_vec3 vTargetPos;
	_float fTargetDist = 3.f;

	for (auto& ele : Get_Layer(LAYER_GAMEOBJ)->Get_MapObject())
	{
		if (CBox* pBox = dynamic_cast<CBox*>(ele.second))
		{
			vTargetPos = pBox->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDIff = vTargetPos - m_pRootPart->pTrans->m_vInfo[INFO_POS];
			_float fDistance = D3DXVec3Length(&vDIff);

			if (fDistance < m_fBoxRange && fTargetDist > fDistance)
			{
				pBoxTarget = pBox;
				fTargetDist = fDistance;
			}
		}
	}
	
	if (fTargetDist < m_fMoveDist)
	{

		 m_vTargetPos;
		 m_pRootPart->pTrans->m_vInfo[INFO_POS].z -= 2.5f - m_fSpeed;
		 m_pRootPart->pTrans->Rotation(ROT_Y, 0.f);

		m_bMove = false;


		// m_pRootPart->pTrans->m_vInfo[INFO_POS].z += 2.5f
		//m_pRootPart->pTrans->m_vInfo[INFO_POS].z += 2.5f;
	}


}

void CCat2::Free()
{
	CSkeletalCube::Free();
	Safe_Release(m_pCubeMap);
}

CCat2* CCat2::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring & wstrPath)
{
	CCat2* pInstance = new CCat2(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CCat2::StateChange()
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

void CCat2::RotateToTargetPos(const _vec3 & vTargetPos, bool bReverse)
{
	_vec3 vLook = vTargetPos - m_pRootPart->pTrans->m_vInfo[INFO_POS];
	if (bReverse) vLook = -vLook;

	D3DXVec3Normalize(&vLook, &vLook);

	const _vec2 v2Look{ 0.f,1.f };
	_vec2 v2ToDest{ vLook.x,vLook.z };

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		m_pRootPart->pTrans->m_vAngle.y = -acosf(fDot);
	else
		m_pRootPart->pTrans->m_vAngle.y = acosf(fDot);
}
