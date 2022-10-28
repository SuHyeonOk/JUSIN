#include "..\..\Header\Scene.h"
#include "Management.h"
USING(Engine)

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
	for (auto& e : m_arrLayer)
		e = CLayer::Create();
}


CScene::~CScene()
{
}

CComponent * CScene::Get_Component(LAYERID eLayerID, const wstring& pObjTag, const wstring& pComponentTag, COMPONENTID eID)
{
	return m_arrLayer[eLayerID]->Get_Component(pObjTag, pComponentTag, eID);
}

CGameObject* CScene::Get_GameObject(LAYERID eLayerID, const wstring& pObjTag)
{
	return m_arrLayer[eLayerID]->Get_GameObject(pObjTag);
}

void CScene::Get_AllGameObject(LAYERID eLayerID, const wstring& pObjTag, list<CGameObject*>& outList)
{
	m_arrLayer[eLayerID]->Get_AllGameObject(pObjTag, outList);
}

CLayer* CScene::Get_Layer(LAYERID eLayerID)
{
	return m_arrLayer[eLayerID];
}

void CScene::AddGameObject(LAYERID eLayerID, const wstring& pObjTag, CGameObject* pObject)
{
	if (pObject == nullptr)
		return;

	m_arrLayer[eLayerID]->Add_GameObject(pObjTag, pObject);
}

HRESULT CScene::Ready_Scene(void)
{
	CManagement::GetInstance()->Set_Scene(this);

	return S_OK;
}

_int CScene::Update_Scene(const _float & fTimeDelta)
{
	_int iResult = 0;

	for (auto& layer : m_arrLayer)
	{
		iResult = layer->Update_Layer(fTimeDelta);
		if (iResult & 0x80000000)
			return iResult;
	}
	
	return iResult;
}

void CScene::LateUpdate_Scene(void)
{
	for (auto& layer : m_arrLayer)
	{
		layer->LateUpdate_Layer();
	}
}

void Engine::CScene::Free(void)
{
	for (auto& i : m_arrLayer)
		Safe_Release(i);

	Safe_Release(m_pGraphicDev);
}
