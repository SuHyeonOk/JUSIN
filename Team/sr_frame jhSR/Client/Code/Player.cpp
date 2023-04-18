#include "stdafx.h"
#include "Player.h"
#include "Controller.h"
#include "GameUtilMgr.h"
#include "StatComponent.h"
#include "TerrainCubeMap.h"
#include "AbstFactory.h"
#include "Particle.h"
#include "StaticCamera.h"
#include "Monster.h"
#include "PlayerController.h"
#include "Crossbow.h"
#include "Sword.h"
#include "Glaive.h"
#include "Axe.h"
#include "SphereEffect.h"
#include "Inventory.h"
#include "Emerald.h"
#include "PlayerStartPos.h"
#include "ServerPacketHandler.h"
#include "TerrainCubeMap.h"
#include "LaserShotRune.h"
#include "ObjectStoreMgr.h"
#include "Arrow.h"

/*-----------------------
 *    CCharacter
 ----------------------*/

const _float CPlayer::s_PotionCollTime = 20.f;
const _float CPlayer::s_RollCoolTime = 3.f;
_bool CPlayer::s_bDropDead = false;

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev) : CSkeletalCube(pGraphicDev)
{
	m_fSpeed = 4.5f;
	m_fRollSpeed = 12.f;
}


CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_Object(const wstring& wstrPath)
{
	CSkeletalCube::Ready_Object();
	if (wstrPath.empty() == false)
		LoadSkeletal(wstrPath);

	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;

	if (m_bRemote)
	{
		CController* pController = Add_Component<CPlayerController>(L"Proto_PlayerRemoteController", L"Proto_PlayerRemoteController", ID_DYNAMIC);
		pController->SetOwner(this);
	}
	else
	{
		CController* pController = Add_Component<CPlayerController>(L"Proto_PlayerController", L"Proto_PlayerController", ID_DYNAMIC);
		pController->SetOwner(this);
	}


	m_pColl = Add_Component<CCollisionCom>(L"Proto_CollisionCom", L"Proto_CollisionCom", ID_DYNAMIC);
	m_pColl->SetOwner(this);
	m_pColl->SetOwnerTransform(m_pRootPart->pTrans);
	m_pColl->SetCollOffset(_vec3{0.f, 1.5f, 0.f});
	m_pColl->SetRadius(0.5f);
	m_pColl->SetCollType(COLL_PLAYER);

	m_CurRollCoolTime = 3.f;	
	m_CurPotionCoolTime = 20.f;

	m_pStat = Add_Component<CStatComponent>(L"Proto_StatCom", L"Proto_StatCom", ID_DYNAMIC);
	m_pStat->SetMaxHP(150);
	m_pStat->SetTransform(m_pRootPart->pTrans);
	m_pStat->SetOwner(this);
	m_pStat->SetHurtSound({
		L"DLC_sfx_mob_whisperer_hit_1.ogg",
		L"DLC_sfx_mob_whisperer_hit_2.ogg",
		L"DLC_sfx_mob_whisperer_hit_3.ogg",
		L"DLC_sfx_mob_whisperer_hit_4.ogg",
		L"DLC_sfx_mob_whisperer_hit_5.ogg",
		L"DLC_sfx_mob_whisperer_hit_6.ogg" });

	// 항상 카메라 먼저 만들고 플레이어 만들기!
	if (m_bRemote == false)
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")->SetTarget(this);

	m_dwWalkDust = GetTickCount();
	m_dwRollDust = GetTickCount();

	if (m_bRemote)
	{
		m_pInventory = CObjectFactory::Create<CInventory>("RemoteInventory", L"RemoteInventory");
		m_pInventory->AddRef();
		m_pInventory->SetOwner(this);
		m_pInventory->AddDefaultItems();
		m_pInventory->SetArrow(100);
		m_arrAnim = m_pInventory->CurWeapon(IT_MELEE)->SetarrAnim();
	}
	else
	{
		// if (CObjectStoreMgr::GetInstance()->GetInventory())
		// {
		// 	m_pInventory = CObjectStoreMgr::GetInstance()->GetInventory();
		// 	m_pInventory->SetArrow(50);
		// 	m_pInventory->AddItemToLayer();
		// 	m_pInventory->AddRef();
		// 	m_pInventory->SetOwner(this);
		// 	Get_Layer(LAYER_GAMEOBJ)->Add_GameObject(L"Inventory", m_pInventory);
		// }
		// else
		// {
		// 	m_pInventory = CObjectFactory::Create<CInventory>("Inventory", L"Inventory");
		// 	CObjectStoreMgr::GetInstance()->StoreInventory(m_pInventory);
		// 	m_pInventory->AddRef();
		// 	m_pInventory->SetOwner(this);
		// 	m_pInventory->AddDefaultItems();
		// }
		m_pInventory = CObjectFactory::Create<CInventory>("Inventory", L"Inventory");
		// CObjectStoreMgr::GetInstance()->StoreInventory(m_pInventory);
		m_pInventory->AddRef();
		m_pInventory->SetOwner(this);
		m_pInventory->AddDefaultItems();

		m_arrAnim = m_pInventory->CurWeapon(IT_MELEE)->SetarrAnim();
		if (g_bOnline)
			m_pInventory->RefreshInventory();
	}


	m_pRootPart->pTrans->Update_Component(0.f);
	//test
	// PlayerSpawn();

	//큐브의 인덱스를 받아옴
	CTerrainCubeMap* pTerrainCube = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	arrBlock = pTerrainCube->GetBlockIndex();
	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);

		
	DEBUG_SPHERE(m_pColl->GetCollPos(), m_pColl->GetRadius(), 0.1f);

	if (m_pStat->IsSatonFascinate())
		m_strStatus = "Fascinate";
	else
		m_strStatus = "Nothing";

		if (m_pCurAnim == m_pIdleAnim) // 이전 애니메이션 종료
			m_bCanPlayAnim = true;

		if (s_RollCoolTime > m_CurRollCoolTime)
			m_CurRollCoolTime += fTimeDelta;
		if (s_PotionCollTime > m_CurPotionCoolTime)
			m_CurPotionCoolTime += fTimeDelta;




	// 상태 변경 조건 설정
	StateChange();
	m_bRoll = false;

	// 각 상태에 따른 프레임 마다 실행할 함수 지정
	switch (m_eState)
	{
	case IDLE:
		break;
	case WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_vMoveDirNormal * m_fSpeed * fTimeDelta;
		break;
	case ATTACK:
		AttackState(); // 근접, 원거리 분기하기
		break;
	case STUN:
		break;
	case ROLL:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fRollSpeed * fTimeDelta;
		if (m_dwRollDust + 300 < GetTickCount())
		{
			CEffectFactory::Create<CCloud>("Roll_Cloud", L"Roll_Cloud", m_pRootPart->pTrans->m_vInfo[INFO_POS] + _vec3{0.f, 0.5f, 0.f});
			m_dwRollDust = GetTickCount();
		}
		break;
	case LEGACY:
		break;
	case FASCINATE:
		RotationToSaton();
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_vTargetPos * m_fSpeed * fTimeDelta;
		break;
	case DEAD:
		break;
	default:
		break;
	}

	

	return OBJ_NOEVENT;
}

