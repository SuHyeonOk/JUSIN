#pragma once

#include "Base.h"
#include "Scene.h"
#include "Engine_Include.h"
#include <atomic>
#include <mutex>

BEGIN(Engine)

class ENGINE_DLL CManagement :	public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual ~CManagement();

public:
	CComponent* Get_Component(LAYERID eLayerID, const wstring& pObjTag, const wstring& pComponentTag, COMPONENTID eID);
	CGameObject* Get_GameObject(LAYERID eLayerID, const wstring& pObjTag);
	void Get_AllGameObject(LAYERID eLayerID, const wstring& pObjTag, list<CGameObject*>& outList);
	CLayer* Get_Layer(LAYERID eLayerID);
	void AddGameObject(LAYERID eLayerID, const wstring& pObjTag, CGameObject* pObject);
public:
	void SwitchSceneLoading(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay = 700/*0.7초*/);
	void SwitchSceneLoadingDeletePrev(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay = 700/*0.7초*/);

	// 현재 씬은 보존하고 입력받은 씬으로 이동하는 함수
	HRESULT		Set_Scene(CScene* pScene);
	// 현재씬은 제거하고 이전 씬으로 돌아가는 함수
	HRESULT     Go_PrevScene();
	// 보존한 이전 씬을 모두 제거하는 함수
	void        Clear_PrevScene();
	_int		Update_Scene(const _float& fTimeDelta);
	void		LateUpdate_Scene(void);
	void		Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

	CScene* GetScene() { return m_pScene; }
	
private:
	CScene*		m_pScene = nullptr;
	CScene* m_pLoadingScene = nullptr;
	vector<CScene*> m_vecScene;
	std::atomic<bool> m_bLoading{false};
	std::mutex m_mtx;

public:
	virtual void Free(void);
};

END
