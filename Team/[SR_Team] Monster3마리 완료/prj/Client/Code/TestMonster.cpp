#include "stdafx.h"
#include "..\Header\TestMonster.h"
#include "Export_Function.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
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
	// 왜 LateUpdate에서 ??
	// 1) 업데이트가 다 끝난뒤에 플레이어를 참조하기 위해
	// 2) 맵 컨테이너의 키: 문자열상수(주소)=> 알파벳순으로 정렬되지 않음=>못찾을 수도 있음
	//									=> 업데이트에서 모든 걸 다 넣은 뒤 참조하기 위해
	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_Environment", L"TestPlayer", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3		vPlayerPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

	m_pTransCom->Chase_Target(&vPlayerPos, 5.f, m_fTimeDelta);

	Engine::CGameObject::LateUpdate_Object();
}

void CTestMonster::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pBufferCom->Render_Buffer();
}

HRESULT CTestMonster::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CTriCol*>(Clone_Proto(L"Proto_TriColCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TriColCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TransformCom", pComponent }); 
	// trasform 클래스를 참조하면, 행렬변환을 두번 하게 되는 것을 확인할 수 있다.
	// 때문에 ID_STATIC으로 만들어 update 연산에서 제외시키는 꼼수를 썼다.

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
