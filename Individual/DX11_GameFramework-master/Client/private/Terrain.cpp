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
			cout << "PickPos : " << vPickPos.x << " | " << vPickPos.y << " | " << vPickPos.z << " | " << vPickPos.w << endl;
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

void CTerrain::ImGui_Navigation()
{
	ImGui::Begin("NavTool");

	// 마우스 자표 얻어오기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
		m_PickingPos = _float4(f4MousePos.x, f4MousePos.y, f4MousePos.z, f4MousePos.w);
	
	RELEASE_INSTANCE(CGameInstance);


	//_vector vDistance = vSourPoints - vDestPoints;
	//_float fDistance = XMVectorGetX(XMVector3Length(vDistance));






	// 하나하나찍기
	if (ImGui::Button("0_PointsSave"))
		m_f3Points[0] = _float3(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z);

	if (ImGui::Button("1_PointsSave"))
		m_f3Points[1] = _float3(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z);

	if (ImGui::Button("2_PointsSave"))
		m_f3Points[2] = _float3(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z);

	// 전체적으로 담기
	if (ImGui::Button("Navigation Save"))
	{
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
