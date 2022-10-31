#pragma once
#include "GameObject.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;	
}

class CUI :
	public CGameObject
{
private:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Obejct(void) override;

private:
	HRESULT				Add_Component(void);	

private:
	CRcTex*				m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;	
	CTexture*			m_pTextureCom = nullptr;	

private: /* 직교투영행렬. */
	_matrix				m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;

public:
	static CUI*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);
};

