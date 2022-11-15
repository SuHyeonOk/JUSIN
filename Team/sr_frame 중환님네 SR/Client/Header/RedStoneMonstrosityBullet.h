#pragma once
#include "GameObject.h"

class CRedStoneMonstrosityBullet :
	public CGameObject
{
public:
	explicit CRedStoneMonstrosityBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRedStoneMonstrosityBullet();

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	static CRedStoneMonstrosityBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void Free() override;

	void SetBulletInform(_vec3 vDist, _float fPower) { m_vDist = vDist; m_fPower = fPower; }
private:
	_bool m_bDead = false;
	Engine::CTransform* m_pTransCom = nullptr;
	CRcShader*			m_pBufferCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	_bool				m_bPlayOnce = false;
	_vec3				m_vDist{};
	_float				m_fPower = 0.f;
	_float				m_fTime = 0.f;
};

