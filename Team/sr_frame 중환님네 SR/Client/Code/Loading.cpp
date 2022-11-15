#include "stdafx.h"
#include "..\Header\Loading.h"
#include "AbstFactory.h"
#include "UI.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}

CLoading::~CLoading()
{
}

// 주의사항 : 로딩 씬 만들때는  CScene::Ready_Scene() 실행하면 안됩니다.
// 또한 팩토리 사용 불가
HRESULT CLoading::Ready_Scene()
{
	m_pImage = CUIFactory::CreateNoLayer<CUI>("DefaultUI", 14, WINCX/2, WINCY/2, WINCX, WINCY);
	m_arrLayer[LAYER_UI]->Add_GameObject(L"LoadingUI", m_pImage);

	return S_OK;
}

_int CLoading::Update_Scene(const _float& fTimeDelta)
{
	CTransform* pTrans = m_pImage->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
	pTrans->m_vScale += fTimeDelta * _vec3{1.f, 1.f, 1.f} * 20.f;
	return CScene::Update_Scene(fTimeDelta);
}

void CLoading::LateUpdate_Scene()
{
	CScene::LateUpdate_Scene();
}

void CLoading::Render_Scene()
{
	CScene::Render_Scene();
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoading *	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free()
{
	CScene::Free();
}
