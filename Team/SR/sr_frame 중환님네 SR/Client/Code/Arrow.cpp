#include "stdafx.h"
#include "..\Header\Arrow.h"

#include "AbstFactory.h"
#include "ArrowCubeMgr.h"
#include "Monster.h"
#include "Particle.h"
#include "StatComponent.h"

CArrow::CArrow(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev)
{
}

CArrow::CArrow(const CArrow& rhs): CGameObject(rhs)
{
}

CArrow::~CArrow()
{
}

HRESULT CArrow::Ready_Object()
{
	CGameObject::Ready_Object();

	m_pTransform = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pCollCom = Add_Component<CCollisionCom>(L"Proto_CollisionCom", L"Proto_CollisionCom", ID_DYNAMIC);
	m_pCollCom->SetOwner(this);
	m_pCollCom->SetOwnerTransform(m_pTransform);
	m_pCollCom->SetRadius(0.7f);
	m_pCollCom->SetCollType(m_eCollType);
	m_pCollCom->SetCallBack(
		[this](CCollisionCom* pOther)
		{
			DynamicCallBack(pOther);
		},
		[this](_vec3 vCenter, _float fRadius)
		{
			StaticCallBack(vCenter, fRadius);
		});

	m_fpSpeed = 20.f;
	m_fLife = 3.f;
	return S_OK;
}

_int CArrow::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);
	DEBUG_SPHERE(m_pCollCom->GetCollPos(), m_pCollCom->GetRadius(), 0.1f);

	m_fLife -= fTimeDelta;
	if (m_fLife <= 0.f) return OBJ_DEAD;

	switch (m_eMoveType)
	{
	case ARROW_MOVE_NORMAL:
		m_pTransform->m_vInfo[INFO_POS] += m_pTransform->m_vInfo[INFO_LOOK] * m_fpSpeed * fTimeDelta;
		break;
	case ARROW_MOVE_DANGLING:
		{
			if (m_pParentStat->IsDead())
				return OBJ_DEAD;
			_matrix matParentWorld = m_pParentTrans->m_matWorld;
			CGameUtilMgr::RemoveScale(matParentWorld);
			m_pTransform->m_matWorld = m_matDanglingLocal * matParentWorld;
		}
		break;
	case ARROW_MOVE_STUCK:
		if (m_fStuckTime > 0.f)
		{
			if (m_bWaveDir)
				m_pTransform->m_vAngle.x += D3DXToRadian(150.f) * fTimeDelta;
			else
				m_pTransform->m_vAngle.x += -D3DXToRadian(150.f) * fTimeDelta;

			m_bCurWaveTime -= fTimeDelta;
			if (m_bCurWaveTime < 0.f)
			{
				m_bWaveDir = !m_bWaveDir;
				m_bCurWaveTime = 0.1f;
			}

			m_fStuckTime -= fTimeDelta;
			if (m_fStuckTime <= 0.f) m_pTransform->Set_StopUpdate();
		}
		break;
	case ARROW_MOVE_END:
		break;
	default:
		break;
	}

	// 렌더는 인스턴싱으로 ArrowCubeMgr에서 일괄적으로 진행
	CArrowCubeMgr::GetInst().Add_Arrow(m_pTransform);
	return OBJ_NOEVENT;
}



