#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"

#include <fstream> // @
#include "Imgui_PropertyEditor.h"	// @
#include "DataManager.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	m_wsTag = L"Terrain";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		_float4		vPickPos;
		if (m_pVIBufferCom->Picking(m_pTransformCom, &vPickPos))
			cout << "PickPos : " << vPickPos.x << "|" << vPickPos.y << "|" << vPickPos.z << endl;
	}

	RELEASE_INSTANCE(CGameInstance);

	ImGui_Navigation();
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
  	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
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

	return S_OK;
}

void CTerrain::ImGui_Navigation()
{
	ImGui::Begin("NavTool");

	// 마우스 자표 얻어오기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
		m_f4PickingPos = _float4(f4MousePos.x, f4MousePos.y, f4MousePos.z, f4MousePos.w);
	
	RELEASE_INSTANCE(CGameInstance);

	for (_int i = 0; i < 3; ++i)	// 반복문을 돌면서, 나와 거리가 2안으로 들어오는 좌표가 있는지 확인한다.
	{
		_vector vPickingPos = XMLoadFloat4(&m_f4PickingPos);												// 현재 찍은 좌표
		_vector vPoints = XMLoadFloat4(&_float4(m_f3TempPoints[i].x, m_f3TempPoints[i].y, m_f3TempPoints[i].z, 1.f));	// 바로 전에 찍었던 좌표들

		_vector vDistance = vPoints - vPickingPos;					// 기존 픽킹 자리 - 현재 픽킹 자리
		_float fRange = XMVectorGetX(XMVector3Length(vDistance));	// 사이의 거리를 구한다.

		if (2.f < fRange)																						// 거리가 2 보다 크다면
			m_f4PickingPos = _float4(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z, m_f4PickingPos.w);	// 👉 새로 입력한 좌표를 넣어준다.
		else																									// 거리가 2 보다 작다면
		{
			m_f4PickingPos = _float4(m_f3TempPoints[i].x, m_f3TempPoints[i].y, m_f3TempPoints[i].z, 1.f);		// 👉 이전에 입력한 좌표를 넣어준다.
			break;
		}
	}
	
	// 하나하나찍기
	if (ImGui::Button("0_PointsSave"))
		m_f3Points[0] = _float3(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z);

	if (ImGui::Button("1_PointsSave"))
		m_f3Points[1] = _float3(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z);

	if (ImGui::Button("2_PointsSave"))
		m_f3Points[2] = _float3(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z);

	// 전체적으로 담기
	if (ImGui::Button("Navigation Save"))
	{
		m_f3TempPoints[0] = _float3(m_f3Points[0].x, m_f3Points[0].y, m_f3Points[0].z);
		m_f3TempPoints[1] = _float3(m_f3Points[1].x, m_f3Points[1].y, m_f3Points[1].z);
		m_f3TempPoints[2] = _float3(m_f3Points[2].x, m_f3Points[2].y, m_f3Points[2].z);

		wofstream fout("../../Data/Navigation.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_f3Points[0].x << "|" << m_f3Points[0].y << L"|" << m_f3Points[0].z << "|"
			 << m_f3Points[1].x << "|" << m_f3Points[1].y << L"|" << m_f3Points[1].z << "|"
			 << m_f3Points[2].x << "|" << m_f3Points[2].y << L"|" << m_f3Points[2].z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Navigation.txt", SW_SHOW);

	ImGui::End();


	//_matrix	Temp;
	//XMPlaneTransform(XMVectorSet(m_f3Points[0].x, m_f3Points[0].y, m_f3Points[0].z, 1.f), Temp);

	//XMVector3Cross()

}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
