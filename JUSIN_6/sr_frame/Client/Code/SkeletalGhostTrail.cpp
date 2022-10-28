#include "stdafx.h"
#include "..\Header\SkeletalGhostTrail.h"

class CStatComponent;

CSkeletalGhostTrail::CSkeletalGhostTrail(LPDIRECT3DDEVICE9 pGraphicDev) : CSkeletalCube(pGraphicDev)
{
}

CSkeletalGhostTrail::CSkeletalGhostTrail(const CSkeletalGhostTrail& rhs) : CSkeletalCube(rhs)
{
}

CSkeletalGhostTrail::~CSkeletalGhostTrail()
{
}

HRESULT CSkeletalGhostTrail::Ready_Object()
{
	CSkeletalCube::Ready_Object();
	m_pShaderCom = Add_Component<CShader>(L"Proto_GhostTrail", L"Proto_GhostTrail", ID_STATIC);

	return S_OK;
}

_int CSkeletalGhostTrail::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	m_fLife -= fTimeDelta;
	if (m_fLife < 0.f) return OBJ_DEAD;

	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CSkeletalGhostTrail::Render_Object()
{
	_matrix ViewMatrix, ProjMatrix;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	_float fS = (m_fInitLife - m_fLife) / m_fInitLife;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_matrix));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_matrix));
	m_pShaderCom->Set_RawValue("g_Rate", &fS, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_baseColor", &m_BaseColor, sizeof(D3DXCOLOR));

	m_bRenderMachine = false;

	m_pRootPart->matParents = m_pRootPart->pTrans->m_matWorld;

	// m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// // m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	// m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	// m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	// m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);


	for (const auto& child : m_pRootPart->vecChild)
	{
		RenderObjectRecur(child);

	}
	// m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	// m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CSkeletalGhostTrail::SetColorTime(_float fLife, D3DXCOLOR BaseColor)
{
	m_fLife = fLife;
	m_fInitLife = fLife;
	m_BaseColor = BaseColor;
}

CSkeletalGhostTrail* CSkeletalGhostTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev, CSkeletalCube* pSkeletal)
{
	NULL_CHECK_RETURN(pSkeletal, nullptr);

	auto pInst = new CSkeletalGhostTrail(pGraphicDev);
	if (FAILED(pInst->Ready_Object()))
	{
		Safe_Release(pInst);
		return nullptr;
	}
	
	pInst->LoadSkeletal(pSkeletal->GetCubePath());
	if (pInst->Get_SkeletalPart("weapon_r"))
		pInst->Get_SkeletalPart("weapon_r")->pBuf = nullptr;
	pInst->m_fAccTime = pSkeletal->GetCurFrameTime();
	pInst->m_pCurAnim = pSkeletal->GetCurAnimFrame();
	if (pInst->m_pCurAnim != nullptr)
		pInst->AnimFrameConsume(0.f);

	return pInst;
}
