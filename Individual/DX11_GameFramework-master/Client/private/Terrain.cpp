#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"
#include "Obj_Manager.h"

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

	m_vecPoints.resize(700);

	Lode_Navigation(TEXT("../../Data/Navigation.txt"));

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

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
		ImGui_Navigation();
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
	//	(CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

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

	//if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
	//	return E_FAIL;

	return S_OK;
}

void CTerrain::Lode_Navigation(const _tchar * pNavigationDataFilePath)
{
	wifstream		fin(pNavigationDataFilePath, ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load Navigation");
		return;
	}

	_tchar szNavigatType[MAX_PATH] = L"";

	_tchar szPosX_0[MAX_PATH] = L"";
	_tchar szPosY_0[MAX_PATH] = L"";
	_tchar szPosZ_0[MAX_PATH] = L"";

	_tchar szPosX_1[MAX_PATH] = L"";
	_tchar szPosY_1[MAX_PATH] = L"";
	_tchar szPosZ_1[MAX_PATH] = L"";

	_tchar szPosX_2[MAX_PATH] = L"";
	_tchar szPosY_2[MAX_PATH] = L"";
	_tchar szPosZ_2[MAX_PATH] = L"";

	POINTS	tPoints;

	while (true)
	{
		fin.getline(szNavigatType, MAX_PATH, '|');

		fin.getline(szPosX_0, MAX_PATH, '|');
		fin.getline(szPosY_0, MAX_PATH, '|');
		fin.getline(szPosZ_0, MAX_PATH, '|');

		fin.getline(szPosX_1, MAX_PATH, '|');
		fin.getline(szPosY_1, MAX_PATH, '|');
		fin.getline(szPosZ_1, MAX_PATH, '|');

		fin.getline(szPosX_2, MAX_PATH, '|');
		fin.getline(szPosY_2, MAX_PATH, '|');
		fin.getline(szPosZ_2, MAX_PATH);

		if (fin.eof())
			break;

		tPoints.iCellType = (_int)_wtof(szNavigatType);

		tPoints.Point_A.x = (_float)_wtof(szPosX_0);
		tPoints.Point_A.y = (_float)_wtof(szPosY_0);
		tPoints.Point_A.z = (_float)_wtof(szPosZ_0);

		tPoints.Point_B.x = (_float)_wtof(szPosX_1);
		tPoints.Point_B.y = (_float)_wtof(szPosY_1);
		tPoints.Point_B.z = (_float)_wtof(szPosZ_1);

		tPoints.Point_C.x = (_float)_wtof(szPosX_2);
		tPoints.Point_C.y = (_float)_wtof(szPosY_2);
		tPoints.Point_C.z = (_float)_wtof(szPosZ_2);

		m_vecPoints.push_back(tPoints);
	}
}

