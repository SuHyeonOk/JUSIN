#pragma once
#include "GameObject.h"

enum DYNAMITE_STATE { DYNAMITE_PICK, DYNAMITE_THROW, DYNAMITE_BOOM, DYNAMITE_END };

class CDynamite :
	public CGameObject
{
private:

	enum BOUNCE { THROWING, ONE_BOUNCE, TWO_BOUNCE,  BOUNCE_END };


	explicit CDynamite(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamite();

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CDynamite* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void SetState(DYNAMITE_STATE eState) { m_eState = eState; }
	DYNAMITE_STATE GetState() { return m_eState; }

private:
	Engine::CTexture*	m_pTextureCom = nullptr;
	Engine::CCubeTex*	m_pBufferCom = nullptr;
	Engine::CTransform*	m_pTransCom = nullptr;
	Engine::CShader*	m_pShaderCom = nullptr;
	DYNAMITE_STATE m_eState = DYNAMITE_END;
	BOUNCE m_eBounce = THROWING;

	_bool m_bRotate = false;
	_float m_fTime = 0.f;
	_float m_fPower = 15.f;
	_float m_fAngle = 0.f;
	_bool m_bDead = false;
	_bool m_bExplosion = false;

	CCollisionCom* m_pColl = nullptr;
	void RotateToCursor();
	void Parabola(const _float & fTimeDelta);

	_bool m_bTwinkle = false;
	_float m_fFireShaderCount = 0.25f;
	_float m_fCurFireShaderCount = 1.f;
	_float m_fFreq = 1.f;

};

