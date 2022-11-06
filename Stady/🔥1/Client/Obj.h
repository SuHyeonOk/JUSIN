#pragma once

#include "Include.h"

class CObj
{
public:
	CObj();
	virtual ~CObj();

public: // Creature
	void			OnDamaged(int _iAttack);

public:
	void			Kill_Obj(void) { m_bDead = true; }
	bool			Get_Dead(void) { return m_bDead; }
	string			Get_Tag(void) { return m_sTag; }

	virtual void	OnCollision(CObj* pOther) {};
	virtual void	OnDead() { Kill_Obj(); };

public:
	const	float		Get_Radius() { return m_fRadius; }
	const	INFO&		Get_Info(void)	const { return m_tInfo; }

public:
	void				Set_Pos(const D3DXVECTOR3& vPos) { m_tInfo.vPos = vPos; }
	void				Set_Dir(const D3DXVECTOR3& vDir) 
	{
		m_tInfo.vDir = vDir;
		D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);
	}

public:
	virtual HRESULT		Initialize(void)	PURE;
	virtual int			Update(void)		PURE;
	virtual void		Late_Update(void)	PURE;
	virtual void		Render(void)		PURE;
	virtual void		Release(void)		PURE;

protected:
	void				Move_Frame(void);

protected:
	static D3DXVECTOR3			m_vScroll;

	INFO						m_tInfo;
	string						m_sTag;
	STATS						m_tStats;

	wstring						m_wstrObjKey;
	wstring						m_wstrStateKey = L"";

	FRAME						m_tFrame;

	D3DXVECTOR3					m_vPoint[4];
	D3DXVECTOR3					m_vOriginPoint[4];
	
	float						m_fSpeed = 0.f;
	float						m_fAngle = 0.f;
	float						m_fDiagonal = 0.f;

	float						m_fRadius = 0.f;

	bool						m_bDead;




};