void CPlayer::LateUpdate_Object()
{
	
	if (m_bApplyMeleeAttackNext)
	{
		m_pInventory->CurWeapon(IT_MELEE)->Collision();
		m_bApplyMeleeAttackNext = false;
	}

	if (m_bApplyMeleeAttack)
	{
		// RotateToCursor()로 회전하고, 이 회전값이 INFO_LOOK로 적용되려면 다음 update를 기다려야한다.
		// attack이 발생하고 다음 프레임에서 실제 공격을 적용하기 위한 코드
		m_bApplyMeleeAttack = false;
		m_bApplyMeleeAttackNext = true;
	}

	if (m_eState != DEAD && s_bDropDead && m_pRootPart->pTrans->m_vInfo[INFO_POS].y < 15.f)
	{
		m_pStat->SetDead();
	}
	
}

void CPlayer::Render_Object()
{
	if (m_bVisible)
	{
		_matrix matView, matProj;
		D3DVIEWPORT9 viewport;
		ZeroMemory(&viewport, sizeof(D3DVIEWPORT9));
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		m_pGraphicDev->GetViewport(&viewport);

		_vec2 vScreen;
		CGameUtilMgr::World2Screen(vScreen, m_pColl->GetCollPos() + _vec3{0.f, 2.3f, 0.f}, matView, matProj, viewport);
		wstring tmp(m_strName.begin(), m_strName.end());
#ifdef _DEBUG
		// tmp = tmp + to_wstring(m_iID);
#endif
		_float fHalf = 10.f * _float(tmp.size()) / 2;
		vScreen.x -= fHalf;

		Engine::Render_Font(L"Gothic_Bold20", tmp.c_str(), &vScreen, D3DCOLOR_ARGB(255, 255, 255, 255));


		CSkeletalCube::Render_Object();
	}
}

