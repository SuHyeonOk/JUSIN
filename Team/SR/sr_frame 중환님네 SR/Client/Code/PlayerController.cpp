#include "stdafx.h"
#include "PlayerController.h"
#include "Player.h"
#include "Box.h"
#include "Inventory.h"
#include "EquipItem.h"
#include "ConsumeItem.h"
#include "Dynamite.h"
#include "InventoryUI.h"
#include "AbstFactory.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "MapUI.h"
#include "MapTable.h"

CPlayerController::CPlayerController() : CController()
{
}

CPlayerController::CPlayerController(const CPlayerController& rhs) : CController(rhs)
{
}


CPlayerController::~CPlayerController()
{
}

_int CPlayerController::Update_Component(const _float& fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pOwner);
	NULL_CHECK_RETURN(pPlayer, 0);
	if (pPlayer->IsVisible() == false) return 0;

	//이거 내리면 인벤 안꺼짐
	if (DIKeyUp(DIK_I))
	{
		pPlayer->GetInventory()->OpenInventory();
	}

	if (pPlayer->GetInventory()->InputLock())
		return 0;

	m_vPressDir = CGameUtilMgr::s_vZero;
	// 움직임 입력
	{
		if (DIKeyPressing(DIK_W))
		{
			m_vPressDir.z += 1.f;
		}
		if (DIKeyPressing(DIK_A))
		{
			m_vPressDir.x += -1.f;
		}
		if (DIKeyPressing(DIK_S))
		{
			m_vPressDir.z += -1.f;
		}
		if (DIKeyPressing(DIK_D))
		{
			m_vPressDir.x += 1.f;
		}

		// if (DIKeyDown(DIK_W))
		// {
		// 	m_vPressDir.z += 1.f;
		// }
		// if (DIKeyUp(DIK_S))
		// {
		// 	m_vPressDir.z += 1.f;
		// }
		//
		// if (DIKeyDown(DIK_A))
		// {
		// 	m_vPressDir.x += -1.f;
		// }
		// if (DIKeyUp(DIK_D))
		// {
		// 	m_vPressDir.x += -1.f;
		// }
		//
		// if (DIKeyDown(DIK_S))
		// {
		// 	m_vPressDir.z += -1.f;
		// }
		// if (DIKeyUp(DIK_W))
		// {
		// 	m_vPressDir.z += -1.f;
		// }
		//
		// if (DIKeyDown(DIK_D))
		// {
		// 	m_vPressDir.x += 1.f;
		// }
		// if (DIKeyUp(DIK_A))
		// {
		// 	m_vPressDir.x += 1.f;
		// }
	}

	//box open
	if (DIKeyDown(DIK_F))
	{
		_vec3 vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
		//박스 열기 , 폭탄 줍기, MapUI열기
		pickGameObj(pPlayer, vTargetPos);
		//아이템 먹기
		putItem(pPlayer, vTargetPos);		
	}

	

	if (false == CGameUtilMgr::Vec3Cmp(m_vPressDir, m_vPrevPressDir)) // 이동 입력 없으면 방향 계산 안하기
	{
		D3DXVec3Normalize(&m_vMoveDir, &m_vPressDir);
		_float fCamYaw = Engine::Get_Component<Engine::CTransform>(LAYER_ENV, L"StaticCamera", L"Proto_TransformCom",
		                                                           ID_DYNAMIC)->
		                 m_vAngle.y;
		_matrix matYaw;
		D3DXMatrixRotationY(&matYaw, fCamYaw);
		D3DXVec3TransformNormal(&m_vMoveDir, &m_vMoveDir, &matYaw);
		pPlayer->SetMoveDir(m_vMoveDir.x, m_vMoveDir.z);


		if (g_bOnline)
		{
			m_iPlayerInputMask = 0;
			if (m_vPressDir.x > 0.f)
				m_iPlayerInputMask |= PLAYER_D;
			else if (m_vPressDir.x < 0.f)
				m_iPlayerInputMask |= PLAYER_A;

			if (m_vPressDir.z > 0.f)
				m_iPlayerInputMask |= PLAYER_W;
			else if (m_vPressDir.z < 0.f)
				m_iPlayerInputMask |= PLAYER_S;

			Protocol::C_PLAYER_INPUT playerInput;
			playerInput.set_inputbit(m_iPlayerInputMask);
			playerInput.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
			playerInput.mutable_player()->set_name("Player_test");

			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(playerInput));
		}
	}


	m_vPrevPressDir = m_vPressDir;


	if (DIKeyDown(DIK_SPACE))
	{
		pPlayer->RollPress();
	}

	if (MouseKeyDown(DIM_LB))
		pPlayer->MeleeAttackPress(true);
	if (MouseKeyUp(DIM_LB))
		pPlayer->MeleeAttackPress(false);

	if (MouseKeyDown(DIM_RB))
	{
		if (m_pDynamite && m_pDynamite->GetState() == DYNAMITE_PICK)
		{
			m_pDynamite->SetState(DYNAMITE_THROW);
			m_pDynamite = nullptr;
			return OBJ_NOEVENT;
		}
	}
	if (MouseKeyUp(DIM_RB))
		pPlayer->RangeAttackPress(false);
	if (MouseKeyPress(DIM_RB) == false)
		pPlayer->RangeAttackPress(false);
	if (MouseKeyDown(DIM_RB))
		pPlayer->RangeAttackPress(true);


	if (DIKeyDown(DIK_1))
	{
		pPlayer->Legacy1Press();
	}
	if (DIKeyDown(DIK_2))
	{
		pPlayer->Legacy2Press();
	}
	if (DIKeyDown(DIK_3))
	{
		pPlayer->Legacy3Press();
	}
	if (DIKeyDown(DIK_4))
	{
		pPlayer->Legacy4Press();
	}
	if (DIKeyDown(DIK_E))
	{
		pPlayer->UsePotion();
	}


	if (g_bOnline)
	{
		if (m_fWorldRefreshTime <= m_fCurWorldRefreshTime)
		{
			
			Protocol::C_PLAYER_WORLD playerWorld;
			playerWorld.mutable_player()->set_id(CClientServiceMgr::GetInstance()->m_iPlayerID);
			playerWorld.mutable_player()->set_name("Player_test");

			const _matrix& matWorld =	pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
			CClientServiceMgr::Mat2Pkt(matWorld, *playerWorld.mutable_matworld());

			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(playerWorld));
			m_fCurWorldRefreshTime = 0.f;
		}
		else
			m_fCurWorldRefreshTime += fTimeDelta;
	}

	return 0;
}

