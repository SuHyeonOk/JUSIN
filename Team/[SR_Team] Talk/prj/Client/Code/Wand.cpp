#include "stdafx.h"
#include "..\Header\Wand.h"
#include "Export_Function.h"

CWand::CWand(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev)
{
}

CWand::~CWand()
{
}

HRESULT CWand::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Scale(0.3f, 0.3f, 0.3f);
	m_pTransCom->Set_Pos(1.f, 2.f, 3.f);

	//NULL_CHECK(m_pTarget);

	return S_OK;
}

HRESULT CWand::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// 버퍼 컴포넌트
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	// 텍스쳐 컴객체 컴포넌트
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_WandTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_WandTexture", pComponent });

	// 월드행렬 컴포넌트
	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	return S_OK;

}


_int CWand::Update_Object(const _float & fTimeDelta)
{
	if (!m_bReady)
	{
		m_pCamera = dynamic_cast<CCamera*>(Engine::Get_GameObject(L"Layer_Environment", L"StaticCamera"));
		if (nullptr == m_pCamera)
			return -1;
		else
		{
			m_pCenter = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
			if (nullptr == m_pCenter)
				return -1;
			else m_bReady = true;
		}
	}

	//_vec3	vPos, vRight, vLook;
	//
	//m_pCenter->Get_Info(INFO_POS, &vPos);
	//vPos.y -= 0.4f;

	//m_pCenter->Get_Info(INFO_RIGHT, &vRight);
	//D3DXVec3Normalize(&vRight, &vRight);
	//vPos += 0.5f * vRight;

	//m_pCenter->Get_Info(INFO_LOOK, &vLook);
	//D3DXVec3Normalize(&vLook, &vLook);
	//vPos += 1.3f * vLook;

	//m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	// 1
	//_vec3 vAt;
	//m_pCamera->Get_At(vAt);
	//m_pTransCom->Set_Pos(vAt.x, vAt.y, vAt.z );


	//m_pTarget->Get_Info(INFO_POS, &vPos);
 //	vPos.y -= 0.5f;

	//m_pTarget->Get_Info(INFO_RIGHT, &vRight);
	//D3DXVec3Normalize(&vRight, &vRight);
	//vPos += 0.5f * vRight;

	//m_pTarget->Get_Info(INFO_LOOK, &vLook);
	//D3DXVec3Normalize(&vLook, &vLook);
	//vPos += 1.3f * vLook;

	//m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	int iResult = CGameObject::Update_Object(fTimeDelta);

	


	Add_RenderGroup(RENDER_ALPHA, this);
	return iResult;
}

void CWand::LateUpdate_Object(void)
{
	_matrix		matWorld, matView, matBill;
	D3DXMatrixIdentity(&matBill);

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;


	matBill._22 = matView._22;
	matBill._23 = matView._23;
	matBill._32 = matView._32;
	matBill._33 = matView._33;

	// 현재 지금 이 코드는 문제가 없지만 나중에 문제가 될 수 있음
	D3DXMatrixInverse(&matBill, 0, &matBill);
	m_pTransCom->Set_WorldMatrix(&(matBill * matWorld));

	CGameObject::LateUpdate_Object();
}

void CWand::Render_Obejct(void)
{
	CGameObject* pObj = Engine::Get_GameObject(L"Layer_Environment", L"StaticCamera");
	_matrix matView, matProj;
	static_cast<CCamera*>(pObj)->Get_ViewMatrix(matView);
	static_cast<CCamera*>(pObj)->Get_ProjectMatrix(matProj);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(_ulong(m_fFrame));

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

CWand * CWand::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWand*		pInstance = new CWand(pGraphicDev);
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWand::Free(void)
{
	CGameObject::Free();
}

void CWand::LevelUp()
{
}

void CWand::Attack()
{
}