void CPlayer::Free()
{
	Safe_Release(m_pInventory);
	CSkeletalCube::Free();
}

void CPlayer::AnimationEvent(const string& strEvent)
{
	if (strEvent == "ActionEnd")
	{
		m_bCanPlayAnim = true;
	}
	else if (strEvent == "MeleeAttackFire")
	{
		m_bApplyMeleeAttack = true;

		// axe crack
		if (m_iAttackCnt == 0 && dynamic_cast<CAxe*>(m_pInventory->CurWeapon(IT_MELEE)))
		{
			Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
				->PlayShake(0.15f, 0.4f);
			_vec3 vDecalPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 3.f;
			vDecalPos.y = 0.5f + Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap")->GetHeight(vDecalPos.x, vDecalPos.z);

			CEffectFactory::Create<CCrack>("Exe_Decal", L"Exe_Decal", vDecalPos);
			for (int i = 0; i < 5; i++)
			{
				CEffectFactory::Create<CCloud>("Decal_Cloud", L"Decal_Cloud", vDecalPos);
			}
		}
		// axe crack
	}
	else if (strEvent == "Glaive_Attack")
	{
		m_bApplyMeleeAttackNext = true;
	}
	else if (strEvent == "step")
	{
		if ((_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].x <= 0 || (_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].z <= 0 ||
			(_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].x > 129 || (_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].z > 129)
			return;

		if (arrBlock[(_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].x][(_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].z] == 13)
		{
			CSoundMgr::GetInstance()->PlaySoundRandom({ L"sfx_player_stepGrass-001.ogg", L"sfx_player_stepGrass-002.ogg" ,L"sfx_player_stepGrass-003.ogg" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		}
		else if (arrBlock[(_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].x][(_int)m_pRootPart->pTrans->m_vInfo[INFO_POS].z] == 15)
		{
			CSoundMgr::GetInstance()->PlaySound(L"DLC_Ice_SluiceGate_Water_LOOP.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS]);

		}
		else
			CSoundMgr::GetInstance()->PlaySoundRandom({ L"sfx_player_stepStone-001.ogg", L"sfx_player_stepStone-002.ogg" ,L"sfx_player_stepStone-003.ogg" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);

		if (m_dwWalkDust + 500 < GetTickCount())
		{
			CEffectFactory::Create<CCloud>("Walk_Cloud", L"Walk_Cloud", m_pRootPart->pTrans->m_vInfo[INFO_POS] + _vec3{0.f, 0.5f, 0.f});
			m_dwWalkDust = GetTickCount();
		}
	}
	else if (strEvent == "AnimStopped")
	{
		SetVisible(false);
	}
	else if (strEvent == "landing")
	{
		CSoundMgr::GetInstance()->PlaySound(L"sfx_player_landing.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		for (int j = 0; j < 15; j++)
			CEffectFactory::Create<CCloud>("ShockPowder_Cloud", L"ShockPowder_Cloud", m_pRootPart->pTrans->m_vInfo[INFO_POS] + _vec3{0.f, 0.5f, 0.f});
	}
	else if (strEvent == "visible")
	{
		SetVisible(true);
	}
	else if (strEvent == "RangeFire")
	{
		PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK3]);
	}
}

void CPlayer::PlayerSpawn()
{
	static CubeAnimFrame landing = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/landing.anim");

	for (auto& obj :Get_Layer(LAYER_GAMEOBJ)->Get_MapObject())
	{
		if (CPlayerStartPos* pStartPos = dynamic_cast<CPlayerStartPos*>(obj.second))
		{
			if (g_bOnline)
			{
				if (pStartPos->GetID() == m_iID)
				{
					m_pRootPart->pTrans->Set_WorldDecompose(pStartPos->GetWorld());
					break;
				}
			}
			else
			{
				m_pRootPart->pTrans->Set_WorldDecompose(pStartPos->GetWorld());
				break;
			}
		}
	}

	// todo 부활 사운드로 죽음 사운드 덮기
	Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")->ResetPosition();
	m_pColl->SetStart();
	m_pStat->Revive();
	m_bReserveStop = false;
	m_bStopAnim = false;
	m_bCanPlayAnim = false;
	PlayAnimationOnce(&landing);
	SetVisible(true);
}


void CPlayer::SpawnArrow(_uint iDamage, PlayerArrowType eType, _bool bCritical, ArrowType eArrowType)
{
	_vec3 vLookAt;
	const _vec3 vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + _vec3{0.f, 1.3f, 0.f};

	if (m_bRemote == false)
	{
		if (PickTargetEnemy(OUT vLookAt) == false)
			vLookAt = vPos + m_pRootPart->pTrans->m_vInfo[INFO_LOOK];

		// pkt
		if (g_bOnline)
		{
			Protocol::C_PLAYER_ARROW arrowActionPkt;
			arrowActionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
			arrowActionPkt.mutable_player()->set_name("Player_test");
			arrowActionPkt.set_yaw(GetYawToCursor());
			arrowActionPkt.mutable_vlookat()->set_x(vLookAt.x);
			arrowActionPkt.mutable_vlookat()->set_y(vLookAt.y);
			arrowActionPkt.mutable_vlookat()->set_z(vLookAt.z);
			arrowActionPkt.set_actionbit(PLAYER_MR);
		
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(arrowActionPkt));
		}

		switch (eType)
		{
		case PlayerArrowType::NORMAL:
			CSoundMgr::GetInstance()->PlaySound(L"sfx_item_arrow_fire.ogg", vPos);
			CBulletFactory::Create<CArrow>("PlayerNormalArrow", L"PlayerNormalArrow", 
			{_float(iDamage), bCritical, COLL_PLAYER_BULLET, eArrowType},
				vPos, vLookAt)->m_pOwner = this;
			break;
		case PlayerArrowType::MULTISHOT:
			{
				CSoundMgr::GetInstance()->PlaySound(L"sfx_item_arrow_fire.ogg", vPos);

				_matrix matRot, matRotReverse;
				D3DXMatrixRotationY(&matRotReverse, D3DXToRadian(-15.f));
				D3DXMatrixRotationY(&matRot, D3DXToRadian(5.f));

				_vec3 vLook = vLookAt - vPos;
				D3DXVec3TransformNormal(&vLook, &vLook, &matRotReverse);

				for (int i = 0; i < 5; ++i)
				{
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					CBulletFactory::Create<CArrow>("PlayerNormalArrow", L"PlayerNormalArrow", 
					{_float(iDamage), bCritical, COLL_PLAYER_BULLET, eArrowType},
						vPos, vLook + vPos)->m_pOwner = this;
				}
			}
			break;
		case PlayerArrowType::LASER:
			m_bLaser = true;
			m_fCurLaserTime = 0.f;
			break;
		default:;
		}
	}
	else
	{
		vLookAt = m_vArrowLookAt;

		switch (eType)
		{
		case PlayerArrowType::NORMAL:
			CSoundMgr::GetInstance()->PlaySound(L"sfx_item_arrow_fire.ogg", vPos);
			CBulletFactory::Create<CArrow>("PlayerNormalArrow", L"PlayerNormalArrow", 
			{_float(iDamage), bCritical, COLL_PLAYER_BULLET, eArrowType},
				vPos, vLookAt)->m_pOwner = this;
			
			break;
		case PlayerArrowType::MULTISHOT:
			{
				CSoundMgr::GetInstance()->PlaySound(L"sfx_item_arrow_fire.ogg", vPos);

				_matrix matRot, matRotReverse;
				D3DXMatrixRotationY(&matRotReverse, D3DXToRadian(-15.f));
				D3DXMatrixRotationY(&matRot, D3DXToRadian(5.f));

				_vec3 vLook = vLookAt - vPos;
				D3DXVec3TransformNormal(&vLook, &vLook, &matRotReverse);

				for (int i = 0; i < 5; ++i)
				{
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					CBulletFactory::Create<CArrow>("PlayerNormalArrow", L"PlayerNormalArrow", 
					{_float(iDamage), bCritical, COLL_PLAYER_BULLET, eArrowType},
						vPos, vLook + vPos)->m_pOwner = this;
				}
			}
			break;
		case PlayerArrowType::LASER:
			m_bLaser = true;
			m_fCurLaserTime = 0.f;
			break;
		default:;
		}
	}
}

_bool CPlayer::PickTargetEnemy(_vec3& vLookAt)
{
	// https://gohen.tistory.com/79 참조(광선과 직선 교차판정)

	_vec3 vOrigin, vRayDir;
	_matrix matView, matProj;
	D3DVIEWPORT9 ViewPort;

	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	CGameUtilMgr::GetPickingRay(vOrigin, vRayDir, g_hWnd, matView, matProj, ViewPort);

	for (const auto& enemy : Engine::Get_Layer(LAYER_ENEMY)->Get_MapObject())
	{
		const auto pColl = enemy.second->Get_Component<CCollisionCom>(L"Proto_CollisionCom", ID_DYNAMIC);
		const _vec3 vSubject = vOrigin - pColl->GetCollPos();
		const _float fB = D3DXVec3Dot(&vRayDir, &vSubject);
		const _float fC = D3DXVec3Dot(&vSubject, &vSubject) - pColl->GetRadius();
		if (fB * fB  - fC >= 0.f)
		{
			vLookAt = pColl->GetCollPos();
			return true;
		}
	}
	return false;
}

void CPlayer::SetMoveDir(_float fX, _float fZ)
{
	m_vMoveDirNormal.x = fX;
	m_vMoveDirNormal.z = fZ;

	// x, z 중 하나가 0 이 아니면 움직임 요청
	m_bMove = CGameUtilMgr::FloatCmp(fX, 0.f) == false || CGameUtilMgr::FloatCmp(fZ, 0.f) == false;
}

void CPlayer::AttackState()
{
	if (m_bCanPlayAnim == false) return;

	if (m_bMeleeAttack)
	{
		m_bCanPlayAnim = false;
		m_iAttackCnt = m_pInventory->CurWeapon(IT_MELEE)->Attack();// 애니메이션 실행
	}
	else if (m_bRangeAttack)
	{

		m_bCanPlayAnim = false;
		m_iAttackCnt = m_pInventory->CurWeapon(IT_RANGE)->Attack();
		if (m_bRemote == false)
		{
			m_pInventory->UseArrow(1);
		}
	}

#pragma region Loading Box 
 	 //CEffectFactory::Create<CCrack>("LoadingBox", L"LoadingBox");
#pragma endregion


#pragma region Attack_Basic
	// Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.3f, RED, 4, 0.2f);
#pragma endregion

}



void CPlayer::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			if (m_bRemote == false)
				CSoundMgr::GetInstance()->PlaySoundChannel(L"sfx_playerDead-001_soundWave.mp3", m_pRootPart->pTrans->m_vInfo[INFO_POS], SOUND_UI);
				// CSoundMgr::GetInstance()->PlaySoundChannel(L"sfx_playerDead-001_soundWave.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS], SOUND_UI);
			m_eState = DEAD;
			PlayAnimationOnce(&m_arrAnim[ANIM_DEAD], true);
			m_bRoll = false;
			m_bCanPlayAnim = false;
			m_bMove = false;
			m_bLaser = false;
			m_bCanPlayAnim = false;
			m_pColl->SetStop();

			if (g_bOnline && m_bRemote == false)
			{
				Protocol::C_PLAYER_DEAD deadPkt;
				deadPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(deadPkt));
			}
		}
		return;
	}

	if (m_pStat->IsStun())
	{
		m_eState = STUN;
		m_bRoll = false;
		m_fCurLaserTime = 3.f;
		return;
	}

	if (m_pStat->IsSatonFascinate())
	{
		m_strStatus = "Fascinate";
		m_eState = FASCINATE;
		m_bRoll = false;
		m_pIdleAnim = &m_arrAnim[ANIM_WALK];
		m_pCurAnim = &m_arrAnim[ANIM_WALK];
		return;
	}

	if (m_bRoll && s_RollCoolTime <= m_CurRollCoolTime)
	{
		CSoundMgr::GetInstance()->PlaySound(L"sfx_player_stepCloth-003.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		m_eState = ROLL;
		if (m_bRemote == false)
		{
			RotateToCursor();
			m_CurRollCoolTime = 0.f;

			if (g_bOnline)
			{
				Protocol::C_PLAYER_YAW_ACTION yawActionPkt;
				yawActionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
				yawActionPkt.mutable_player()->set_name("Player_test");
				yawActionPkt.set_yaw(GetYawToCursor());
				yawActionPkt.set_actionbit(PLAYER_ROLL);
			
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(yawActionPkt));
			}
		}
		m_bCanPlayAnim = false;
		m_fCurLaserTime = 3.f;
		PlayAnimationOnce(&m_arrAnim[ANIM_ROLL]);
		m_bRoll = false;

		return;
	}

	if (m_bLaser && m_bCanPlayAnim)
	{
		if (m_fLaserTime < m_fCurLaserTime)
		{
			m_bLaser = false;
			Get_GameObject<CLaserShotRune>(LAYER_ITEM, L"LaserShotRune")->KillLaser();
		}
		else
		{
			Get_GameObject<CLaserShotRune>(LAYER_ITEM, L"LaserShotRune")->Collision();
			m_fCurLaserTime += CGameUtilMgr::s_fTimeDelta;
			return;
		}	
	}
	
	if (m_bLegacy1 && m_bCanPlayAnim)
	{
		m_bLegacy1 = false;

		if (m_pInventory->CurWeapon(IT_LEGACY1) == nullptr)
			return;
		if (m_pInventory->CurWeapon(IT_LEGACY1)->GetCoolTime() < 1.f)
			return;

		m_eState = LEGACY;
		m_bCanPlayAnim = false;
		PlayAnimationOnce(&m_arrAnim[ANIM_LEGACY1]);
		m_pInventory->CurWeapon(IT_LEGACY1)->Use();

		if (g_bOnline && m_bRemote == false)
		{
			Protocol::C_PLAYER_ACTION actionPkt;
			actionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
			actionPkt.mutable_player()->set_name("Player_test");
			actionPkt.set_actionbit(PLAYER_1);
		
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(actionPkt));
		}
		return;
	}

	if (m_bLegacy2 && m_bCanPlayAnim)
	{
		m_bLegacy2 = false;

		if (m_pInventory->CurWeapon(IT_LEGACY2) == nullptr)
			return;
		if (m_pInventory->CurWeapon(IT_LEGACY2)->GetCoolTime() < 1.f)
			return;

		m_eState = LEGACY;
		m_bCanPlayAnim = false;
		PlayAnimationOnce(&m_arrAnim[ANIM_LEGACY2]);
		m_pInventory->CurWeapon(IT_LEGACY2)->Use();

		if (g_bOnline && m_bRemote == false)
		{
			Protocol::C_PLAYER_ACTION actionPkt;
			actionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
			actionPkt.mutable_player()->set_name("Player_test");
			actionPkt.set_actionbit(PLAYER_2);
		
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(actionPkt));
		}
		return;
	}

	if (m_bLegacy3 && m_bCanPlayAnim)
	{
		m_bLegacy3 = false;

		if (m_pInventory->CurWeapon(IT_LEGACY3) == nullptr)
			return;
		if (m_pInventory->CurWeapon(IT_LEGACY3)->GetCoolTime() < 1.f)
			return;

		m_eState = LEGACY;
		m_bCanPlayAnim = false;
		PlayAnimationOnce(&m_arrAnim[ANIM_LEGACY2]);
		m_pInventory->CurWeapon(IT_LEGACY3)->Use();

		if (g_bOnline && m_bRemote == false)
		{
			Protocol::C_PLAYER_ACTION actionPkt;
			actionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
			actionPkt.mutable_player()->set_name("Player_test");
			actionPkt.set_actionbit(PLAYER_3);
		
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(actionPkt));
		}
		return;
	}

	if (m_bMeleeAttack && m_bCanPlayAnim)
	{
		m_eState = ATTACK;
		if (m_bRemote == false)
		{
			RotateToCursor();
			if (g_bOnline)
			{
				Protocol::C_PLAYER_YAW_ACTION yawActionPkt;
				yawActionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
				yawActionPkt.mutable_player()->set_name("Player_test");
				yawActionPkt.set_yaw(GetYawToCursor());
				yawActionPkt.set_actionbit(PLAYER_ML);
			
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(yawActionPkt));
			}
		}
		WeaponChange(IT_MELEE);
		return;
	}

	if (m_bRangeAttack && m_bCanPlayAnim && m_pInventory->GetArrowCnt() > 0)
	{
		m_eState = ATTACK;
		if (m_bRemote == false)
			RotateToCursor();
		m_bDelay = true;
		WeaponChange(IT_RANGE);
		return;
	}

	if (m_bMove && m_bCanPlayAnim)
	{
		m_eState = WALK;
		RotateToMove();
		m_pIdleAnim = &m_arrAnim[ANIM_WALK];
		m_pCurAnim = &m_arrAnim[ANIM_WALK];

		if (m_bDelay) m_bDelay = false;
		else WeaponChange(IT_MELEE);
		return;
	}

	if (m_bCanPlayAnim)
	{

		m_eState = IDLE;
		m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
		m_pCurAnim = &m_arrAnim[ANIM_IDLE];

		if (m_bDelay) m_bDelay = false;
		else WeaponChange(IT_MELEE);
		return;
	}


}