CComponent* CPlayerController::Clone()
{
	return new CPlayerController(*this);
}

CPlayerController* CPlayerController::Create()
{
	return new CPlayerController;
}


void CPlayerController::putItem(CPlayer* pPlayer, const _vec3& vTargetPos)
{
	_float fEquipItemDist = 3.f;
	_float fConsumItemDist = 3.f;
	CEquipItem* pEquipItem = nullptr;
	CConsumeItem* pConsumItem = nullptr;

	for (auto& ele : Get_Layer(LAYER_ITEM)->Get_MapObject())
	{
		if (CEquipItem* pItem = dynamic_cast<CEquipItem*>(ele.second))
		{
			if (pItem->GetItemState() == IS_DROP)
			{
				_vec3 vDiff = vTargetPos - pItem->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->
				                                  m_vInfo[INFO_POS];
				_float fDist = D3DXVec3Length(&vDiff);

				if (fDist < fEquipItemDist)
				{
					pEquipItem = pItem;
					fEquipItemDist = fDist;
				}
			}
		}

		if (CConsumeItem* pItem = dynamic_cast<CConsumeItem*>(ele.second))
		{
			_vec3 vDiff = vTargetPos - pItem->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->
			                                  m_vInfo[INFO_POS];
			_float fDist = D3DXVec3Length(&vDiff);

			if (pItem->GetItemState() == IS_DROP)
			{
				if (fDist < fConsumItemDist)
				{
					pConsumItem = pItem;
					fConsumItemDist = fDist;
				}
			}
		}
	}

	if (!pEquipItem && !pConsumItem)
		return;

	fEquipItemDist >= fConsumItemDist ? pPlayer->GetInventory()->Put(pConsumItem) : pPlayer->GetInventory()->Put(pEquipItem);

	CSoundMgr::GetInstance()->PlaySoundRandom({
		L"sfx_item_eatGeneric-001.ogg",
		L"sfx_item_eatGeneric-002.ogg",
		L"sfx_item_eatGeneric-003.ogg",
		L"sfx_item_eatGeneric-004.ogg",
		L"sfx_item_eatGeneric-005.ogg",
		L"sfx_item_eatGeneric-006.ogg" },
		vTargetPos);

}

void CPlayerController::pickGameObj(CPlayer* pPlayer, const _vec3& vTargetPos)
{
	_float fDynamiteDist = 3.f;
	_float fBoxDist = 3.f;
	_float fTableDist = 3.f;
	CBox* pBox = nullptr;
	CDynamite* pDynamite = nullptr;
	CMapTable* pMapTable = nullptr;

	for (auto& ele : Get_Layer(LAYER_GAMEOBJ)->Get_MapObject())
	{
		if (CDynamite* pGameObj = dynamic_cast<CDynamite*>(ele.second))
		{
			_vec3 vDiff = vTargetPos - pGameObj->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->
			                                     m_vInfo[INFO_POS];
			_float fDist = D3DXVec3Length(&vDiff);

			if (fDist < fDynamiteDist)
			{
				pDynamite = pGameObj;
				fDynamiteDist = fDist;
			}
		}

		if (CBox* pGameObj = dynamic_cast<CBox*>(ele.second))
		{
			_vec3 vDiff = vTargetPos - pGameObj->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_float fDist = D3DXVec3Length(&vDiff);

			if (fDist < fBoxDist)
			{
				pBox = pGameObj;
				fBoxDist = fDist;
			}
		}

		if (CMapTable* pGameObj = dynamic_cast<CMapTable*>(ele.second))
		{
			_vec3 vDiff = vTargetPos - pGameObj->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_float fDist = D3DXVec3Length(&vDiff);

			if (fDist < fTableDist)
			{
				pMapTable = pGameObj;
				fTableDist = fDist;
			}
		}
	}

	if (pBox)
	{
		pBox->BoxOpen();
		return;
	}

	if (pDynamite)
	{
		pDynamite->SetState(DYNAMITE_PICK);
		m_pDynamite = pDynamite;
		return;
	}

	if (pMapTable)
	{
		Engine::Get_GameObject<CMapUI>(LAYER_UI, L"MapUI")->Open();
		return;
	}
	CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_item_eatGeneric-001.ogg",
			L"sfx_item_eatGeneric-002.ogg",
			L"sfx_item_eatGeneric-003.ogg",
			L"sfx_item_eatGeneric-004.ogg",
			L"sfx_item_eatGeneric-005.ogg",
			L"sfx_item_eatGeneric-006.ogg"},
			vTargetPos);
}