CArrow* CArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, ArrowParams tArrowParams)
{
	CArrow* pInstance = new CArrow(pGraphicDev);
	pInstance->m_fDamage = tArrowParams.fDamage;
	pInstance->m_eType = tArrowParams.eType;
	pInstance->m_eCollType = tArrowParams.eCollType;
	pInstance->m_bCritical = tArrowParams.bCiritical;

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CArrow::Free()
{
	Safe_Release(m_pParentTrans);
	Safe_Release(m_pParentStat);
	CGameObject::Free();
}

void CArrow::FireWork()
{
	m_fLife = 0.f;
	CSoundMgr::GetInstance()->PlaySoundChannel(L"sfx_item_fireworksTwinkle-002_soundWave.ogg", m_pTransform->m_vInfo[INFO_POS], SOUND_UI);

	// firework particle
	_float fColor = CGameUtilMgr::GetRandomFloat(0.0f, 1.0f);
	_float fBlend = CGameUtilMgr::GetRandomFloat(0.0f, 0.5f);
	ULONGLONG tick = GetTickCount64();

	if (tick % 3 == 0)
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(
			m_pTransform->m_vInfo[INFO_POS], 0.5f, D3DXCOLOR(fColor, fBlend, 0, 0), 256, 0.5f);
	else if (tick % 3 == 1)
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(
			m_pTransform->m_vInfo[INFO_POS], 0.5f, D3DXCOLOR(0, fColor, fBlend, 0), 256, 0.5f);
	else if (tick % 3 == 2)
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(
			m_pTransform->m_vInfo[INFO_POS], 0.5f, D3DXCOLOR(fBlend, 0, fColor, 0), 256, 0.5f);
	else
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(
			m_pTransform->m_vInfo[INFO_POS], 0.5f, D3DXCOLOR(fColor, fBlend, fBlend, fColor), 256, 0.5f);

	CEffectFactory::Create<CUVCircle>("Firwork_Circle", L"Firwork_Circle", m_pTransform->m_vInfo[INFO_POS]);
	// firework particle

	set<CGameObject*> setObj;
	Engine::GetOverlappedObject(setObj, m_pTransform->m_vInfo[INFO_POS], 5.f);
	for (auto& obj : setObj)
	{
		if (CMonster* pMonster = dynamic_cast<CMonster*>(obj))
		{
			pMonster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
			        ->TakeDamage((_int)m_fDamage, m_pTransform->m_vInfo[INFO_POS], m_pOwner);
			DEBUG_SPHERE(m_pTransform->m_vInfo[INFO_POS], 5.f, 1.f);
		}
	}
}

void CArrow::DynamicCallBack(CCollisionCom* pOther)
{
	m_pCollCom->SetStop();
	if (m_eType == ARROW_FIREWORK && m_fLife > 0.f)
	{
		FireWork();
	}
	else if (m_eType == ARROW_NORMAL)
	{
		if (pOther->GetOwner()->Has_Component(L"Proto_StatCom", ID_DYNAMIC))
		{
			m_eMoveType = ARROW_MOVE_DANGLING;

			m_pParentStat = pOther->GetOwner()->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC);
			m_pParentStat->AddRef();
			m_pParentStat->TakeDamage((_int)m_fDamage, m_pTransform->m_vInfo[INFO_POS], m_pOwner, DT_END, m_bCritical);

			m_pParentTrans = pOther->GetTransform();
			m_pParentTrans->AddRef();
			m_pTransform->Set_StopUpdate();

			_vec3 vLocalAngle = m_pTransform->m_vAngle;
			vLocalAngle.y += m_pParentTrans->m_vAngle.y;

			CGameUtilMgr::MatWorldComposeEuler(m_matDanglingLocal, m_pTransform->m_vScale, vLocalAngle, {0.f, 1.5f, 0.f});
			Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pTransform->m_vInfo[INFO_POS], 0.3f, YELLOW, 4, 0.2f);
			CSoundMgr::GetInstance()->PlaySoundRandom({L"sfx_item_arrowChargedImpact-001_soundWave.ogg", L"sfx_item_arrowChargedImpact-002_soundWave.ogg", L"sfx_item_arrowChargedImpact-003_soundWave.ogg"}, m_pTransform->m_vInfo[INFO_POS], 0.35f);
		}
		else
		{
			m_fLife = 0.f;
		}
	}
}

void CArrow::StaticCallBack(_vec3 vCenter, _float fRadius)
{
	m_eMoveType = ARROW_MOVE_END;
	m_pCollCom->SetStop();
	if (m_eType == ARROW_FIREWORK && m_fLife > 0.f)
	{
		FireWork();
	}
	else if (m_fLife > 0.f)
	{
		m_eMoveType = ARROW_MOVE_STUCK;
		Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")
			->Add_Particle(m_pTransform->m_vInfo[INFO_POS], 0.3f, YELLOW, 4, 0.2f);
		CSoundMgr::GetInstance()->PlaySoundRandom({L"sfx_item_arrowChargedImpact-001_soundWave.ogg", L"sfx_item_arrowChargedImpact-002_soundWave.ogg", L"sfx_item_arrowChargedImpact-003_soundWave.ogg"}, m_pTransform->m_vInfo[INFO_POS], 0.35f);
	}
}
