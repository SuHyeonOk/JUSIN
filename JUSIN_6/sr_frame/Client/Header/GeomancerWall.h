#pragma once
#include "GameObject.h"

class CGeomancerWall : public CGameObject
{
private:
	explicit CGeomancerWall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGeomancerWall(const CGeomancerWall& rhs);
	virtual ~CGeomancerWall() override;

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	static CGeomancerWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bBomb);

private:
	CTransform* m_pTransform = nullptr;
	CTexture* m_pTexture = nullptr;
	CCubeTex* m_pBuffer = nullptr;
	CShader* m_pShaderCom = nullptr;

	_bool m_bBomb = false;

	// 153 : normal, 154 : bomb
	_uint m_iTexNum = 0;

	_bool m_bSetY = false;
	_float m_fInitY = 0.f;
	_float m_fFinishY = 0.f;

	_float m_fLifeTime = 0.f;
	_float m_fSpeed = 0.f;

	_bool m_bFire = false;
	_bool m_bDead = false;
	
};

