#pragma once
#include "Monster.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
}

class CGreenSlime : public CMonster
{
	enum STATE { IDLE, ATTACK, HIT, DIE, MOTION_END };

private:
	explicit CGreenSlime(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGreenSlime();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	STATE		m_ePreState;
	STATE		m_eCurState;
	void		Motion_Change(const _float& fTimeDelta);


	virtual HRESULT		Add_Component(void);

	void				Target_Follow(const _float& fTimeDelta);
	void				Billboard();

private:
	CRcTex*				m_pBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransCom = nullptr;

private:


public:
	static CGreenSlime*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};


