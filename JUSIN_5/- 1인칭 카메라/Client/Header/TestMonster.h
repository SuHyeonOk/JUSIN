#pragma once
#include "GameObject.h"

namespace Engine
{
	class CTransform;
	class CTriCol;
}

class CTestMonster : public CGameObject
{
private:
	explicit CTestMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestMonster();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Obejct(void) override;

private:
	HRESULT				Add_Component(void);

private:
	CTriCol*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	_vec3				m_vDirection;
	_float				m_fTimeDelta = 0.f;

public:
	static CTestMonster*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);
};

