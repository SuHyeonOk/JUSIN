#pragma once

#include "Base.h"
#include "Engine_Include.h"
#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene :	public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene();

public:
	CComponent*				Get_Component(LAYERID eLayerID, const wstring& pObjTag, const wstring& pComponentTag, COMPONENTID eID);
	CGameObject* Get_GameObject(LAYERID eLayerID, const wstring& pObjTag);
	void Get_AllGameObject(LAYERID eLayerID, const wstring& pObjTag, list<CGameObject*>& outList);
	CLayer* Get_Layer(LAYERID eLayerID);
	void AddGameObject(LAYERID eLayerID, const wstring& pObjTag, CGameObject* pObject);
public:	
	virtual		HRESULT		Ready_Scene(void);
	virtual		_int Update_Scene(const _float& fTimeDelta);
	virtual		void LateUpdate_Scene(void);
	virtual		void Render_Scene(void) {}

protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev;
	// map<wstring, CLayer*>		m_mapLayer;
	array<CLayer*, LAYER_END>    m_arrLayer;

public:
	virtual void	Free(void);
};

END