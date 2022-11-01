#pragma once
#include "GameObject.h"

enum SPHERETYPE
{
	GOLEM_MELEE_L = 0,
	GOLEM_MELEE_M,
	GOLEM_MELEE_S,
	GOLEM_SPIT,
	SPHERE_L,
	SPHERE_M

};

class CSphereEffect :public CGameObject
{
public:
	explicit CSphereEffect(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	virtual ~CSphereEffect()override;

	virtual HRESULT Ready_Object(_float _size, SPHERETYPE _type);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;


	virtual void Free() override;


public:
	static CSphereEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, SPHERETYPE _type);



private:

	CSphereMesh*			m_pBufferCom = nullptr;
	Engine::CTransform*		m_pTransCom = nullptr;
	_float m_fSpeed;
	_float m_fTime;
	_float m_fCurTime;

};

