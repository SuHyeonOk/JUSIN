#pragma once
#include "GameObject.h"



namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CEffect : public CGameObject
{
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Obejct(void) override;

private:
	HRESULT				Add_Component(void);

private:
	CRcTex*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

	_float				m_fFrame = 0.f;

public:
	static CEffect*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);
};

