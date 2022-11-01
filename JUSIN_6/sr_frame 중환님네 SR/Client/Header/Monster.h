#pragma once
#include "Player.h"
#include "SkeletalCube.h"

class CStatComponent;
class CPlayer;
class CController;

class CMonster : public CSkeletalCube
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;

	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);
	virtual void Free() override;

	_bool CheckCC() { return m_bCantCC; }

protected:
	void RotateToTargetPos(const _vec3& vTargetPos, bool bReverse = false);

protected:
	CCollisionCom* m_pColl = nullptr;
	CStatComponent* m_pStat = nullptr;
	string m_strState;
	_float m_fSpeed; // ¼Óµµ
	_bool m_bDelete = false;
	_bool m_bCantCC = false;

};

