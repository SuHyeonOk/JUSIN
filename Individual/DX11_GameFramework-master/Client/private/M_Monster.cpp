#include "stdafx.h"
#include "..\public\M_Monster.h"

#include "GameInstance.h"

CM_Monster::CM_Monster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CM_Monster::CM_Monster(const CM_Monster & rhs)
	: CGameObject(rhs)
{

}

HRESULT CM_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Monster::Initialize(void * pArg)
{
	MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MONSTERDESC));

	if (FAILED(__super::Initialize(&MonsterDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CM_Monster::Tick(const _double& TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CM_Monster::Late_Tick(const _double& TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Monster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CM_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
