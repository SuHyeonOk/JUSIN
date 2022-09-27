#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END
class CFontLogo : public CGameObject
{
private:
	explicit CFontLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFontLogo();

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

public:
	static CFontLogo*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};


