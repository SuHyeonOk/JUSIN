#include "stdafx.h"
#include "..\Header\TestMonster.h"

#include "Export_Function.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}


CTestMonster::~CTestMonster()
{
}

HRESULT CTestMonster::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTestMonster::Update_Object(const _float & fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	_int iResult = Engine::CGameObject::Update_Object(fTimeDelta);
	
	Add_RenderGroup(RENDER_NONALPHA, this);

	return iResult;
}

void CTestMonster::LateUpdate_Object(void)
{
	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Ready_Layer_Environment", L"TestPlayer", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3		vPlayerPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

	m_pTransCom->Chase_Target(&vPlayerPos, 5.f, m_fTimeDelta);

	Engine::CGameObject::LateUpdate_Object();
}

void CTestMonster::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pBufferCom->Render_Buffer();
}

HRESULT CTestMonster::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CTriCol*>(Clone_Proto(L"Proto_TriColCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TriColCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TransformCom", pComponent });


	return S_OK;
}

CTestMonster * CTestMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestMonster *	pInstance = new CTestMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTestMonster::Free(void)
{
	CGameObject::Free();
}