/*----------------------
 * Remote
 -------------------*/
CPlayerRemoteController::CPlayerRemoteController() : CPlayerController()
{
}

CPlayerRemoteController::CPlayerRemoteController(const CPlayerRemoteController& rhs)
	: CPlayerController(rhs)
{
}

CPlayerRemoteController::~CPlayerRemoteController()
{
}

_int CPlayerRemoteController::Update_Component(const _float& fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pOwner);
	NULL_CHECK_RETURN(pPlayer, 0);

	m_vPressDir = CGameUtilMgr::s_vZero;
	if (m_iPlayerInputMask & PLAYER_W)
		m_vPressDir.z += 1.f;

	if (m_iPlayerInputMask & PLAYER_A)
		m_vPressDir.x += -1.f;

	if (m_iPlayerInputMask & PLAYER_S)
		m_vPressDir.z += -1.f;

	if (m_iPlayerInputMask & PLAYER_D)
		m_vPressDir.x += 1.f;


	if (false == CGameUtilMgr::Vec3Cmp(m_vPressDir, m_vPrevPressDir)) // 이동 입력 없으면 방향 계산 안하기
	{
		D3DXVec3Normalize(&m_vMoveDir, &m_vPressDir);
		_float fCamYaw = Engine::Get_Component<Engine::CTransform>(LAYER_ENV, L"StaticCamera", L"Proto_TransformCom",ID_DYNAMIC)
			->m_vAngle.y;
		_matrix matYaw;
		D3DXMatrixRotationY(&matYaw, fCamYaw);
		D3DXVec3TransformNormal(&m_vMoveDir, &m_vMoveDir, &matYaw);
		pPlayer->SetMoveDir(m_vMoveDir.x, m_vMoveDir.z);

		m_vPrevPressDir = m_vPressDir;
	}


	if (m_bWorldSet)
	{
		pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
			->Set_WorldDecompose(m_matWorld);

		m_bWorldSet.store(false);
	}

	if (m_bYawActionSet)
	{
		pPlayer->RotateTo(m_fYaw);
		if (m_iYawAction & PLAYER_ROLL)
		{
			pPlayer->RollPress();
		}
		if (m_iYawAction & PLAYER_ML)
		{
			pPlayer->MeleeAttackPress(true);
		}

		m_iYawAction.store(0);
		m_bYawActionSet.store(false);
	}
	else
	{
		pPlayer->MeleeAttackPress(false);
	}

	if (m_bActionSet)
	{
		if (m_iAction & PLAYER_POTION)
			pPlayer->UsePotion();
		if (m_iAction & PLAYER_1)
			pPlayer->Legacy1Press();
		if (m_iAction & PLAYER_2)
			pPlayer->Legacy2Press();
		if (m_iAction & PLAYER_3)
			pPlayer->Legacy3Press();

		m_iAction.store(0);
		m_bActionSet.store(false);
	}

	if (m_bArrowActionSet)
	{
		if (m_iArrowAction & PLAYER_MR)
		{
			pPlayer->RotateTo(m_fYaw);
			pPlayer->RangeAttackPress(true);
			pPlayer->SetArrowLookAt(m_vLookAt);
		}

		m_iArrowAction.store(0);
		m_bArrowActionSet.store(false);
	}
	else
	{
		pPlayer->RangeAttackPress(false);
	}

	return 0;
}

CComponent* CPlayerRemoteController::Clone()
{
	return new CPlayerRemoteController(*this);
}

CPlayerRemoteController* CPlayerRemoteController::Create()
{
	return new CPlayerRemoteController;
}

void CPlayerRemoteController::SetYawAction(_float fYaw, _uint iAction)
{
	m_fYaw = fYaw;
	m_iYawAction.store(iAction);
	m_bYawActionSet.store(true);
}

void CPlayerRemoteController::SetAction(_uint iAction)
{
	m_iAction.store(iAction);
	m_bActionSet.store(true);
}

void CPlayerRemoteController::SetArrow(_float fYaw, _vec3 vLookAt, _uint iAction)
{
	m_fYaw = fYaw;
	m_vLookAt = vLookAt;
	m_bArrowActionSet.store(true);
	m_iArrowAction.store(iAction);
}
