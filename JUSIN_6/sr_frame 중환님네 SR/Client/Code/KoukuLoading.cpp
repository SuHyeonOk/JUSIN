#include "stdafx.h"
#include "..\Header\KoukuLoading.h"
#include "AbstFactory.h"
#include "UI.h"

CKoukuLoading::CKoukuLoading(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}

CKoukuLoading::~CKoukuLoading()
{
}

// 주의사항 : 로딩 씬 만들때는  CScene::Ready_Scene() 실행하면 안됩니다.
// 또한 팩토리 사용 불가
HRESULT CKoukuLoading::Ready_Scene()
{
	m_pImage = CUIFactory::CreateNoLayer<CUI>("DefaultUI", 39, WINCX/2, WINCY/2, WINCX, WINCY);
	m_arrLayer[LAYER_UI]->Add_GameObject(L"LoadingUI", m_pImage);

	return S_OK;
}

_int CKoukuLoading::Update_Scene(const _float& fTimeDelta)
{
	CTransform* pTrans = m_pImage->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
	pTrans->m_vScale += fTimeDelta * _vec3{1.f, 1.f, 1.f} * 20.f;
	return CScene::Update_Scene(fTimeDelta);
}

void CKoukuLoading::LateUpdate_Scene()
{
	CScene::LateUpdate_Scene();
}

void CKoukuLoading::Render_Scene()
{
	CScene::Render_Scene();
}

CKoukuLoading* CKoukuLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKoukuLoading *	pInstance = new CKoukuLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CKoukuLoading::Free()
{
	CScene::Free();
}
