#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

class ENGINE_DLL CNPC : public CGameObject
{
public:
	explicit CNPC(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNPC();

public:
	virtual HRESULT		Ready_Object(void)						{ return S_OK; };
	virtual _int		Update_Object(const _float& fTimeDelta) { return 0; };
	virtual void		LateUpdate_Object(void)					{};
	virtual void		Render_Obejct(void)						{};

protected:
	virtual HRESULT		Add_Component(void) PURE;
	virtual void		Billboard();
	virtual void		OnText(const _float& fTimeDelta) {};


public:
	virtual void		Free(void);

protected:
	CRcTex*				m_pBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransCom = nullptr; 
	CAnimator*			m_pAnimtorCom = nullptr;
};

END