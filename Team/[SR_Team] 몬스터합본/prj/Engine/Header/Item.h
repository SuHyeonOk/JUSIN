#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

// 모든 아이템들이 가진 공통적인 특성
class ENGINE_DLL CItem	: public CGameObject
{

	enum ITEM_STATE { STATE_STAGE, STATE_INV, STATE_USE, STATE_END };

protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {};
	virtual ~CItem() {};

public:
	virtual HRESULT Ready_Object(void) { return S_OK; }
	virtual _int	Update_Object(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Object(void) {}
	virtual	void	Render_Obejct(void) {}

protected:
	virtual void	Free(void) {};

protected:
	CRcTex*			m_pBufferCom	= nullptr;
	CTexture*		m_pTextureCom	= nullptr;
	CTransform*		m_pTransCom		= nullptr;

	ITEM_STATE		m_eItemState = STATE_END;
};

END
