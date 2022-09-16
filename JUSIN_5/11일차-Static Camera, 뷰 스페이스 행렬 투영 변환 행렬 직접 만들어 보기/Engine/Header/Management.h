#pragma once

#include "Base.h"
#include "Scene.h"
#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CManagement :	public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual ~CManagement();

public:
	CComponent*				Get_Component(const _tchar* pLayerTag, 
		const _tchar* pObjTag, 
		const _tchar* pComponentTag,
		COMPONENTID eID);

public:
	HRESULT		Set_Scene(CScene* pScene);
	_int		Update_Scene(const _float& fTimeDelta);
	void		LateUpdate_Scene(void);
	void		Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
	
private:
	CScene*		m_pScene;

public:
	virtual void Free(void);
};
END
