#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTexture;
class CCubeTex;
class CTransform;

END

class CBullet :	public CGameObject
{
private:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBullet();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	HRESULT				Add_Component();
	void				MoveBullet(const _float & fTimeDelta);

private:
	CCubeTex*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

private:
	_vec3				m_vDirection;

	float				m_fSpeed;


public:
	static CBullet*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free();
};

