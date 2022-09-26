#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTransform;
class CTexture;

class ENGINE_DLL CBullet : public  CGameObject
{
protected:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet();

public:
	virtual HRESULT Ready_Object(void) { return S_OK; }
	virtual _int	Update_Object(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Object(void) {}
	virtual	void	Render_Obejct(void) {}

protected:
	virtual void	Reset() {};  //풀로 돌아간 오브젝트를 재사용하기 위해 기존 정보를 리셋한다.
	virtual void	Free(void);

public:
	void	Set_Fire(_bool _bFire) { m_bFire = _bFire; m_bDead = false; }


protected:
	CCubeTex*		m_pBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransCom = nullptr;
	_bool			m_bFire = false;
	_bool			m_bDead = false;
	_float			m_fLifeTime = 0.f;
};
END

