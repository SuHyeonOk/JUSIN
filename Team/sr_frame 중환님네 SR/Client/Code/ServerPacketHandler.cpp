#include "stdafx.h"
#include "ServerPacketHandler.h"
#include "ClientServiceMgr.h"
#include "AbstFactory.h"
#include "Kouku.h"
#include "KoukuController.h"
#include "Player.h"
#include "PlayerController.h"
#include "ZombieController.h"
#include "RemoteInventory.h"
#include "Stage_Kouku.h"
#include "SatonController.h"
#include "StatComponent.h"
#include "ObjectStoreMgr.h"
#include "Portrait.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];



// Á÷Á¢ ÄÁÅÙÃ÷ ÀÛ¾÷ÀÚ

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return true;

	CClientServiceMgr::GetInstance()->m_iPlayerID = pkt.playerid();

	Protocol::C_ENTER_GAME enterPkt;
	
	enterPkt.mutable_player()->set_id(pkt.playerid());
	enterPkt.mutable_player()->set_name(CObjectStoreMgr::GetInstance()->GetPlayerName());
	enterPkt.set_playerskin(CObjectStoreMgr::GetInstance()->GetPlayerSkin());

	auto SendBuffer = ServerPacketHandler::MakeSendBuffer(enterPkt);
	session->Send(SendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (pkt.success() == false)
		return true;

	if (CClientServiceMgr::GetInstance()->m_iPlayerID == pkt.player().id())
	{
		CPlayer* pPlayer = Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, L"Player");

		pPlayer->SetID(CClientServiceMgr::GetInstance()->m_iPlayerID);
		pPlayer->SetName(pkt.player().name());
		pPlayer->PlayerSpawn();
		return true;
	}

	return true;
}

bool Handle_S_OTHER_PLAYER(PacketSessionRef& session, Protocol::S_OTHER_PLAYER& pkt)
{
	for (int i = 0; i < pkt.player_size(); ++i)
	{

		Protocol::Player* pPlayerPkt = pkt.mutable_player(i);
		Protocol::PlayerSkin eSkin = pkt.playerskin(i);
		if (pPlayerPkt->id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
			continue;

		wstring wstrPlayerTag = L"Player_Remote_" + to_wstring(pPlayerPkt->id());

		if (Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, wstrPlayerTag) != nullptr)
			continue;

		CPlayer* pPlayer = nullptr;
		switch (eSkin)
		{
		case Protocol::PLAYER_TYPE_STEVE:
			pPlayer = CPlayerFactory::Create<CPlayer>("Steve_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
			break;
		case Protocol::PLAYER_TYPE_PRIDE:
			pPlayer = CPlayerFactory::Create<CPlayer>("Pride_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
			break;
		case Protocol::PLAYER_TYPE_ESHE:
			pPlayer = CPlayerFactory::Create<CPlayer>("Eshe_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
			break;
		case Protocol::PLAYER_TYPE_COPPER:
			pPlayer = CPlayerFactory::Create<CPlayer>("Copper_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
			break;
		case Protocol::PlayerSkin_INT_MIN_SENTINEL_DO_NOT_USE_:
		case Protocol::PlayerSkin_INT_MAX_SENTINEL_DO_NOT_USE_:
		default:
			_CRASH("Wrong Skin");
		}

		pPlayer->SetID(pPlayerPkt->id());
		pPlayer->SetName(pPlayerPkt->name());
		pPlayer->PlayerSpawn();
	}

	return true;
}

bool Handle_S_PLAYER_INPUT(PacketSessionRef& session, Protocol::S_PLAYER_INPUT& pkt)
{
	if (pkt.player().id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;

	Engine::Get_Component<CPlayerRemoteController>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id()), L"Proto_PlayerRemoteController",  ID_DYNAMIC)
		->SetInputMask(pkt.inputbit());

	return true;
}

bool Handle_S_PLAYER_WORLD(PacketSessionRef& session, Protocol::S_PLAYER_WORLD& pkt)
{
	if (pkt.success() == false)
		return true;
	if (pkt.player().id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;


	_matrix matWorld;
	CClientServiceMgr::Pkt2mat(pkt.matworld(), matWorld);
	Engine::Get_Component<CPlayerRemoteController>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id()), L"Proto_PlayerRemoteController",  ID_DYNAMIC)
		->SetWorld(matWorld);

	return true;
}

bool Handle_S_PLAYER_YAW_ACTION(PacketSessionRef& session, Protocol::S_PLAYER_YAW_ACTION& pkt)
{
	if (pkt.success() == false)
		return true;
	if (pkt.player().id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;

	Engine::Get_Component<CPlayerRemoteController>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id()), L"Proto_PlayerRemoteController",  ID_DYNAMIC)
		->SetYawAction(pkt.yaw(), pkt.actionbit());
	return true;
}

bool Handle_S_PLAYER_ACTION(PacketSessionRef& session, Protocol::S_PLAYER_ACTION& pkt)
{
	if (pkt.success() == false)
		return true;
	if (pkt.player().id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;

	Engine::Get_Component<CPlayerRemoteController>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id()), L"Proto_PlayerRemoteController",  ID_DYNAMIC)
		->SetAction(pkt.actionbit());

	return true;
}

