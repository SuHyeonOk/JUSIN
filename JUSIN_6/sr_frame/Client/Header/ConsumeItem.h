#pragma once
#include "Item.h"
class CConsumeItem :
	public CItem
{
protected:
	explicit CConsumeItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CConsumeItem();

public:
	//gameobj function
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	//consume function
	ITEMEFFECT GetItemEffect() { return m_eItemEffect; }
	void	ItemDelete() { m_bDead = true; }
protected:
	CTexture*	m_pTextureCom = nullptr;
	CRcTex*		m_pBufferCom = nullptr;
	ITEMEFFECT	m_eItemEffect;
	_bool		m_bDead;
};

