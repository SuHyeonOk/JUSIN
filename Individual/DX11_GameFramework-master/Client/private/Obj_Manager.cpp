#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CObj_Manager)

CObj_Manager::CObj_Manager()
{
}

HRESULT		CObj_Manager::Initialized()
{
	m_tPlayerInfo.iHp		= 100;
	m_tPlayerInfo.iHpMax	= 100;
	m_tPlayerInfo.iAttack	= 10;
	m_tPlayerInfo.iExp		= 0;
	m_tPlayerInfo.iExpMax	= 100;
	m_tPlayerInfo.iLevel	= 1;
	m_tPlayerInfo.iKey		= 0;
	m_tPlayerInfo.iHeart	= 0;
	m_tPlayerInfo.iCoin		= 0;

	m_tPlayerInfo.iKey = 0;
	m_tPlayerInfo.iHeart = 0;

	m_tPlayerInfo.ePlayer = PLAYERINFO::PLAYER::FINN;
	m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_ROOT;
	m_tPlayerInfo.eJakeWeapon = PLAYERINFO::JAKEWEAPON::LFIST;



	return S_OK;
}

void CObj_Manager::Tick_Player_Transform()
{
	Get_Player_Transform();
}

_vector			CObj_Manager::Get_Player_Transform()
{
	if (m_bNextLevel)
		return _vector();

	// 현재 Player 를 확인하고 그 Player 의 Transform 을 넘겨준다.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINN)
	{
		CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn 좌표 받아옴
		return vPlayerPos;
	}
	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake 좌표 받아옴
		return vPlayerPos;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _vector();	// 없다면 쓰레기 값을 넘김
}

//_bool	CObj_Manager::Get_Player_Collider(CCollider* pColliderCom[COLLTYPE_END])
//{
//	// 현재 Player 를 확인하고, 그 Player 의 Collider 를 넘겨준다.
//
//	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
//
//	if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINN)
//	{
//		CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Collider"), 0);
//		if (nullptr == pTargetCollider)
//			return false;
//
//		return pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider);
//	}
//	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
//	{
//		CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Collider"), 0);
//		if (nullptr == pTargetCollider)
//			return false;
//
//		return pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider);
//	}
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return false;	// 없다면 쓰레기 값을 넘김
//}

void		CObj_Manager::Set_Player_MinusHp(_int eHp)
{
	//if (PLAYERINFO::PLAYER::JAKE == m_tPlayerInfo.ePlayer &&
	//	PLAYERINFO::STATE::CONTROL == m_tPlayerInfo.eJakeWeapon)
	//	return;

	if (m_bShield)
	{
		m_bShield = false;
		return;
	}

	m_iMonster_Attck = eHp;
}

void		CObj_Manager::Tick(_double TimeDelta)
{
	Current_Player();			// 현재 플레이어가 누구인지                                     Tick
	Player_Exp();				// 플레이어 경험치를 계산하영 일정 경험치 보다 커지면 레벨업, 최대 경험치 증가, 공격력 증가
	Key_Input();				// 전체적인 키 입력

	//cout << "HP : " << m_tPlayerInfo.iHp << " | MAXHP : " << m_tPlayerInfo.iHpMax <<
	//	" | ATTACK : " << m_tPlayerInfo.iAttack << " | LEVEL : " << m_tPlayerInfo.iLevel << 
	//	" | EXP : " << m_tPlayerInfo.iExp << " | MAXEXP : " << m_tPlayerInfo.iExpMax << endl;

	if (0 < m_iMonster_Attck)
	{
		m_dPlayerAttck_TimeAcc += TimeDelta;
		if (0.7 < m_dPlayerAttck_TimeAcc)
		{
			if (0 < m_tPlayerInfo.iHp) 
				m_tPlayerInfo.iHp -= m_iMonster_Attck;
			
			m_iMonster_Attck = 0;
			m_dPlayerAttck_TimeAcc = 0;
		}
	}

}

void		CObj_Manager::Key_Input()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 전역적으로 네이게이션, 충돌체를 껐다가 켤 수 있다.
	if (pGameInstance->Key_Down(DIK_F11))
		g_bNavigationRender = !g_bNavigationRender;

	// 상호작용 키 움직이지 못함.
	if (pGameInstance->Key_Down(DIK_F))
		m_bInteraction = !m_bInteraction;

	// TODO : 무기변경
	if (pGameInstance->Key_Down(DIK_U))	// TODO : 1 Map 이 끝나면 변경
	{
		m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_DOLDEN;
	}
	if (pGameInstance->Key_Down(DIK_I))	// TODO : 중간 보스 잡으면 변경
	{
		m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_FAMILY;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_Z))
	{
		m_ChangeTarget++;
		Set_Current_Player_State(PLAYERINFO::CHANGE);
	}

	switch (m_ChangeTarget)
	{
	case PLAYERINFO::FINN:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FINN;
		break;

	case PLAYERINFO::JAKE:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.JAKE;
		break;

	case PLAYERINFO::FREE:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FREE; // ▣ 추 후 삭제 해야함
		break;

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // 즉, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Player_Exp()
{
	if (m_tPlayerInfo.iExp >= m_tPlayerInfo.iExpMax)
	{
		m_tPlayerInfo.iHp = m_tPlayerInfo.iHpMax;		// 체력 꽉 채워주기
		m_tPlayerInfo.iLevel++;							// 레벨 증가
		m_tPlayerInfo.iExp = 0;							// 경험치 0 으로 초기화
		m_tPlayerInfo.iExpMax += 50;					// 최대 경험치 증가
		m_tPlayerInfo.iAttack += 10;					// 공격력 증가

		return;
	}
}

_float		CObj_Manager::Get_Player_Distance(_fvector	_MyPos)
{
	_vector	vDistance = CObj_Manager::GetInstance()->Get_Player_Transform() - _MyPos;

	return XMVectorGetX(XMVector3Length(vDistance));
}

void		CObj_Manager::Free()
{
}
