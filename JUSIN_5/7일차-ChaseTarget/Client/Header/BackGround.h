#pragma once

#include "Engine_Include.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcCol;
class CTransform;

END
class CBackGround :	public Engine::CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBackGround();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Obejct(void) override;


private:
	HRESULT				Add_Component(void);

private:
	CRcCol*				m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;

public:
	static CBackGround*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:

	virtual void Free(void);


};

