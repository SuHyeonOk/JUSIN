#pragma once

#include "GameObject.h"
#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	CComponent*				Get_Component(const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);

public:
	HRESULT			Add_GameObject(const _tchar* pObjTag, CGameObject* pInstance);
	HRESULT			Ready_Layer(void);
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(void);

private:
	map<const _tchar*, CGameObject*>			m_mapObject;

public:
	static CLayer*		Create(void);
	virtual void		Free(void);

};

END