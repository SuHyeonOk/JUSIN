#include "..\..\Header\DebugSphere.h"
#include "GameUtilMgr.h"
#include "Renderer.h"

CDebugSphere::CDebugSphere(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev)
{
}

CDebugSphere::CDebugSphere(const CDebugSphere& rhs): CGameObject(rhs)
{
}

CDebugSphere::~CDebugSphere()
{
}

HRESULT CDebugSphere::Ready_Object(const _vec3& vPos, _float fRadius)
{
	D3DXCreateSphere(m_pGraphicDev, fRadius, 10, 10, &m_pSphere, 0);
	CGameUtilMgr::MatWorldComposeEuler(m_matWorld, {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f}, vPos);

	return S_OK;
}

_int CDebugSphere::Update_Object(const _float& fTimeDelta)
{
	m_fTime -= fTimeDelta;

	if (m_fTime < 0.f)
		return OBJ_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);
	return OBJ_NOEVENT;
}

void CDebugSphere::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pSphere->DrawSubset(0);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CDebugSphere::Free()
{
	Safe_Release(m_pSphere);
	CGameObject::Free();
}

CDebugSphere* CDebugSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _float fRadius, _float fTime)
{
	auto pInst = new CDebugSphere(pGraphicDev);
	if (FAILED(pInst->Ready_Object(vPos, fRadius)))
	{
		MSG_BOX("Fail to Create DebugSphere");
		return nullptr;
	}

	pInst->m_fTime = fTime;

	return pInst;
}
