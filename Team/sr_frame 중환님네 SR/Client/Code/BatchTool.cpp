#include "stdafx.h"
#include "BatchTool.h"
#include "DynamicCamera.h"
#include "AbstFactory.h"
#include "ImGuiFileDialog.h"
#include "Player.h"
#include "TerrainCubeMap.h"

CBatchTool::CBatchTool(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}

CBatchTool::~CBatchTool()
{
}

HRESULT CBatchTool::Ready_Scene()
{
	CScene::Ready_Scene();

	CGameObject* pGameObject = nullptr;
	pGameObject = m_pCam = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);


	pGameObject = m_pMap = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/Stage1.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);

	return S_OK;
}

_int CBatchTool::Update_Scene(const _float& fTimeDelta)
{
	CScene::Update_Scene(fTimeDelta);

	IM_BEGIN("Batch Tool");

	if (ImGui::Button("Load Batch"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToLoadBatch", "Choose File", ".batch",
		                                        "../Bin/Resource/Batch/");
	}
	if (ImGuiFileDialog::Instance()->Display("ChooseFileToLoadBatch"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring tmp;
			tmp.assign(filePathName.begin(), filePathName.end());
			Load(tmp);
		}
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Batch"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToSaveBatch", "Choose File", ".batch",
		                                        "../Bin/Resource/Batch/");
	}
	if (ImGuiFileDialog::Instance()->Display("ChooseFileToSaveBatch"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring tmp;
			tmp.assign(filePathName.begin(), filePathName.end());
			Save(tmp);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	CImGuiMgr::BatchControl(m_pCam, m_pTransform, m_pMap, m_pGraphicDev);
	IM_END;


	// 좌클릭 : collision 가능한 오브젝트 선택
	if (MouseKeyDown(DIM_LB))
	{
		_vec3 vOrigin, vRayDir;
		_matrix matView, matProj;
		D3DVIEWPORT9 ViewPort;

		ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
		m_pGraphicDev->GetViewport(&ViewPort);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		CGameUtilMgr::GetPickingRay(vOrigin, vRayDir, g_hWnd, matView, matProj, ViewPort);

		// collcom 가지면 피킹 가능
		for (int i = 0; i < LAYER_END; ++i)
		{
			for (const auto& obj : Engine::Get_Layer((LAYERID)i)->Get_MapObject())
			{
				CTransform* pTrans = nullptr;
				if (obj.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
				{
					pTrans = obj.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
				}
				else if (obj.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
				{
					pTrans = obj.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
				}
				if (pTrans == nullptr) continue;

				const _vec3 vSubject = vOrigin - pTrans->m_vInfo[INFO_POS];
				const _float fB = D3DXVec3Dot(&vRayDir, &vSubject);
				const _float fC = D3DXVec3Dot(&vSubject, &vSubject) - 0.5f;
				if (fB * fB  - fC >= 0.f)
				{
					m_pTransform = pTrans;
					goto LoopEnd;
				}
			}
		}

		LoopEnd:
		int a =3;
	}

	// 우클릭 : 선택한 팩토리 태그로 피킹한 위치에 생성

	return 0;
}


void CBatchTool::Free()
{
	CScene::Free();
}

CBatchTool* CBatchTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBatchTool* pInstance = new CBatchTool(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}


	return pInstance;
}

void CBatchTool::Load(const wstring& wstrPath)
{
	// 적, 게임 오브젝트 만 로드합니다.
	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Fail to Load CubeAnimFrame");
		return;
	}
	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	_uint iObjNum = 0;

	size_t mapSize;
	ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);
	// enemy
	string strEnemyTag;
	wstring tmp;
	_matrix matWorld;
	for (size_t i = 0; i < mapSize; ++i)
	{
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		_tchar wszBuf[128]{};
		ReadFile(hFile, wszBuf, dwStrByte, &dwByte, nullptr);
		tmp = wszBuf;
		ReadFile(hFile, &matWorld, sizeof(_matrix), &dwByte, nullptr);

		strEnemyTag.assign(tmp.begin(), tmp.end());

		CEnemyFactory::Create<CGameObject>(strEnemyTag, tmp + L"_" + to_wstring(iObjNum++), matWorld);
	}

	ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);


	string strObjectTag;
	for (size_t i = 0; i < mapSize; ++i)
	{
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		_tchar wszBuf[128]{};
		ReadFile(hFile, wszBuf, dwStrByte, &dwByte, nullptr);
		tmp = wszBuf;
		ReadFile(hFile, &matWorld, sizeof(_matrix), &dwByte, nullptr);


		strEnemyTag.assign(tmp.begin(), tmp.end());
		CObjectFactory::Create<CGameObject>(strEnemyTag, tmp + L"_" + to_wstring(iObjNum++), matWorld);
	}
}

void CBatchTool::Save(const wstring& wstrPath)
{
	// 적, 게임 오브젝트 만 저장합니다.

	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Fail to Save Batch");
		return;
	}
	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	size_t mapSize = m_arrLayer[LAYER_ENEMY]->Get_MapObject().size();
	WriteFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);

	for(auto& e : m_arrLayer[LAYER_ENEMY]->Get_MapObject())
	{
		// target part name
		wstring strFactoryTag = e.first;
		strFactoryTag = SplitWString(strFactoryTag, '_').front();

		dwStrByte = (DWORD)strFactoryTag.size() * sizeof(_tchar);
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, strFactoryTag.c_str(), dwStrByte, &dwByte, nullptr);

		_matrix matWorld;
		if (e.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
			matWorld = e.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
		else if (e.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
			matWorld = e.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
		else _CRASH("Fail to load");
		WriteFile(hFile, &matWorld, sizeof(_matrix), &dwByte, nullptr);
	}

	mapSize = m_arrLayer[LAYER_GAMEOBJ]->Get_MapObject().size() - 1; // static camera
	WriteFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);

	for(auto& e : m_arrLayer[LAYER_GAMEOBJ]->Get_MapObject())
	{
		if (e.first == L"DynamicCamera") continue;

		wstring strFactoryTag = e.first;
		strFactoryTag = SplitWString(strFactoryTag, '_').front();

		// target part name
		dwStrByte = (DWORD)strFactoryTag.size() * sizeof(_tchar);
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, strFactoryTag.c_str(), dwStrByte, &dwByte, nullptr);

		_matrix matWorld;
		if (e.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
			matWorld = e.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
		else if (e.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
			matWorld = e.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
		else _CRASH("Fail to load");
		WriteFile(hFile, &matWorld, sizeof(_matrix), &dwByte, nullptr);
	}
}

std::vector<std::wstring> CBatchTool::SplitWString(std::wstring str, _tchar splitter)
{
	std::vector<std::wstring> result;
	std::wstring current = L"";
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == splitter)
		{
			if (current != L"")
			{
				result.push_back(current);
				current = L"";
			}
			continue;
		}
		current += str[i];
	}
	if (current.size() != 0)
		result.push_back(current);
	return result;
}