void CTerrain::ImGui_Navigation()
{
	ImGui::Begin("NavTool");

	// 마우스 자표 얻어오기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		++m_iButtonCount;
		m_f4PickingPos = _float4(f4MousePos.x, f4MousePos.y, f4MousePos.z, f4MousePos.w);
	}

	for (auto& pPoints : m_vecPoints)
	{
		_vector vPickingPos = XMLoadFloat4(&m_f4PickingPos);														// 현재 찍은 좌표
		_vector vPoints_A = XMLoadFloat4(&_float4(pPoints.Point_A.x, pPoints.Point_A.y, pPoints.Point_A.z, 1.f));	// 바로 이전에 찍은 좌표
		_vector vPoints_B = XMLoadFloat4(&_float4(pPoints.Point_B.x, pPoints.Point_B.y, pPoints.Point_B.z, 1.f));	
		_vector vPoints_C = XMLoadFloat4(&_float4(pPoints.Point_C.x, pPoints.Point_C.y, pPoints.Point_C.z, 1.f));	
		
		_vector vDistance_A = vPoints_A - vPickingPos;																// 기존 픽킹 자리 - 현재 픽킹 자리
		_vector vDistance_B = vPoints_B - vPickingPos;																
		_vector vDistance_C = vPoints_C - vPickingPos;																

		_float fRange_A = XMVectorGetX(XMVector3Length(vDistance_A));												// 사이의 거리를 구한다.
		_float fRange_B = XMVectorGetX(XMVector3Length(vDistance_B));												
		_float fRange_C = XMVectorGetX(XMVector3Length(vDistance_C));												

		_float fSaveRange = 0.1f;

		if (fSaveRange < fRange_A)
			m_f4PickingPos = _float4(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z, m_f4PickingPos.w);	
		else																									
			m_f4PickingPos = _float4(pPoints.Point_A.x, pPoints.Point_A.y, pPoints.Point_A.z, 1.f);

		if (fSaveRange < fRange_B)
			m_f4PickingPos = _float4(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z, m_f4PickingPos.w);
		else
			m_f4PickingPos = _float4(pPoints.Point_B.x, pPoints.Point_B.y, pPoints.Point_B.z, 1.f);

		if (fSaveRange < fRange_C)
			m_f4PickingPos = _float4(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z, m_f4PickingPos.w);
		else
		{
			m_f4PickingPos = _float4(pPoints.Point_C.x, pPoints.Point_C.y, pPoints.Point_C.z, 1.f);
			break;
		}
	}

	// 하나하나찍기
	if(1 == m_iButtonCount)
		m_f3Points[0] = _float3(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z);
	else if (2 == m_iButtonCount)
		m_f3Points[1] = _float3(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z);
	else if (3 == m_iButtonCount)
	{
		m_iButtonCount = 0;
		m_f3Points[2] = _float3(m_f4PickingPos.x, m_f4PickingPos.y, m_f4PickingPos.z);
	}

	//// 벡터 노멀라이즈 하고, 내적
	//// 양수, 음수 음수일 경우 두 점의 위치를 바꿔주면 된다.
	//// 01 / 02

	if (0 == m_iButtonCount)
	{
		// 반 시계 방향 예외처리
		_vector	vDiatance_A = XMVectorSet(m_f3Points[0].x, m_f3Points[0].y, m_f3Points[0].z, 1.f) - XMVectorSet(m_f3Points[1].x, m_f3Points[1].y, m_f3Points[1].z, 1.f);
		_vector	vDiatance_B = XMVectorSet(m_f3Points[0].x, m_f3Points[0].y, m_f3Points[0].z, 1.f) - XMVectorSet(m_f3Points[2].x, m_f3Points[2].y, m_f3Points[2].z, 1.f);
		_vector vCross = XMVector3Cross(vDiatance_A, vDiatance_B);
		_vector vRelease = XMVector3Dot(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVector3Normalize(vCross));
		
		if (0 > XMVectorGetY(vRelease))
		{
			_float3 f3TempPoints = _float3(m_f3Points[1].x, m_f3Points[1].y, m_f3Points[1].z);
			m_f3Points[1] = _float3(m_f3Points[2].x, m_f3Points[2].y, m_f3Points[2].z);
			m_f3Points[2] = _float3(f3TempPoints.x, f3TempPoints.y, f3TempPoints.z);
		}
	}


	if (ImGui::Button("CellType : Default"))
		m_iCellType = 0;
	if (ImGui::Button("CellType : River"))
		m_iCellType = 1;

	if (ImGui::Button("Delegate"))
		m_vecPoints.pop_back();

	// 전체적으로 담기
	if (pGameInstance->Key_Down(DIK_3)) //(ImGui::Button("Navigation Save"))
	{
		if (0 != m_iButtonCount)
		{
			MSG_BOX("점 3개를 다 찍지 않았습니다.!");
			ImGui::End();
			return;
		}

		POINTS points;
		points.Point_A = _float3(m_f3Points[0].x, m_f3Points[0].y, m_f3Points[0].z);
		points.Point_B = _float3(m_f3Points[1].x, m_f3Points[1].y, m_f3Points[1].z);
		points.Point_C = _float3(m_f3Points[2].x, m_f3Points[2].y, m_f3Points[2].z);

		m_vecPoints.push_back(points);

		wofstream fout("../../Data/Navigation.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_iCellType << "|"
			 << m_f3Points[0].x << "|" << m_f3Points[0].y << L"|" << m_f3Points[0].z << "|"
			 << m_f3Points[1].x << "|" << m_f3Points[1].y << L"|" << m_f3Points[1].z << "|"
			 << m_f3Points[2].x << "|" << m_f3Points[2].y << L"|" << m_f3Points[2].z << "\n";

		fout.close();

		// 셀 실시간으로 생성하기
		CNavigation::CELLINFO tCellInfo;
		tCellInfo.iCellType = m_iCellType;
		tCellInfo.Point_A = _float3(m_f3Points[0].x, m_f3Points[0].y, m_f3Points[0].z);
		tCellInfo.Point_B = _float3(m_f3Points[1].x, m_f3Points[1].y, m_f3Points[1].z);
		tCellInfo.Point_C = _float3(m_f3Points[2].x, m_f3Points[2].y, m_f3Points[2].z);
		m_pNavigationCom->Cell_Create(tCellInfo);
	}

	if (pGameInstance->Key_Down(DIK_4)) // (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Navigation.txt", SW_SHOW);

	RELEASE_INSTANCE(CGameInstance);

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

	m_vecPoints.clear();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
