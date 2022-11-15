#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTexture;
class CCubeTex;
class CTransform;

END

class CTestCube : public CGameObject
{
private:
	explicit CTestCube(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTestCube(const CGameObject& rhs);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT				Add_Component(void);

public:
	CCubeTex*		m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

public:
	static CTestCube*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

