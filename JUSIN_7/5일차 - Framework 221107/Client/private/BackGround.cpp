#include "stdafx.h"
#include "..\public\BackGround.h"

CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CBackGround::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBackGround::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBackGround*		pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBackGround::Clone(void * pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

}