void CPlayer::RotationToSaton(void)
{
	_vec3 vLook = _vec3(62.5f, 0.f, 44.5f) -m_pRootPart->pTrans->m_vInfo[INFO_POS];

	D3DXVec3Normalize(&vLook, &vLook);

	m_pRootPart->pTrans->m_vInfo[INFO_LOOK] = -vLook;

	m_vTargetPos = vLook;

	const _vec2 v2Look{ 0.f, 1.f };

	_vec2 v2ToDest{ vLook.x, vLook.z };

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		m_pRootPart->pTrans->m_vAngle.y = -acosf(fDot);
	else
		m_pRootPart->pTrans->m_vAngle.y = acosf(fDot);
}

void CPlayer::UsePotion()
{
	if (m_bRemote == false)
	{
		if (s_PotionCollTime <= m_CurPotionCoolTime)
		{
			m_pStat->ModifyHP(_int(_float(m_pStat->GetMaxHP()) * 0.7f));
			m_CurPotionCoolTime = 0.f;

			// particle
			CEffectFactory::Create<CHealCircle>("Heal_Circle_L", L"Heal_Circle_L", m_pRootPart->pTrans->m_vInfo[INFO_POS])
				->SetFollow(m_pRootPart->pTrans);
			for (int i = 0; i < 12; i++)
			{
				CEffectFactory::Create<CHeartParticle>("HeartParticle", L"HeartParticle", m_pRootPart->pTrans->m_vInfo[INFO_POS])
					->SetFollow(m_pRootPart->pTrans);
			}
			
			CSoundMgr::GetInstance()->PlaySoundRandom({
				L"sfx_ui_healthsynergy-001.ogg",
				L"sfx_ui_healthsynergy-002.ogg",
				L"sfx_ui_healthsynergy-003.ogg",
				L"sfx_ui_healthsynergy-004.ogg" },
				m_pRootPart->pTrans->m_vInfo[INFO_POS]);

			if (g_bOnline)
			{
				Protocol::C_PLAYER_ACTION actionPkt;
				actionPkt.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
				actionPkt.mutable_player()->set_name("Player_test");
				actionPkt.set_actionbit(PLAYER_POTION);
			
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(actionPkt));
			}
		}
	}
	else
	{
		m_pStat->ModifyHP(_int(_float(m_pStat->GetMaxHP()) * 0.7f));

		// particle
		CEffectFactory::Create<CHealCircle>("Heal_Circle_L", L"Heal_Circle_L", m_pRootPart->pTrans->m_vInfo[INFO_POS])
			->SetFollow(m_pRootPart->pTrans);
		for (int i = 0; i < 12; i++)
		{
			CEffectFactory::Create<CHeartParticle>("HeartParticle", L"HeartParticle", m_pRootPart->pTrans->m_vInfo[INFO_POS])
				->SetFollow(m_pRootPart->pTrans);
		}
		
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_ui_healthsynergy-001.ogg",
			L"sfx_ui_healthsynergy-002.ogg",
			L"sfx_ui_healthsynergy-003.ogg",
			L"sfx_ui_healthsynergy-004.ogg" },
			m_pRootPart->pTrans->m_vInfo[INFO_POS]);
	}
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote)
{
	CPlayer* pInstance = new CPlayer(pGraphicDev);
	pInstance->m_bRemote = bRemote;

	if (FAILED(pInstance->Ready_Object(wstrPath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	// if (!wstrPath.empty())
	// 	pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

_float CPlayer::GetYawToCursor()
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_vec3 vPoint;
	_vec3 vAt;

	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	// 뷰포트 -> 투영
	vPoint.x = (_float)ptMouse.x / ((_float)ViewPort.Width * 0.5f) - 1.f;
	vPoint.y = (_float)ptMouse.y / -((_float)ViewPort.Height * 0.5f) + 1.f;
	vPoint.z = 0.f;
	vAt.x = vPoint.x;
	vAt.y = vPoint.y;
	vAt.z = 1.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vPoint, &vPoint, &matProj);
	D3DXVec3TransformCoord(&vAt, &vAt, &matProj);

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXVec3TransformCoord(&vPoint, &vPoint, &matView);
	D3DXVec3TransformCoord(&vAt, &vAt, &matView);

	D3DXPLANE tmpPlane;
	D3DXPlaneFromPointNormal(&tmpPlane, &m_pRootPart->pTrans->m_vInfo[INFO_POS], &CGameUtilMgr::s_vUp);

	_vec3 vLookAt;
	D3DXPlaneIntersectLine(&vLookAt, &tmpPlane, &vPoint, &vAt);

	_vec3 vLook = vLookAt - m_pRootPart->pTrans->m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vLook, &vLook);

	const _vec2 v2Look{0.f, 1.f};
	_vec2 v2ToDest{vLook.x, vLook.z};

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		return -acosf(fDot);
	else
		return acosf(fDot);
}

void CPlayer::RotateToCursor()
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_vec3 vPoint;
	_vec3 vAt;

	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	// 뷰포트 -> 투영
	vPoint.x = (_float)ptMouse.x / ((_float)ViewPort.Width * 0.5f) - 1.f;
	vPoint.y = (_float)ptMouse.y / -((_float)ViewPort.Height * 0.5f) + 1.f;
	vPoint.z = 0.f;
	vAt.x = vPoint.x;
	vAt.y = vPoint.y;
	vAt.z = 1.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vPoint, &vPoint, &matProj);
	D3DXVec3TransformCoord(&vAt, &vAt, &matProj);

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXVec3TransformCoord(&vPoint, &vPoint, &matView);
	D3DXVec3TransformCoord(&vAt, &vAt, &matView);

	D3DXPLANE tmpPlane;
	D3DXPlaneFromPointNormal(&tmpPlane, &m_pRootPart->pTrans->m_vInfo[INFO_POS], &CGameUtilMgr::s_vUp);

	_vec3 vLookAt;
	D3DXPlaneIntersectLine(&vLookAt, &tmpPlane, &vPoint, &vAt);

	_vec3 vLook = vLookAt - m_pRootPart->pTrans->m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vLook, &vLook);

	const _vec2 v2Look{0.f, 1.f};
	_vec2 v2ToDest{vLook.x, vLook.z};

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		m_pRootPart->pTrans->m_vAngle.y = -acosf(fDot);
	else
		m_pRootPart->pTrans->m_vAngle.y = acosf(fDot);

	m_pRootPart->pTrans->Update_Component(0.f);

	m_pIdleAnim = &m_arrAnim[ANIM_WALK];
	m_pCurAnim = m_pIdleAnim;
}

