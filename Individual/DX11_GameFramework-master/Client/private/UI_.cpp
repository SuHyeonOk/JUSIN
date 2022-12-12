#include "stdafx.h"
#include "..\public\UI_.h"

#include "GameInstance.h"

CUI_::CUI_(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CUI_::CUI_(const CUI_ & rhs)
	: CGameObject(rhs)
{

}

HRESULT CUI_::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_::Initialize(void * pArg)
{
	UIDESC		UIDesc;
	ZeroMemory(&UIDesc, sizeof(UIDESC));

	if (nullptr != pArg)
		memcpy(&UIDesc, pArg, sizeof(UIDESC));

	if (FAILED(__super::Initialize(&UIDesc)))
		return E_FAIL;
	
	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CUI_::Tick(const _double& TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CUI_::Late_Tick(const _double& TimeDelta)
{
	__super::Late_Tick(TimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
