#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent*	Get_ComponentPtr(const _tchar* pComponentTag, _uint iLayerIndex); // ��
	class CGameObject*	Get_GameObject(const _tchar* pGameObjectTag, _uint iLayerIndex); // ��

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Initialize();
	void	Tick(_double TimeDelta);
	void	Late_Tick(_double TimeDelta);
	
	list<class CGameObject*>& GetGameObjects() { return m_GameObjects; }

private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;

};

END