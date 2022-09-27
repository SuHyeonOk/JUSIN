#pragma once
#include "Engine_Include.h"
#include "GameObject.h"

BEGIN(Engine)

// 싱글톤(렌더링이라는 기능) + 컴포넌트(오브젝트에 포함되기 위함)
class ENGINE_DLL CRenderer : public CComponent
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public:
	void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Clear_RenderGroup(void);

	void		Set_RenderState(LPDIRECT3DDEVICE9& pGraphicDev, _int _i);


private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];

private:
	virtual CComponent*		Clone(void) { return nullptr; } // 싱글톤은 복제되면 안되니까 깡통
	virtual	void			Free(void);
};

END
