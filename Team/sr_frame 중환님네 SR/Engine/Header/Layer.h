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
	CComponent*				Get_Component(const wstring& pObjTag, const wstring& pComponentTag, COMPONENTID eID);
	CGameObject* Get_GameObject(const wstring& pObjTag);
	void Get_AllGameObject(const wstring& pObjTag, list<CGameObject*>& outList);
	const unordered_multimap<wstring, CGameObject*>& Get_MapObject() const { return m_mapObject; }

public:
	HRESULT			Add_GameObject(const wstring& pObjTag, CGameObject* pInstance);
	HRESULT			Ready_Layer(void);
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(void);
	HRESULT			Delete_GameObject(const wstring& pObjTag);

private:
	unordered_multimap<wstring, CGameObject*>			m_mapObject;

public:
	static CLayer*		Create(void);
	virtual void		Free(void);

};

END