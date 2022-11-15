#include "Export_Utility.h"
#include <thread>
#include <chrono>

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pScene(nullptr), m_pLoadingScene(nullptr)
{
}


CManagement::~CManagement()
{
	Free();
}

CComponent* Engine::CManagement::Get_Component(LAYERID eLayerID, const wstring& pObjTag, const wstring& pComponentTag, COMPONENTID eID)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(eLayerID, pObjTag, pComponentTag, eID);
}

CGameObject* CManagement::Get_GameObject(LAYERID eLayerID, const wstring& pObjTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_GameObject(eLayerID, pObjTag);
}

void CManagement::Get_AllGameObject(LAYERID eLayerID, const wstring& pObjTag, list<CGameObject*>& outList)
{
	if (nullptr == m_pScene)
		return;

	m_pScene->Get_AllGameObject(eLayerID, pObjTag, outList);
}

CLayer* CManagement::Get_Layer(LAYERID eLayerID)
{
	NULL_CHECK_RETURN(m_pScene, nullptr);
	return m_pScene->Get_Layer(eLayerID);
}

void CManagement::AddGameObject(LAYERID eLayerID, const wstring& pObjTag, CGameObject* pObject)
{
	if (pObject == nullptr)
		return;

	NULL_CHECK(m_pScene);
	m_mtx.lock();
	m_pScene->AddGameObject(eLayerID, pObjTag, pObject);
	m_mtx.unlock();
}

void CManagement::SwitchSceneLoading(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay)
{
	Safe_Release(m_pLoadingScene);
	m_pLoadingScene = pLoading;
	m_bLoading = true;

	std::thread threadLoading([this, pSceneCreate, delay]()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		pSceneCreate();
		this->m_bLoading = false;
		return;
	});
	threadLoading.detach();
}

void CManagement::SwitchSceneLoadingDeletePrev(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay)
{
	SwitchSceneLoading(pLoading, pSceneCreate, delay);
	Clear_PrevScene();
}

HRESULT CManagement::Set_Scene(CScene * pScene)
{
	if (m_pScene != nullptr)
		m_vecScene.push_back(m_pScene);
	Engine::Clear_RenderGroup(); // 기존 scene에 그려지고 있던 모든 렌더 요소들을 삭제
	CCollider::GetInstance()->Clear_ColliderAll();

	m_pScene = pScene;

	return S_OK;
}

HRESULT CManagement::Go_PrevScene()
{
	if (m_vecScene.empty())
	{
		_CRASH("No Scene to go back");
		return E_FAIL;
	}

	Safe_Release(m_pScene);
	m_pScene = m_vecScene.back();
	m_vecScene.pop_back();
	return S_OK;
}

void CManagement::Clear_PrevScene()
{
	for (auto& scene : m_vecScene)
		Safe_Release(scene);
	m_vecScene.clear();
}

_int CManagement::Update_Scene(const _float & fTimeDelta)
{
	if (m_bLoading)
	{
		return m_pLoadingScene->Update_Scene(fTimeDelta);
	}

	if (nullptr == m_pScene)
		return -1;

	return m_pScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene(void)
{
	if (m_bLoading)
		return;

	if (nullptr == m_pScene)
		return;

	m_pScene->LateUpdate_Scene();
	CCollider::GetInstance()->Check_Blocking();
	CCollider::GetInstance()->Clear_Dynamic();
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::Render_GameObject(pGraphicDev);

	if (nullptr == m_pScene)
		return;

	m_pScene->Render_Scene();
}

void Engine::CManagement::Free(void)
{
	Safe_Release(m_pLoadingScene);
	Safe_Release(m_pScene);
	for (auto& scene : m_vecScene)
		Safe_Release(scene);
	m_vecScene.clear();
}

