#include "stdafx.h"
#include "..\Header\SkyBox.h"

#include "Export_Function.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CSkyBox::~CSkyBox()
{
}

HRESULT CSkyBox::Ready_Object(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CShereBuffer*>(Clone_Proto(L"Proto_SkySphere"));
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SkySphere", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_SkySphereTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SkySphereTexture", pComponent });

	m_pShader = Add_Component<CShader>(L"Proto_SkySphereShaderCom", L"Proto_SkySphereShaderCom", ID_STATIC);

	return S_OK;
}

_int CSkyBox::Update_Object(const _float & fTimeDelta)
{
	Add_RenderGroup(RENDER_PRIORITY, this);
	return 0;
}

void CSkyBox::Render_Object(void)
{
	_matrix ViewMatrix, ProjMatrix;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShader->Set_RawValue("g_WorldMatrix", &CGameUtilMgr::s_matIdentity, sizeof(_matrix));
	m_pShader->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_matrix));
	m_pShader->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_matrix));
	m_pTextureCom->Set_Texture(m_pShader, "g_DefaultTexture", m_iTexNum);

	m_pShader->Begin_Shader(0);
	m_pBuffer->Render_Buffer();
	m_pShader->End_Shader();
}

CSkyBox * CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox*	pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSkyBox::Free(void)
{
	CGameObject::Free();
}


// AX + BY + CZ + D = 0