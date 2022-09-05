#pragma once


#include "Engine_Include.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer : public CComponent
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public: // CRenderer
	void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Clear_RenderGroup(void);


private: // CRenderer
	list<CGameObject*>			m_RenderGroup[RENDER_END];

private: // CRenderer
	virtual CComponent*	Clone(void) { return nullptr; }
	virtual void Free(void);
};
END