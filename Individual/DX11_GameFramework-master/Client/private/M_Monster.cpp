#include "stdafx.h"
#include "..\public\M_Monster.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "Skill_Manager.h"
#include "ItemManager.h"
#include "Effect_Manager.h"
#include "E_DieCenter.h"
#include "UI_Manager.h"

CM_Monster::CM_Monster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CM_Monster::CM_Monster(const CM_Monster & rhs)
	: CGameObject(rhs)
{

}

HRESULT CM_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Monster::Initialize(void * pArg)
{
	MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MONSTERDESC));

	if (FAILED(__super::Initialize(&MonsterDesc)))
		return E_FAIL;
	
	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CM_Monster::Tick(const _double& TimeDelta)
{
	__super::Tick(TimeDelta);

	Hit_Process(TimeDelta);
}

void CM_Monster::Late_Tick(const _double& TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
	m_pColliderCom[COLLTYPE_AABB]->Update(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CM_Monster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CM_Monster::On_Collision(CGameObject * pOther)
{
	if (CObj_Manager::PLAYERINFO::STATE::ATTACK == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		if (L"Player_Weapon" == pOther->Get_Tag())
			m_bPlayer_Attack = true;
	}

	if(L"Skill_Paint" == pOther->Get_Tag())
		m_bPlayer_Attack = true;

	if (L"Skill_Marceline" == pOther->Get_Tag())
		m_tMonsterInfo.eState = m_tMonsterInfo.DANCE;
}

_bool CM_Monster::Random_Move(CTransform * pTransform, _float4 f4CenterPos, _double TimeDelta, _float fRange)
{
	if (!m_bRandomTime)
	{
		m_bRandomTime = true;
		m_bRandom_TimeAcc = _double(CUtilities_Manager::GetInstance()->Get_Random(1.5f, 3.0f));
	}

	// 이동 시킬 시간을 담는다.
	m_bRandomMove_TimeAcc += TimeDelta;
	if (m_bRandom_TimeAcc < m_bRandomMove_TimeAcc)
	{
		m_bRandomMove_TimeAcc = 0;
		m_bRandomTime = false;
		return false;
	}

	_vector	vMyPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vCenterPos = XMLoadFloat4(&f4CenterPos);
	_vector vDistance = vCenterPos - vMyPos;	// 내 원점 - 현재 위치
	_float	fDiatance = XMVectorGetX(XMVector3Length(vDistance));

	pTransform->Go_Straight(TimeDelta);

	if (fRange < fDiatance)	// 일정 범위를 나가면
	{
		pTransform->Chase(vCenterPos, TimeDelta);	// 원점으로 돌아가고
		_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
		pTransform->Rotation(pTransform->Get_State(CTransform::STATE_UP), fRandomAxis);	// Look 을 변경한다.
	}

	return true;
}

_bool CM_Monster::RandomMove(CTransform* pTransform, _float4 f4FirstPos, _float fRange, _double TimeDelta, _float fStart, _float fEnd)
{
	if (!m_bRandomPos)
	{
		// 랜덤한 좌표를 구한다.
		_float fRandom_1 = CUtilities_Manager::GetInstance()->Get_Random(fStart, fEnd);
		_float fRandom_2 = CUtilities_Manager::GetInstance()->Get_Random(fStart, fEnd);

		_vector vMyPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		_int iRandom = CUtilities_Manager::GetInstance()->Get_Random(0, 7);
		if (0 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x + fRandom_1), f4MyPos.y, (f4MyPos.z + fRandom_2), 1.f);
		else if (1 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z - fRandom_1), 1.f);
		else if (2 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x + fRandom_1), f4MyPos.y, (f4MyPos.z - fRandom_2), 1.f);
		else if (3 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z + fRandom_1), 1.f);
		else if (4 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z - fRandom_1), 1.f);
		else if (5 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x + fRandom_1), f4MyPos.y, (f4MyPos.z - fRandom_2), 1.f);
		else if (6 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z + fRandom_1), 1.f);
		else
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_1), f4MyPos.y, (f4MyPos.z + fRandom_2), 1.f);

		_vector vRandomPos, vFirstPos;
		vRandomPos = XMLoadFloat4(&m_f4RandomPos);
		vFirstPos = XMLoadFloat4(&f4FirstPos);
		_vector vCheckDistance = vRandomPos - vFirstPos;
		_float fCheckDistance = XMVectorGetX(XMVector3Length(vCheckDistance));

		m_bRandomPos = true;
	}

	// 랜덤한 좌표를 받은 곳 으로 이동한다.
	_vector vRandomPos;
	vRandomPos = XMLoadFloat4(&m_f4RandomPos);

	pTransform->LookAt(vRandomPos);
	pTransform->Chase(vRandomPos, TimeDelta);

	// 랜덤 좌표와 현재 좌표가 가까워지면...
	_vector vMyPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = vRandomPos - vMyPos;
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	if (0.1f > fDistance)
	{
		m_bRandomPos = false;
		return true;
	}
	else
		return false;
}

