#pragma once
#include "NPC.h"

class CSmileMan : public CNPC
{
private:
	explicit CSmileMan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSmileMan();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void) override;
	void				OnText(const _float& fTimeDelta);

public:
	static CSmileMan*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	vector<wstring>		m_vecDialogue;
	_int iCount = 0;
	_bool		m_bTalk = false;

private:
	// Timer
	_float				m_fTimeAcc;
	_float				m_fLBClick = 0.f; // TODO 키 입력 한 번 받을 수 있게 되면 삭제
};


