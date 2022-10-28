#pragma once
#include "Item.h"
#include "SkeletalCube.h"
#include "ItemUI.h"

class CPlayer;


class CEquipItem :
	public CItem
{
protected:

	enum Animation
	{
		ANIM_WALK,
		ANIM_IDLE,
		ANIM_DEAD,
		ANIM_ATTACK1,
		ANIM_ATTACK2,
		ANIM_ATTACK3,
		ANIM_RANGE_ATTACK,
		ANIM_LEGACY1,
		ANIM_LEGACY2,
		ANIM_LEGACY3,
		ANIM_ROLL,
		ANIM_TELEPORT,
		ANIM_RESCUE,
		ANIM_END
	};

	explicit CEquipItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEquipItem();

public:
	//gameobj function
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	// equipitem function
	virtual void Equipment(SkeletalPart* pSkeletalPart) {}
	virtual void Collision() {}
	//현제 플레이어 한명일때만 가정해서 구현. 서버되면 수정
	virtual _int Attack() { return 0; }
	virtual void Use() { m_bUse = true; }
	_float GetCoolTime() const { return m_fCurCoolTime / m_fCoolTime; }
	void SetOwner(CPlayer* pOwner) { m_pOwner = pOwner; NULL_CHECK(m_pOwner); }
	virtual void SetDelete() override;
public:
	const array<CubeAnimFrame, ANIM_END>& SetarrAnim(){ return m_arrAnim; }
	ITEMTYPE GetItemType() { return m_eItemType; }
	_int	GetUITexNum() { return m_iUItexNum; }
	CItemUI* GetItemUI() { return m_pItemUI; }

protected:
	CPlayer* m_pOwner = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;
	Engine::CVoxelTex*	m_pBufferCom = nullptr;
	
	//UI
	CItemUI* m_pItemUI;
	_uint m_iUItexNum;

	array<CubeAnimFrame, ANIM_END> m_arrAnim{};
	_uint m_iAttackCnt; // 콤보 번호
	_bool m_bUse;
	ITEMTYPE	m_eItemType;


	// 유물 쿨타임(Legacy로 상속단계 추가했어야 했는데....)
	// 굿
	_float m_fCoolTime = 1.f;
	_float m_fCurCoolTime = 1.f; 
};

