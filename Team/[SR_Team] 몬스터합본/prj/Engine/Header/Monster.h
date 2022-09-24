#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

class ENGINE_DLL CMonster : public CGameObject
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual HRESULT		Ready_Object(void)						override { return S_OK; };
	virtual _int		Update_Object(const _float& fTimeDelta) override { return 0; };
	virtual void		LateUpdate_Object(void)					override {};
	virtual void		Render_Obejct(void)						override {};

protected:
	virtual HRESULT		Add_Component(void) PURE;

protected:
	_float				m_fFrame;
	_float				m_fSpeed;

public:
	virtual void		Free(void);

protected:
	CRcTex*				m_pBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
};

END