bool Handle_S_PLAYER_ARROW(PacketSessionRef& session, Protocol::S_PLAYER_ARROW& pkt)
{
	if (pkt.success() == false)
		return true;
	if (pkt.player().id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;

	_vec3 vLookAt;
	vLookAt.x =  pkt.vlookat().x();
	vLookAt.y =  pkt.vlookat().y();
	vLookAt.z =  pkt.vlookat().z();

	Engine::Get_Component<CPlayerRemoteController>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id()), L"Proto_PlayerRemoteController",  ID_DYNAMIC)
		->SetArrow(pkt.yaw(), vLookAt, pkt.actionbit());

	return true;
}

bool Handle_S_PLAYER_EQUIP(PacketSessionRef& session, Protocol::S_PLAYER_EQUIP& pkt)
{
	if (pkt.success() == false)
		return true;
	if (pkt.player().id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;

	if (CPlayer* pPlayer = Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id())))
	{
		if (CRemoteInventory* pInven = dynamic_cast<CRemoteInventory*>(pPlayer->GetInventory()))
		{
			pInven->SetEquipStateChange(pkt.state());
		}
	}

	return true;
}

bool Handle_S_ALL_PLAYER_ENTER(PacketSessionRef& session, Protocol::S_ALL_PLAYER_ENTER& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID != 0) // if not host
		return true;

	// CScene* pCurScene = CManagement::GetInstance()->GetScene();
	// if (dynamic_cast<CStage_Kouku*>(pCurScene))
	// {
	// 	_matrix matWorld;
	// 	{
	// 		CGameUtilMgr::MatWorldComposeEuler(matWorld, { 3.f, 3.f, 3.f }, { 0.f, D3DXToRadian(90.f) ,0.f }, { 62.5f, 21.5f ,47.8f });
	// 		Protocol::C_BOSS_SPAWN satonSpawnPkt;
	// 		satonSpawnPkt.set_factory("Saton");
	// 		CClientServiceMgr::Mat2Pkt(matWorld, *satonSpawnPkt.mutable_matrix());
	//
	// 		session->Send(ServerPacketHandler::MakeSendBuffer(satonSpawnPkt));
	// 	}
	// 	{
	// 		CGameUtilMgr::MatWorldComposeEuler(matWorld, { 0.7f, 0.7f, 0.7f }, { 0.f, D3DXToRadian(90.f) ,0.f }, { 62.5f, 25.f ,44.8f });
	// 		Protocol::C_BOSS_SPAWN koukuSpawnPkt;
	// 		koukuSpawnPkt.set_factory("Kouku");
	// 		CClientServiceMgr::Mat2Pkt(matWorld, *koukuSpawnPkt.mutable_matrix());
	// 		session->Send(ServerPacketHandler::MakeSendBuffer(koukuSpawnPkt));
	// 	}
	// }

	return true;
}

bool Handle_S_PLAYER_DEAD(PacketSessionRef& session, Protocol::S_PLAYER_DEAD& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == pkt.player().id()) 
		return true;

	for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second))
		{
			if (pPlayer->GetID() == pkt.player().id())
			{
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)->SetDead();
				break;
			}
		}
	}

	return true;
}

bool Handle_S_PLAYER_RESPAWN(PacketSessionRef& session, Protocol::S_PLAYER_RESPAWN& pkt)
{
	if (pkt.success() == false)
		return true;

	if (CClientServiceMgr::GetInstance()->m_iPlayerID == pkt.player().id()) 
		return true;

	// if (CPlayer* pPlayer = Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, L"Player_Remote_" + to_wstring(pkt.player().id())))
	// {
	// 	pPlayer->PlayerSpawn();
	// }

	for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second))
		{
			if (pPlayer->GetID() == pkt.player().id())
			{
				pPlayer->PlayerSpawn();
				break;
			}
		}
	}

	return true;
}

bool Handle_S_PLAYER_MOVE_STAGE(PacketSessionRef& session, Protocol::S_PLAYER_MOVE_STAGE& pkt)
{
	if (pkt.success() == false)
		return true;

	CSceneFactory::LoadScene(pkt.loadingtag(), pkt.stagetag(), true, 1000);
	return true;
}

