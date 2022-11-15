#include "..\..\Header\DebugLine.h"
#include "GameUtilMgr.h"
#include "Renderer.h"

CDebugLine::CDebugLine(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev)
{
}

CDebugLine::CDebugLine(const CDebugLine& rhs) : CGameObject(rhs)
{
}

CDebugLine::~CDebugLine()
{
}

HRESULT CDebugLine::Ready_Object(const _vec3& vOrigin, const _vec3& vDir, const _float fLength)
{
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_vVertexList[0] = vOrigin;
	m_vVertexList[1] = vOrigin + (vDir * fLength);
	return S_OK;
}


_int CDebugLine::Update_Object(const _float& fTimeDelta)
{
	m_fTime -= fTimeDelta;

	if (m_fTime < 0.f)
		return OBJ_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return OBJ_NOEVENT;
}

void CDebugLine::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &CGameUtilMgr::s_matIdentity);
	m_pLine->SetWidth(10.0f);
	_matrix view, proj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &proj);
	_matrix matFinal = view * proj;

	// m_pLine->Begin();
	m_pLine->DrawTransform(m_vVertexList, 2, &matFinal, 0xff408010);
	// m_pLine->End();
}

void CDebugLine::Free()
{
	Safe_Release(m_pLine);
	CGameObject::Free();
}

CDebugLine* CDebugLine::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vOrigin, const _vec3& vDir,
	const _float fLength, _float fTime)
{
	auto pInst = new CDebugLine(pGraphicDev);
	if (FAILED(pInst->Ready_Object(vOrigin, vDir, fLength)))
	{
		MSG_BOX("Fail to Create DebugLine");
		return nullptr;
	}

	pInst->m_fTime = fTime;

	return pInst;
}