void CM_Monster::Die(const _double & TimeDelta, _float fPlusY, _uint iBronzeCount, _uint iSilverCount, _uint iGoldCount)
{
	// 몬스터가 죽고 나면 할 행동

	if (0.0f >= m_fAlpha)
	{
		// 몬스터 죽으면 UI 초기화
		CUI_Manager::GetInstance()->Set_HPGauge_Monster(1.0f);
		CObj_Manager::GetInstance()->Set_Monster_Crash(false);
		//CUI_Manager::GetInstance()->Set_Ui_Monster(false);

		// 알파값이 다 사라지면 죽음
		CGameObject::Set_Dead();	
	}

	if (0.0f < m_fAlpha)															// 알파값 줄어 들도록
		m_fAlpha -= _float(TimeDelta);

	if (5 != m_iDieEffect_Count)													// 이펙트 5개
	{
		CUI_Manager::GetInstance()->Set_HPGauge_Monster(m_tMonsterInfo.fHP / m_tMonsterInfo.fMaxHP);

		++m_iDieEffect_Count;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CE_DieCenter::DIECENTERINFO tDieCenterInfo;

		if(MONSTERDESC::MONSTERKIND::W_BEE == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::W_WORKE == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::GHOST_1 == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::GHOST_2 == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::GHOST_3 == m_tMonsterDesc.eMonsterKind)
  			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::SKY;
		else if (MONSTERDESC::MONSTERKIND::S_COWBOY == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::S_SR == m_tMonsterDesc.eMonsterKind)
			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::BLUE;
		else if (MONSTERDESC::MONSTERKIND::G_BLUE == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::G_RED == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::G_YELLOW == m_tMonsterDesc.eMonsterKind)
			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::YELLOW;
		else if (MONSTERDESC::MONSTERKIND::TREE_WITCH == m_tMonsterDesc.eMonsterKind)
			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::BROWN;
		else if (MONSTERDESC::MONSTERKIND::MAGIC_MAN == m_tMonsterDesc.eMonsterKind)
			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::PURPLE;
		else if (MONSTERDESC::MONSTERKIND::MIMIC == m_tMonsterDesc.eMonsterKind)
			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::RED;
		else if (MONSTERDESC::MONSTERKIND::SKELETON_SHIELD_1 == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::SKELETON_SHIELD_2 == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::SKELETON_ARCHER_1 == m_tMonsterDesc.eMonsterKind ||
			MONSTERDESC::MONSTERKIND::SKELETON_ARCHER_2 == m_tMonsterDesc.eMonsterKind)
			tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::GRAY;

  		tDieCenterInfo.f3Pos = _float3(vf4MyPos.x, vf4MyPos.y + fPlusY, vf4MyPos.z - 0.7f);
		CEffect_Manager::GetInstance()->DieCenter_Create(tDieCenterInfo);
	}

	if (!m_OneCoin)															// 한 번만
	{
		// 플레이어 UI 관리
		CObj_Manager::GetInstance()->Set_Player_PlusExp(m_tMonsterInfo.fExp);
		CUI_Manager::GetInstance()->Set_LevelGauge_Player(CObj_Manager::GetInstance()->Get_Current_Player().fExp / CObj_Manager::GetInstance()->Get_Current_Player().fExpMax);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), iBronzeCount, iSilverCount, iGoldCount); 	// 동전 생성

		m_OneCoin = true;
	}
}

void CM_Monster::Dance_Time()
{
	if (CSkill_Manager::PLAYERSKILL::MARCELINT != CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
}

void CM_Monster::Effect_Hit(_float3 fPulsPos)
{
	if (!m_bShader_Hit)
		return;

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CEffect_Manager::GetInstance()->Effect_Hit_Create(_float3(f4MyPos.x + fPulsPos.x, f4MyPos.y + fPulsPos.y, f4MyPos.z + fPulsPos.z));
}

void CM_Monster::Hit_Process(const _double & TimeDelta)
{
	if (!m_bPlayer_Attack)
		return;

	// 몬스터 공격 받는 중...

	// 몬스터 Hit 셰이더 흰색 깜박!
	m_bShader_Hit = true;

	if (0.1 < m_dPlayer_Attack_TimeAcc)
		m_bShader_Hit = false;

	// 몬스터 상태 변경
	m_tMonsterInfo.eState = m_tMonsterInfo.HIT;

	// UI 에 내 체력 넘겨주기
	CUI_Manager::GetInstance()->Set_HPGauge_Monster(m_tMonsterInfo.fHP / m_tMonsterInfo.fMaxHP);

	m_dPlayer_Attack_TimeAcc += TimeDelta;
	if (0.5 < m_dPlayer_Attack_TimeAcc)
	{
		m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.05f);

		// 플레이어의 공격력 으로 몬스터 체력 깍기
		m_tMonsterInfo.fHP -= CObj_Manager::GetInstance()->Get_Player_Attack();

		m_bPlayer_Attack = false;
		m_dPlayer_Attack_TimeAcc = 0;
		return;
	}
}

void CM_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom[COLLTYPE_AABB]);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