bool Handle_S_PLAYER_MOVE_STAGE_FINISH(PacketSessionRef& session, Protocol::S_PLAYER_MOVE_STAGE_FINISH& pkt)
{
	if (auto pKoukuScene = dynamic_cast<CStage_Kouku*>(CManagement::GetInstance()->GetScene()))
	{
		while(pKoukuScene->m_bLoadingDone == false)
		{
			
		}
	}
	

	Protocol::Player pPlayerPkt = pkt.player();
	Protocol::PlayerSkin eSkin = pkt.playerskin();
	if (pPlayerPkt.id() == CClientServiceMgr::GetInstance()->m_iPlayerID)
		return true;

	wstring wstrPlayerTag = L"Player_Remote_" + to_wstring(pPlayerPkt.id());

	if (Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, wstrPlayerTag) != nullptr)
		return true;

	CPlayer* pPlayer = nullptr;
	switch (eSkin)
	{
	case Protocol::PLAYER_TYPE_STEVE:
		pPlayer = CPlayerFactory::Create<CPlayer>("Steve_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
		break;
	case Protocol::PLAYER_TYPE_PRIDE:
		pPlayer = CPlayerFactory::Create<CPlayer>("Pride_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
		break;
	case Protocol::PLAYER_TYPE_ESHE:
		pPlayer = CPlayerFactory::Create<CPlayer>("Eshe_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
		break;
	case Protocol::PLAYER_TYPE_COPPER:
		pPlayer = CPlayerFactory::Create<CPlayer>("Copper_Remote", wstrPlayerTag, CGameUtilMgr::s_matIdentity);
		break;
	case Protocol::PlayerSkin_INT_MIN_SENTINEL_DO_NOT_USE_:
	case Protocol::PlayerSkin_INT_MAX_SENTINEL_DO_NOT_USE_:
	default:
		_CRASH("Wrong Skin");
	}

	pPlayer->SetID(pPlayerPkt.id());
	pPlayer->SetName(pPlayerPkt.name());
	pPlayer->PlayerSpawn();

	return true;
}

bool Handle_S_SPAWN_MONSTER(PacketSessionRef& session, Protocol::S_SPAWN_MONSTER& pkt)
{
	if (pkt.success() == false)
		return true;

	wstring tmp;
	tmp.assign(pkt.factory().begin(), pkt.factory().end());
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // host 
	{
		_matrix matWorld = CGameUtilMgr::s_matIdentity;
		matWorld._41 = 3.f;
		matWorld._43 = 7.f;
		CEnemyFactory::Create<CGameObject>(pkt.factory(), tmp + L"_" + to_wstring(pkt.id()), matWorld)
			->SetID(pkt.id());
	}
	else
	{
		_matrix matWorld = CGameUtilMgr::s_matIdentity;
		CEnemyFactory::Create<CGameObject>(pkt.factory() + "_Remote", tmp + L"_Remote_" + to_wstring(pkt.id()), matWorld)
			->SetID(pkt.id());
	}

	return true;
}

bool Handle_S_MONSTER_SET_TARGET(PacketSessionRef& session, Protocol::S_MONSTER_SET_TARGET& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // host 
		return true;

	CZombieRemoteController* pCon = Engine::Get_ComponentUnCheck<CZombieRemoteController>(LAYER_ENEMY, L"Zombie_Remote_" + to_wstring(pkt.monsterid()), L"Proto_ZombieRemoteController",  ID_DYNAMIC);
	if (pCon)
	{
		if (pkt.battack())
			pCon->SetAttack();

		pCon->SetTarget(pkt.playerid());
	}

	return true;
}

bool Handle_S_MONSTER_WORLD(PacketSessionRef& session, Protocol::S_MONSTER_WORLD& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // host 
		return true;

	CZombieRemoteController* pCon = Engine::Get_ComponentUnCheck<CZombieRemoteController>(LAYER_ENEMY, L"Zombie_Remote_" + to_wstring(pkt.monsterid()), L"Proto_ZombieRemoteController",  ID_DYNAMIC);
	if (pCon)
	{
		_matrix matWorld;
		CClientServiceMgr::Pkt2mat(pkt.matworld(), matWorld);
		pCon->SetWorld(matWorld);
	}


	return true;
}

bool Handle_S_BOSS_SPAWN(PacketSessionRef& session, Protocol::S_BOSS_SPAWN& pkt)
{
	if (pkt.success() == false)
		return true;

	wstring wstrFactory(pkt.factory().begin(), pkt.factory().end());
	_matrix matWorld;
	CClientServiceMgr::Pkt2mat(pkt.matrix(), matWorld);

	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // if host
	{
		CEnemyFactory::Create<CGameObject>(pkt.factory(), wstrFactory, matWorld);
	}
	else
	{
		CEnemyFactory::Create<CGameObject>(pkt.factory() + "_Remote", wstrFactory + L"_Remote", matWorld);
	}

	return true;
}

bool Handle_S_BOSS_WORLD(PacketSessionRef& session, Protocol::S_BOSS_WORLD& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // host 
		return true;

	if (pkt.objkey() == "Saton")
	{
		CSatonRemoteController* pCon = Engine::Get_ComponentUnCheck<CSatonRemoteController>(LAYER_ENEMY, L"Saton_Remote", L"Proto_SatonRemoteController",  ID_DYNAMIC);
		if (pCon)
		{
			_matrix matWorld;
			CClientServiceMgr::Pkt2mat(pkt.matworld(), matWorld);
			pCon->SetWorld(matWorld);
		}
	}
	else if (pkt.objkey() == "Kouku")
	{
		CKoukuRemoteController* pCon = Engine::Get_ComponentUnCheck<CKoukuRemoteController>(LAYER_ENEMY, L"Kouku_Remote", L"Proto_KoukuRemoteController",  ID_DYNAMIC);
		if (pCon)
		{
			_matrix matWorld;
			CClientServiceMgr::Pkt2mat(pkt.matworld(), matWorld);
			pCon->SetWorld(matWorld, pkt.ihp());
		}
	}


	return true;
}

bool Handle_S_SATON_ATTACK(PacketSessionRef& session, Protocol::S_SATON_ATTACK& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // host 
		return true;

	CSatonRemoteController* pCon = Engine::Get_ComponentUnCheck<CSatonRemoteController>(LAYER_ENEMY, L"Saton_Remote", L"Proto_SatonRemoteController",  ID_DYNAMIC);
	if (pCon)
	{
		_vec3 vTargetPos;
		vTargetPos.x = pkt.targetpos().x();
		vTargetPos.y = pkt.targetpos().y();
		vTargetPos.z = pkt.targetpos().z();

		pCon->SetPattern(vTargetPos, pkt.pattern());
	}

	return true;
}

bool Handle_S_KOUKU_ATTACK(PacketSessionRef& session, Protocol::S_KOUKU_ATTACK& pkt)
{
	if (pkt.success() == false)
		return true;
	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // host 
		return true;

	CKoukuRemoteController* pCon = Engine::Get_ComponentUnCheck<CKoukuRemoteController>(LAYER_ENEMY, L"Kouku_Remote", L"Proto_KoukuRemoteController",  ID_DYNAMIC);
	if (pCon)
	{
		_vec3 vTargetPos;
		vTargetPos.x = pkt.targetpos().x();
		vTargetPos.y = pkt.targetpos().y();
		vTargetPos.z = pkt.targetpos().z();

		pCon->SetPattern(vTargetPos, pkt.pattern());
	}
	return true;
}

bool Handle_S_KOUKU_COUNTER(PacketSessionRef& session, Protocol::S_KOUKU_COUNTER& pkt)
{
	if (pkt.success() == false)
		return true;

	CKouku* pKouku = nullptr;

	if (CClientServiceMgr::GetInstance()->m_iPlayerID == 0) // if host
	{
		pKouku = Get_GameObject<CKouku>(LAYER_ENEMY, L"Kouku");
	}
	else
	{
		pKouku = Get_GameObject<CKouku>(LAYER_ENEMY, L"Kouku_Remote");
	}

	if (pKouku == nullptr) _CRASH("Kouku not exist");

	CStatComponent* pStat = pKouku->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC);
	if (pStat->IsStun() == false)
		pKouku->SetKoukuCounter();

	return true;
}

bool Handle_S_KOUKU_RESULT(PacketSessionRef& session, Protocol::S_KOUKU_RESULT& pkt)
{
	if (pkt.success() == false)
		return true;

	CPortrait* port = CUIFactory::Create<CPortrait>("PortraitUI", L"PortraitUI", -1, WINCX/2, WINCY/2, WINCX, WINCY);

	vector<Protocol::PlayerResult*> vecResult;
	for (int i = 0; i < pkt.result_size(); ++i)
	{
		vecResult.push_back(pkt.mutable_result(i));
	}

	std::sort(vecResult.begin(), vecResult.end(), [](Protocol::PlayerResult* p1, Protocol::PlayerResult* p2)
	{
		return p1->damage() > p2->damage();
	});

	
	for (size_t i = 0 ; i < vecResult.size(); ++i)
	{
		wstring tmp(vecResult[i]->player().name().begin(), vecResult[i]->player().name().end());
		port->SetPlayerSkeletal(vecResult[i]->skin(), PLAYER_MVP_TYPE(i), (_float)vecResult[i]->damage() * 100.f / 12000, vecResult[i]->counter(), tmp);
	}


	return true;
}

