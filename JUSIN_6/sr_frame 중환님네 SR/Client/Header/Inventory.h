#pragma once
#include "EquipItem.h"
#include "GameObject.h"
#include "Protocol.pb.h"

#define COL 4
#define ROW 3

class CEquipItem;
class SkeletalCube;
class CConsumeItem;
class CInventoryUI;
class CItemUI;
class CPlayer;
class CWeapon;
class CRune;
class CItemTexUI;

enum LEGACY_SLOT { LEGACY_SLOT1, LEGACY_SLOT2, LEGACY_SLOT3, LEGACY_SLOT_END };

class CInventory :
	public CGameObject
{
protected:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CInventory* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	// 인벤 창 생성
	void OpenInventory();
	// 인벤토리에 아이템 넣기
	void Put(CEquipItem* pItem);
	void Put(CConsumeItem* pItem);
	void TakeOut(CEquipItem* pItem);

	void UseArrow(_uint iArrowCnt) { m_iArrow -= iArrowCnt;}
	void SetOwner(CPlayer* pOwner) { m_pOwner = pOwner; NULL_CHECK(m_pOwner);}
	void SetArrow(_uint iArrowCnt) { m_iArrow = iArrowCnt; }
	void AddDefaultItems();


	//아이템 장착
	void Equip_Item(SkeletalPart* pSkeletalPart, ITEMTYPE eIT);

	CEquipItem* CurWeapon(ITEMTYPE eIT);

	_uint GetEmeraldCnt() const { return m_iEmerald; }
	_uint GetArrowCnt() const { return m_iArrow; }

	_bool InputLock() { return m_bLock; }
	CEquipItem* GetCurClickItem() { return m_pCurClickItem; }

	_float GetLegacyCoolTime(LEGACY_SLOT eType) const
	{
		if (m_arrLegacy[eType] == nullptr)
			return 1.f;
		return m_arrLegacy[eType]->GetCoolTime();
	}
	_uint GetItemUITexNum(LEGACY_SLOT eType) const
	{
		if (m_arrLegacy[eType] == nullptr)
			return 0;
		return m_arrLegacy[eType]->GetUITexNum();
	}
	void ResetWeaponEquipped();

	void SetRune(CRune* pRune) { m_pRune = pRune; }
	void CreateClickFrame();
	void CreateCollFrame(CEquipItem* pCurCollItem);

	void RefreshInventory()
	{
		for (auto& e : m_arrPreEquipProtocol)
			e.Clear();
		m_bEquipChange = true;
	}
protected:
	// for network
	void GetProtocolFromEquip(OUT Protocol::EquipState& state, CEquipItem* pEquipItem);
	CEquipItem* GetEquipFromProtocol(const Protocol::EquipState& state);
	// for network

protected:
	CPlayer* m_pOwner = nullptr;
	array<CEquipItem*, (COL * ROW)> m_arrItem{}; //  아이템 스페이스
	CEquipItem* m_pMelee = nullptr;
	CEquipItem* m_pRange = nullptr;
	array<CEquipItem*, LEGACY_SLOT_END> m_arrLegacy{};

	// 여기 있는 룬을 렌더한다.
	CRune* m_pRune = nullptr;


	//UI
	CInventoryUI* m_pBackGround = nullptr;
	array<CInventoryUI*, (COL * ROW)> m_arrItemSlot{};  // 아이템 스페이스 (빈 공간 UI)
	CInventoryUI* m_pMeleeSlot = nullptr;
	CInventoryUI* m_pRangeSlot = nullptr;
	array<CInventoryUI*, 3> m_arrLegacySlot{};
	CInventoryUI* m_pRuneSlot = nullptr;


	CItemUI* m_pCollFrame = nullptr;
	CItemUI* m_pClickFrame = nullptr;

	//arrItem 이면 equipstate = flase, arrEquip이면 equipstate = true
	//void MouseEvent(CItemSpaceUI* pItemSpaceUI, CEquipItem* pEquipItem, _int index, _bool bEquipState);
	void MouseEvent(CEquipItem* pCurCollItem, CItemUI* pCurCollUI, _int iSlotIndex);
	_uint m_iEmerald = 0;
	_uint m_iArrow = 0;
	_bool m_bLock = false;

	CItemUI*	pCurClickUI = nullptr;
	CEquipItem*	m_pCurClickItem = nullptr;
	CEquipItem*	m_pPreClickItem = nullptr;

	CEquipItem*	m_pCurCollItem = nullptr;
	_int m_iSlotIndex = 0;

	CItemTexUI* m_pItemTexUI = nullptr;


	// equip network
	_bool m_bEquipChange = false;
	 // melee, range, legacy1, 2, 3
	array<Protocol::EquipState, 5> m_arrPreEquipProtocol{};
};

