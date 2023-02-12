#include "stdafx.h"
#include "..\public\Snail.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"
#include <fstream>

CSnail::CSnail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CSnail::CSnail(const CSnail & rhs)
	: CGameObject(rhs)
{

}

HRESULT CSnail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSnail::Initialize(void * pArg)
{	
	m_wsTag = L"Snail";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Load_Position();

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CSnail::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	UI_Tick(TimeDelta);
	NextPosition(TimeDelta);
}

void CSnail::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_XRAYBLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CSnail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderCom, 0, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderCom, 0, "g_BoneMatrices", 0);

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

HRESULT CSnail::Render_XRay()
{
	if (FAILED(__super::Render_XRay()))
		return E_FAIL;

	if (nullptr == m_pShaderXRayCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderXRayCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float	fObjectID = 3.0f;
	if (FAILED(m_pShaderXRayCom->Set_RawValue("g_ObjectID", &fObjectID, sizeof _float)))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderXRayCom, 0, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderXRayCom, 0, "g_BoneMatrices", 0);

	return S_OK;
}

void CSnail::On_Collision(CGameObject * pOther)
{

}

HRESULT CSnail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_ShaderXRay */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel_XRay"), TEXT("Com_ShaderXRay"),
		(CComponent**)&m_pShaderXRayCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Snail"), TEXT("Com_Model"),
	(CComponent**)&m_pModelCom)))
	return E_FAIL;
	
	CCollider::COLLIDERDESC			ColliderDesc;
	
	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
	(CComponent**)&m_pColliderCom, &ColliderDesc)))
	return E_FAIL;
	
	return S_OK;
}

HRESULT CSnail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CSnail::Load_Position()
{
	wifstream		fin("../../Data/Snail_Position.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
	}

	_tchar szObjPosX[MAX_PATH] = L"";
	_tchar szObjPosY[MAX_PATH] = L"";
	_tchar szObjPosZ[MAX_PATH] = L"";

	_float	fObjPosX = 0.0f;
	_float	fObjPosY = 0.0f;
	_float	fObjPosZ = 0.0f;

	while (true)
	{
		fin.getline(szObjPosX, MAX_PATH, '|');
		fin.getline(szObjPosY, MAX_PATH, '|');
		fin.getline(szObjPosZ, MAX_PATH);

		if (fin.eof())
			break;

		fObjPosX = (_float)_tstof(szObjPosX);
		fObjPosY = (_float)_tstof(szObjPosY);
		fObjPosZ = (_float)_tstof(szObjPosZ);

		m_vecNextPosition.push_back(_float3(fObjPosX, fObjPosY, fObjPosZ));
	}

	return S_OK;
}

void CSnail::NextPosition(const _double & TimeDelta)
{
	if (true == m_bArrive)
		return;

	_vector vNextPosition = XMVectorSet(m_vecNextPosition[m_iIndex].x, m_vecNextPosition[m_iIndex].y, m_vecNextPosition[m_iIndex].z, 1.0f);
	m_pTransformCom->LookAt(vNextPosition);

	// 내 거리와 목표 지점의 거리를 구해서 목표지점에 도달하면 그 때 i를 증가 시킨다.
	if (true == m_pTransformCom->TargetPoint(vNextPosition, TimeDelta))
	{
		++m_iIndex;                 
	}

	if (m_iIndex > m_vecNextPosition.size() - 1)
	{
		m_bArrive = true;
	}
}

void CSnail::UI_Tick(const _double & TimeDelta)
{
	if (false == m_bArrive)
	{
		m_bMove_TimeAcc += TimeDelta;
	}
	else
	{
		m_bMove_TimeAcc;
		_int a = 0;
	}

	// TODO : 달팽이 이동 거리 구하기
	//_float fTemp = (_float(m_iIndex + 1) / 21.0f) * 100.0f;
	//if (m_fCurrent_Percentage < fTemp)
	//{
	//	m_fCurrent_Percentage = _float(TimeDelta) * 2.0f;
	//}

	//cout << m_fCurrent_Percentage << " \ " << (_float(m_iIndex + 1) / 21.0f) * 100.0f << endl;

	CUI_Manager::GetInstance()->Set_Snail_Distance((_int(m_iIndex / 21) * 100));
}

CSnail * CSnail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSnail*		pInstance = new CSnail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSnail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSnail::Clone(void * pArg)
{
	CSnail*		pInstance = new CSnail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSnail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSnail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderXRayCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
