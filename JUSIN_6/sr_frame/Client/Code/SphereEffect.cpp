#include "stdafx.h"
#include "..\Header\SphereEffect.h"
#include "RedStoneMonstrosity.h"

CSphereEffect::~CSphereEffect()
{
}

HRESULT CSphereEffect::Ready_Object(_float _size, SPHERETYPE _type)
{
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	// m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));


	if (_type == GOLEM_MELEE_L)
	{
		m_pBufferCom = Add_Component<CSphereMesh>(L"Proto_SphereCom_Ring", L"Proto_SphereCom_Ring", ID_STATIC);

		CTransform* pTrans= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pTrans = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}

		_vec3 pPos = pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = pTrans->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&vLook, &vLook);
		pPos += vLook * 3.f;
		m_pTransCom->Set_Pos(pPos.x, pPos.y + 0.25f, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fCurTime = 0.f;
		m_fTime = 0.65f;
		m_fSpeed = 62.f;
	}
	else if (_type == GOLEM_MELEE_M)
	{
		m_pBufferCom = Add_Component<CSphereMesh>(L"Proto_SphereCom_Lava", L"Proto_SphereCom_Lava", ID_STATIC);
		CTransform* pTrans = nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pTrans = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
				break;
			}
		}
		_vec3 pPos = pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = pTrans->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&vLook, &vLook);
		pPos += vLook * 3.f;
		m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fCurTime = 0.f;
		m_fTime = 0.65f;
		m_fSpeed = 61.75f;
	}

	else if (_type == GOLEM_MELEE_S)
	{
		m_pBufferCom = Add_Component<CSphereMesh>(L"Proto_SphereCom_Ring", L"Proto_SphereCom_Ring", ID_STATIC);
		CTransform* pTrans= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pTrans = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}
		_vec3 pPos = pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = pTrans->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&vLook, &vLook);
		pPos += vLook * 3.f;
		m_pTransCom->Set_Pos(pPos.x, pPos.y - 0.25f, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		
		m_fCurTime = 0.f;
		m_fTime = 0.7f;
		m_fSpeed = 61.f;
	}
	else if (_type == SPHERE_L)
	{
		m_pBufferCom = Add_Component<CSphereMesh>(L"Proto_SphereCom_L", L"Proto_SphereCom_L", ID_STATIC);
		CTransform* pTrans= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pTrans = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}
		_vec3 pPos = pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = pTrans->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&vLook, &vLook);
		pPos += vLook * 3.f;
		m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fCurTime = 0.f;
		m_fTime = 0.6f;
		m_fSpeed = 62.65f;
	}

	else if (_type == SPHERE_M)
	{
		m_pBufferCom = Add_Component<CSphereMesh>(L"Proto_SphereCom_M", L"Proto_SphereCom_M", ID_STATIC);
		CTransform* pTrans= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pTrans = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}
		_vec3 pPos = pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = pTrans->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&vLook, &vLook);
		pPos += vLook * 3.f;
		m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fCurTime = 0.f;
		m_fTime = 0.6f;
		m_fSpeed = 61.f;
	}

	else if (_type == GOLEM_SPIT)
	{
		m_pBufferCom = Add_Component<CSphereMesh>(L"Proto_SphereCom_M", L"Proto_SphereCom_M", ID_STATIC);
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		_vec3 pPos;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);
		m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);

		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fCurTime = 0.f;
		m_fTime = 0.6f;
		m_fSpeed = 61.f;
	}
	// m_pTransCom->Set_Pos(pPos.x + CGameUtilMgr::GetRandomFloat(-3.f, 3.f),
	// 	pPos.y + 0.5f, pPos.z + CGameUtilMgr::GetRandomFloat(-3.f, 3.f));
	IM_LOG("%f, %f", m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].z);
	IM_LOG("%f", m_pTransCom->m_vScale.x);

	m_pTransCom->Update_Component(0.f);

	return S_OK;
}

_int CSphereEffect::Update_Object(const _float& fTimeDelta)
{
	if (m_fCurTime>= m_fTime)
		return OBJ_DEAD;

	m_fCurTime += fTimeDelta;

	CGameObject::Update_Object(fTimeDelta);

	// m_pTransCom->m_vAngle.y += D3DXToRadian(40.f) * fTimeDelta;
	 m_pTransCom->m_vScale *= m_fSpeed * fTimeDelta;

	

	// m_pTransCom->m_vScale == 

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	
	// m_pTransCom->Set_Scale(CGameUtilMgr::GetRandomFloat(0.7f, 2.5f), CGameUtilMgr::GetRandomFloat(0.7f, 2.5f), CGameUtilMgr::GetRandomFloat(0.7f, 2.5f));

	// _float fY = CGameUtilMgr::GetRandomFloat(0.f, 2.f);
	// m_pTransCom->Rotation(ROT_Y, D3DXToRadian(fY * fTimeDelta));

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CSphereEffect::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CSphereEffect::Render_Object()
{
	m_pBufferCom->Render_Buffer();
}

void CSphereEffect::Free()
{
	CGameObject::Free();
}

CSphereEffect* CSphereEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, SPHERETYPE _type)
{
	CSphereEffect* Inst = new CSphereEffect(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size,_type)))
	{
		return nullptr;
	}
	return Inst;
}
