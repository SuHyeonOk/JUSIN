#pragma once
#include "GameObject.h"

class CSatonCircle :public CGameObject
{
private:
	explicit CSatonCircle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSatonCircle(const CSatonCircle& rhs);
	virtual ~CSatonCircle() override;

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	static CSatonCircle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _IsRed);

private:
	Engine::CTransform* m_pTransform = nullptr;
	CTexture* m_pTexture = nullptr;
	CCubeTex* m_pBuffer = nullptr;

	_bool m_bIsRed = false;

	_bool m_bSetY = false;
	_float m_fLifeTime = 0.f;
	_float m_fSpeed = 0.f;

	_bool m_bDead = false;
	DWORD m_dwMakeCircleCoolTime;
};