void CPlayer::RotateToMove()
{
	const _vec2 v2Look{0.f, 1.f};
	const _vec2 v2ToDest{m_vMoveDirNormal.x, m_vMoveDirNormal.z};
	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);


	if (m_vMoveDirNormal.x < 0)
		m_pRootPart->pTrans->m_vAngle.y = -acosf(fDot);
	else
		m_pRootPart->pTrans->m_vAngle.y = acosf(fDot);
}

void CPlayer::RotateTo(_float fYaw)
{
	m_pRootPart->pTrans->m_vAngle.y = fYaw;
	m_pRootPart->pTrans->Update_Component(0.f);

	m_pIdleAnim = &m_arrAnim[ANIM_WALK];
	m_pCurAnim = m_pIdleAnim;
}

void CPlayer::Legacy4Press()
{ 
	WeaponChange(IT_MELEE);
}

void CPlayer::WeaponChange(ITEMTYPE eIT)
{
	if (m_pWeaponPart == nullptr)
	{
		const auto itr = m_mapParts.find("weapon_r");
		if (itr == m_mapParts.end())
			return;
		m_pWeaponPart = itr->second;
	}
	
	m_pInventory->Equip_Item(m_pWeaponPart, eIT);
	m_arrAnim = m_pInventory->CurWeapon(eIT)->SetarrAnim();
}