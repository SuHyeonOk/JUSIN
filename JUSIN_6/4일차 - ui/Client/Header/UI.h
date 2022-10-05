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

private:
	_matrix				m_ProjMatrix; // 직교 투영 행렬
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY; // UI의 크기조절을 위한 변수

public:
	static CUI*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);
};

