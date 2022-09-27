#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END
class CFontStart : public CGameObject
{
private:
	explicit CFontStart(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFontStart();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	 LateUpdate_Object(void) override;
	virtual void	Render_Obejct(void) override;

private:
	HRESULT		Add_Component(void);

private:
	CRcTex*			m_pBufferCom = nullptr;		
	CTexture*		m_pTextureCom = nullptr;	
	CTransform*		m_pTransCom = nullptr;

private:
	// Timer
	_float				m_fTimeTrueAcc = 0.f;
	_float				m_fTimeFalseAcc = 0.f;

	_bool				m_bRender = false;

public:
	static CFontStart*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};


