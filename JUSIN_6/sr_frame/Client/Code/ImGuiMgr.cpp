#include "stdafx.h"
#include "..\Header\ImGuiMgr.h"
#include "ImGuizmo.h"
#include "SkeletalCube.h"
#include "ImGuiFileDialog.h"
#include "ImSequencerImpl.h"
#include "TerrainCubeMap.h"
#include "AbstFactory.h"
#include "DynamicCamera.h"
#include "MapTool.h"
#include "BatchTool.h"

ImGuiTextBuffer CImGuiMgr::s_log;
SkeletalPart* CImGuiMgr::s_SelectedPart = nullptr;
_bool CImGuiMgr::s_bShowColl = false;

void CImGuiMgr::TransformEditor(CCamera* pCamera, CTransform* pTransform)
{
#ifndef _DEBUG
	return;
#endif

	NULL_CHECK(pCamera);
	if (pTransform == nullptr)
	{
		IM_LOG("Warning : TransformEditor's pTransform is nullptr");
		return;
	}

	ImGuizmo::BeginFrame();
	static float snap[3] = {1.f, 1.f, 1.f};
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	_matrix matWorld = pTransform->m_matWorld;

	ImGuizmo::DecomposeMatrixToComponents(matWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matWorld);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	static bool useSnap(false);
	// if (ImGui::IsKeyPressed(83))
		// useSnap = !useSnap;
	ImGui::Checkbox("##something", &useSnap);
	ImGui::SameLine();
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}

	_matrix matId;
	D3DXMatrixIdentity(&matId);

	ImGuiIO& io = ImGui::GetIO();
	RECT rt;
	GetClientRect(g_hWnd, &rt);
	POINT lt{rt.left, rt.top};
	ClientToScreen(g_hWnd, &lt);
	ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

	// ImGuizmo::DrawGrid(m_pCam->GetView(), m_pCam->GetPrj(), matId, 100.f);

	ImGuizmo::Manipulate(pCamera->GetView(), pCamera->GetProj(), mCurrentGizmoOperation, mCurrentGizmoMode, matWorld,
	                     NULL, useSnap ? &snap[0] : NULL);

	pTransform->m_matWorld = matWorld;

	ImGuizmo::DecomposeMatrixToComponents(matWorld, matrixTranslation, matrixRotation, matrixScale);
	matrixRotation[0] = D3DXToRadian(matrixRotation[0]);
	matrixRotation[1] = D3DXToRadian(matrixRotation[1]);
	matrixRotation[2] = D3DXToRadian(matrixRotation[2]);
	memcpy(&pTransform->m_vInfo[INFO_POS], matrixTranslation, sizeof(matrixTranslation));
	memcpy(&pTransform->m_vAngle, matrixRotation, sizeof(matrixRotation));
	memcpy(&pTransform->m_vScale, matrixScale, sizeof(matrixScale));
}

void CImGuiMgr::LocalTransformEditor(CCamera* pCamera, _matrix& matLocal)
{
#ifndef _DEBUG
	return;
#endif

	NULL_CHECK(pCamera);

	ImGuizmo::BeginFrame();
	static float snap[3] = {1.f, 1.f, 1.f};
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matLocal, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matLocal);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	static bool useSnap(false);
	// if (ImGui::IsKeyPressed(83))
		// useSnap = !useSnap;
	ImGui::Checkbox("##something", &useSnap);
	ImGui::SameLine();
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}

	_matrix matId;
	D3DXMatrixIdentity(&matId);

	ImGuiIO& io = ImGui::GetIO();
	RECT rt;
	GetClientRect(g_hWnd, &rt);
	POINT lt{rt.left, rt.top};
	ClientToScreen(g_hWnd, &lt);
	ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

	// ImGuizmo::DrawGrid(pCamera->GetView(), pCamera->GetProj(), matId, 100.f);

	ImGuizmo::Manipulate(pCamera->GetView(), pCamera->GetProj(), mCurrentGizmoOperation, mCurrentGizmoMode, matLocal,
	                     NULL, useSnap ? &snap[0] : NULL);

	ImGuizmo::DecomposeMatrixToComponents(matLocal, matrixTranslation, matrixRotation, matrixScale);
}

void CImGuiMgr::LoggerWindow()
{
#ifndef _DEBUG
	return;
#endif

	ImGui::Begin("Logger");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	// _vec3 vpos = Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	// ImGui::Text("x : %f, z : %f", vpos.x, vpos.z);
	if (ImGui::Checkbox("Show Collision", &s_bShowColl))

	if (ImGui::Button("Clear")) { s_log.clear(); }
	ImGui::SameLine();

	ImGui::BeginChild("Log");
	ImGui::SetWindowFontScale(1.1f);
	ImGui::TextUnformatted(s_log.begin(), s_log.end());
	// ImGui::SetScrollHereY(1.f);

	ImGui::EndChild();
	ImGui::End();
}

void CImGuiMgr::Logging(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	s_log.appendfv(fmt, args);
	s_log.append("\n");
	va_end(args);
}

void CImGuiMgr::SkeletalEditor(CCamera* pCamera, CSkeletalCube* pSkeletal)
{
#ifndef _DEBUG
	return;
#endif

	NULL_CHECK(pSkeletal);
	NULL_CHECK(pCamera);

	if (ImGui::Button("Load Skeletal"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToLoad", "Choose File", ".cube",
		                                        "../Bin/Resource/SkeletalCube/");
	}
	if (ImGuiFileDialog::Instance()->Display("ChooseFileToLoad"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring tmp;
			tmp.assign(filePathName.begin(), filePathName.end());
			pSkeletal->LoadSkeletal(tmp);
		}
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Skeletal"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToSave", "Choose File", ".cube",
		                                        "../Bin/Resource/SkeletalCube/");
	}
	if (ImGuiFileDialog::Instance()->Display("ChooseFileToSave"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring tmp;
			tmp.assign(filePathName.begin(), filePathName.end());
			pSkeletal->SaveSkeletal(tmp);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	//////////////////////////////////////////////////
	static SkeletalPart* pSelectedPart = nullptr;
	static string strSelected = CSkeletalCube::s_strRoot;

	static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_DefaultOpen;

	ImGui::Text("Skeletal Selector");
	SkeletalRecursive(pSkeletal->m_pRootPart, strSelected, baseFlags);

	const auto itr = pSkeletal->m_mapParts.find(strSelected);
	if (itr != pSkeletal->m_mapParts.end())
		pSelectedPart = (*itr).second;

	string tmpTex, tmpBuf;
	tmpTex.assign(pSelectedPart->strTexProto.begin(), pSelectedPart->strTexProto.end());
	tmpBuf.assign(pSelectedPart->strBufProto.begin(), pSelectedPart->strBufProto.end());
	
	ImGui::Text("Texture : %s %d", tmpTex.c_str(), pSelectedPart->iTexIdx);
	ImGui::Text("Buffer : %s", tmpBuf.c_str());

	//////////////////////////////////////////////////////
	ImGui::Separator();
	ImGui::Text("Part Transform");
	enum TransMode
	{
		TransMode_World,
		TransMode_Local
	};
	static int mode = TransMode_World;
	if (ImGui::RadioButton("World", mode == TransMode_World)) { mode = TransMode_World; }
	ImGui::SameLine();
	if (ImGui::RadioButton("Local", mode == TransMode_Local)) { mode = TransMode_Local; }

	switch (mode)
	{
	case TransMode_World:
		if (pSelectedPart == nullptr) break;
		ImGui::Text("World Transform");
		TransformEditor(pCamera, pSelectedPart->pTrans);
		break;
	case TransMode_Local:
		if (pSelectedPart == nullptr) break;
		ImGui::Text("Local Transform");
		LocalTransformEditor(pCamera, pSelectedPart->matLocal);
		break;
	default:
		IM_LOG("Warning : selected TransMode is not valid");
	}

	//////////////////////////////////////////////////////
	ImGui::Separator();
	static wstring strSelectTex;
	static _uint iSelectTexNum;
	static wstring strSelectBuf;
	static char szPartName[16] = "";
	CImGuiMgr::TextureSelector(strSelectTex, iSelectTexNum);
	ImGui::Separator();
	CImGuiMgr::VIBufferSelector(strSelectBuf);
	if (ImGui::Button("Add Part"))
	{
		if (pSelectedPart != nullptr && !strSelectTex.empty() && !strSelectBuf.empty() && strcmp(szPartName, "") != 0)
		{
			pSkeletal->AddSkeletalPart(string(szPartName), strSelected, strSelectBuf, strSelectTex, iSelectTexNum);
		}
		else
		{
			IM_LOG("Warning : Select Part, texture, Part Name and buffer to add");
		}
	}
	ImGui::SameLine();
	ImGui::InputText("Part Name", &szPartName[0], sizeof(szPartName));
	if (ImGui::Button("Change Texture Index"))
	{
		if (pSelectedPart != nullptr && !strSelectTex.empty())
		{
			pSelectedPart->iTexIdx = iSelectTexNum;
		}
		else
		{
			IM_LOG("Warning : Select Part and Texture to Change");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Select Part"))
	{
		if (pSelectedPart != nullptr)
		{
			pSkeletal->DeleteSkeletalPart(strSelected);
			strSelected = CSkeletalCube::s_strRoot;
		}
		else
		{
			IM_LOG("Warning : Select Part to Delete");
		}
	}

	s_SelectedPart = pSelectedPart; // for AnimationEditor
}

void CImGuiMgr::TextureSelector(wstring& strTex, _uint& iTexIdx)
{
#ifndef _DEBUG
	return;
#endif

	ImGui::Text("Texture Selector");
	static const vector<wstring> vecTexName{
		L"Proto_CubeTexture",
		L"Proto_MinecraftCubeTexture",
		L"Proto_WeaponTexture",
		L"Proto_BossCubeTile"
	};
	static vector<_int> vecTexIdx(vecTexName.size(), 0);
	static size_t iCurIdx = 0;

	if (ImGui::BeginListBox("##1"))
	{
		for (size_t i = 0; i < vecTexName.size(); ++i)
		{
			CTexture* pTexture = dynamic_cast<CTexture*>(Find_Proto(vecTexName[i]));
			NULL_CHECK(pTexture);

			size_t iTexSize = pTexture->GetTexSize() - 1;
			string _TexName;
			_TexName.assign(vecTexName[i].begin(), vecTexName[i].end());

			const bool bSelected = (iCurIdx == i);
			if (ImGui::Selectable(_TexName.c_str(), bSelected))
			{
				iCurIdx = i;
				strTex = vecTexName[i];
				iTexIdx = vecTexIdx[i];
			}

			if (bSelected)
			{
				ImGui::SliderInt("TexIdx", &vecTexIdx[i], 0, static_cast<int>(iTexSize));
				ImGui::InputInt("TexIdxInput", &vecTexIdx[i]);
				iTexIdx = vecTexIdx[i];
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}
}

void CImGuiMgr::VIBufferSelector(wstring& strBuf)
{
	ImGui::Text("VIBuffer Selector");
	static const vector<wstring> vecBufName{
		L"Proto_CubeTexCom",
		L"Proto_VoxelTex_Sword",
		L"Proto_VoxelTex_Bow",
	};
	static size_t iCurIdx = 0;

	if (ImGui::BeginListBox("##2"))
	{
		for (size_t i = 0; i < vecBufName.size(); ++i)
		{
			CVIBuffer* pBuf = dynamic_cast<CVIBuffer*>(Find_Proto(vecBufName[i]));
			NULL_CHECK(pBuf);

			string _BufName;
			_BufName.assign(vecBufName[i].begin(), vecBufName[i].end());

			const bool bSelected = (iCurIdx == i);
			if (ImGui::Selectable(_BufName.c_str(), bSelected))
			{
				iCurIdx = i;
				strBuf = vecBufName[i];
			}
		}
		ImGui::EndListBox();
	}
}

void CImGuiMgr::AnimationEditor(CSkeletalCube* pSkeletal)
{
#ifndef _DEBUG
	return;
#endif

	ImGui::Text("60 frame = 1 sec / only use world transform");
	static CImSequencerImpl mySequence;
	static bool bOnce = true;
	if (bOnce)
	{
		pSkeletal->m_pCurAnim = &mySequence.m_CubeAnim;
		mySequence.m_CubeAnim.bLoop = true;
		mySequence.m_iFrameMin = 0;
		mySequence.m_iFrameMax = 100;
		bOnce = false;
	}

	static int selectedEntry = -1;
	static int firstFrame = 0;
	static bool expanded = true;
	static int currentFrame = 0;
	static char szPartName[128];
	static char szEventName[128];
	static int iFrameAmount = 0;

	if (mySequence.m_iFrameMin < 0)
		mySequence.m_iFrameMin = 0;

	if (pSkeletal->m_bStopAnim == false)
	{
		currentFrame = static_cast<int>(pSkeletal->m_fAccTime * 60.f); // 60 frame == 1 sec
	}
	else
	{
		pSkeletal->m_fAccTime = (_float)currentFrame / 60.f;
	}

	mySequence.m_CubeAnim.fTotalTime = (_float)mySequence.m_iFrameMax / 60.f;

	ImGui::PushItemWidth(130);
	ImGui::InputInt("Frame Min", &mySequence.m_iFrameMin);
	ImGui::SameLine();
	ImGui::InputInt("Frame ", &currentFrame);
	ImGui::SameLine();
	ImGui::InputInt("Frame Max", &mySequence.m_iFrameMax);
	ImGui::InputText("PartName", szPartName, 128);
	ImGui::SameLine();
	if (ImGui::Button("Add Part"))
	{
		mySequence.AddPart(szPartName);
	}
	if (ImGui::Button("Play"))
	{
		pSkeletal->m_bStopAnim = !pSkeletal->m_bStopAnim;
	}
	if (ImGui::Button("Add Selected Part Trans"))
	{
		mySequence.AddTransFrame(currentFrame, s_SelectedPart);
	}
	ImGui::SameLine();
	if (ImGui::Button("Add All Part Trans"))
	{
		mySequence.AddTransFrameRecur(currentFrame, pSkeletal->m_pRootPart);
	}
	ImGui::InputText("Event Name", szEventName, 128);
	ImGui::SameLine();
	if (ImGui::Button("Add Event"))
	{
		mySequence.AddEvent(currentFrame, szEventName);
	}
	ImGui::InputInt("Move Frame Amount", &iFrameAmount);
	ImGui::SameLine();
	if (ImGui::Button("Move Frame"))
	{
		mySequence.MoveFrame(currentFrame, iFrameAmount);
	}

	ImGui::PopItemWidth();
	Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame,
	          ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_CHANGE_FRAME);


	if (ImGui::Button("Load Anim"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToLoadAnim", "Choose File", ".anim",
		                                        "../Bin/Resource/CubeAnim/");
	}
	if (ImGuiFileDialog::Instance()->Display("ChooseFileToLoadAnim"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring tmp;
			tmp.assign(filePathName.begin(), filePathName.end());
			mySequence.LoadAnimFrame(tmp);
			pSkeletal->m_pCurAnim = &mySequence.m_CubeAnim;
		}
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Anim"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToSaveAnim", "Choose File", ".anim",
		                                        "../Bin/Resource/CubeAnim/");
	}
	if (ImGuiFileDialog::Instance()->Display("ChooseFileToSaveAnim"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring tmp;
			tmp.assign(filePathName.begin(), filePathName.end());
			mySequence.m_CubeAnim.Save(tmp);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void CImGuiMgr::MapControl(Engine::MapTool& tMaptool, _float& _far, CTerrainCubeMap* cubemap)
{
#ifndef _DEBUG
	return;
#endif
	ImGui::Separator();

	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Set Block"))
		{

			ImGui::Text("Set Block Texture");
			ImGui::InputInt("Cube Index", &tMaptool.iTexIdx);
			ImGui::NewLine();

			ImGui::Text("Set Plant Texture");
			ImGui::InputInt("Plant Texture", &tMaptool.iPlantIdx);
			ImGui::NewLine();

			ImGui::Text("Set Height");
			ImGui::SliderFloat("Y", &tMaptool.fHeight, 1.f, 2.0f, "ratio = %.3f");
			ImGui::InputFloat("Insert Y", &tMaptool.fHeight);
			ImGui::NewLine();

			ImGui::Text("Set Far");
			ImGui::SliderFloat("Far", &_far, 10.f, 30.0f, "ratio = %.3f");
			ImGui::InputFloat("Insert Far", &_far);
			ImGui::NewLine();

		
			ImGui::Text("Select Options");
			ImGui::RadioButton("Cube", &tMaptool.iPickingOption, PICK_CUBE); ImGui::SameLine();
			ImGui::RadioButton("Plant", &tMaptool.iPickingOption, PICK_PLANT); ImGui::SameLine();
			ImGui::RadioButton("Delete", &tMaptool.iPickingOption, PICK_DELETE); ImGui::SameLine();

			ImGui::NewLine();
			ImGui::NewLine();

			ImGui::Text("Cube Type");
			ImGui::SameLine(); 
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "     (Please Check Type!!)");
			ImGui::RadioButton("Land", &tMaptool.iCubeType, TYPE_LAND); ImGui::SameLine();
			ImGui::RadioButton("Collision", &tMaptool.iCubeType, TYPE_COLLISION); ImGui::SameLine();
			ImGui::RadioButton("Decoration", &tMaptool.iCubeType, TYPE_DECO); ImGui::SameLine();

			ImGui::NewLine();
			ImGui::NewLine();

			static int clicked = 0;
			if (ImGui::Button("Debug Mode"))
				clicked++;

			if (clicked & 1)
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Checked!!");
				cubemap->m_bRendState = true;
			}
			else
				tMaptool.bRendState = false;


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Set Rect"))
		{
			ImGui::Text("holl");

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Set Element"))
		{
			ImGui::Text("checkyt");

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Save / Load"))
		{
			
			if (ImGui::Button("Load Map"))
			{
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToLoadMap", "Choose File", ".map",
					"../Bin/Resource/Map/");
			}
			if (ImGuiFileDialog::Instance()->Display("ChooseFileToLoadMap"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

					wstring tmp;
					tmp.assign(filePathName.begin(), filePathName.end());
					cubemap->LoadMap(tmp);
				}
				ImGuiFileDialog::Instance()->Close();
			}
			ImGui::SameLine();

			if (ImGui::Button("Save Map"))
			{
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToSaveMap", "Choose File", ".map",
					"../Bin/Resource/Map/");
			}
			if (ImGuiFileDialog::Instance()->Display("ChooseFileToSaveMap"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

					wstring tmp;
					tmp.assign(filePathName.begin(), filePathName.end());
					cubemap->SaveMap(tmp);
				}
				ImGuiFileDialog::Instance()->Close();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	
}

void CImGuiMgr::SceneSwitcher()
{
#ifndef _DEBUG
	return;
#endif

	ImGui::Begin("Scene Switcher");
	static string strLoadingTag;
	static int iCurLoadingTagIdx = 0;

	static string strSceneTag;
	static int iCurSceneTagIdx = 0;

	static int iDelay = 1000;

	if (ImGui::BeginListBox("Loading Tag"))
	{
		int iIdx = 0;
		for (auto& loading : CSceneFactory::s_mapLoadingSpawner)
		{
			const bool bSelected = iCurLoadingTagIdx == iIdx;
			if (ImGui::Selectable(loading.first.c_str(), bSelected))
			{
				iCurLoadingTagIdx = iIdx;
				strLoadingTag = loading.first;
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
			++iIdx;
		}
		ImGui::EndListBox();
	}
	if (ImGui::BeginListBox("Scene Tag"))
	{
		int iIdx = 0;
		for (auto& scene : CSceneFactory::s_mapSceneSpawner)
		{
			const bool bSelected = iCurSceneTagIdx == iIdx;
			if (ImGui::Selectable(scene.first.c_str(), bSelected))
			{
				iCurSceneTagIdx = iIdx;
				strSceneTag = scene.first;
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
			++iIdx;
		}
		ImGui::EndListBox();
	}
	ImGui::SliderInt("Loading Min Delay", &iDelay, 0, 3000);

	if (ImGui::Button("Scene Load"))
	{
		CSceneFactory::LoadScene(strLoadingTag, strSceneTag, false, iDelay);
	}
	ImGui::SameLine();
	if (ImGui::Button("Scene Load delete prev"))
	{
		CSceneFactory::LoadScene(strLoadingTag, strSceneTag, true, iDelay);
	}

	ImGui::End();
}

void CImGuiMgr::BatchControl(CCamera* pCamera, CTransform*& pTransform, CTerrainCubeMap* pMap, LPDIRECT3DDEVICE9 pGraphicDev)
{
	static string strEnemyFactoryTag;
	static string strobjFactoryTag;
	static int iFactoryNum = 0;
	static _uint iObjNumber = 0;

	if (pTransform)
		TransformEditor(pCamera, pTransform);
	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Enemy"))
		{
			iFactoryNum = 0;

			static _uint iCurTagIdx = 0;
			if (ImGui::BeginListBox("Enemy Factory"))
			{
				int iIdx = 0;
				for (auto& enemy : CEnemyFactory::s_mapEnemySpawner)
				{
					const bool bSelected = iCurTagIdx == iIdx;
					if (ImGui::Selectable(enemy.first.c_str(), bSelected))
					{
						iCurTagIdx = iIdx;
						strEnemyFactoryTag = enemy.first;
					}
				
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					++iIdx;
				}
				ImGui::EndListBox();
			}

			if (ImGui::BeginListBox("Current Enemy"))
			{
				for (const auto& obj : Engine::Get_Layer(LAYER_ENEMY)->Get_MapObject())
				{
					string tmp;
					tmp.assign(obj.first.begin(), obj.first.end());
					if (ImGui::Selectable(tmp.c_str()))
					{
						pTransform = obj.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
					}
				}
				ImGui::EndListBox();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Object"))
		{
			iFactoryNum = 1;

			static _uint iCurTagIdx = 0;
			if (ImGui::BeginListBox("Object Factory"))
			{
				int iIdx = 0;
				for (auto& object : CObjectFactory::s_mapObjectSpawner)
				{
					const bool bSelected = iCurTagIdx == iIdx;
					if (ImGui::Selectable(object.first.c_str(), bSelected))
					{
						iCurTagIdx = iIdx;
						strobjFactoryTag = object.first;
					}
				
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					++iIdx;
				}
				ImGui::EndListBox();
			}

			if (ImGui::BeginListBox("Current Object"))
			{
				for (const auto& obj : Engine::Get_Layer(LAYER_GAMEOBJ)->Get_MapObject())
				{
					string tmp;
					tmp.assign(obj.first.begin(), obj.first.end());
					if (ImGui::Selectable(tmp.c_str()))
					{
						if (obj.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
							pTransform = obj.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
						else if (obj.second->Has_Component(L"Proto_TransformCom", ID_DYNAMIC))
							pTransform = obj.second->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
					}
				}
				ImGui::EndListBox();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabItem();
	}


	// 피킹한 위치에 선택한 팩토리로 생성
	if (MouseKeyDown(DIM_RB))
	{
		// pMap->m_vecLand
		_vec3 vOrigin, vRayDir;
		_matrix matView, matProj;
		D3DVIEWPORT9 ViewPort;

		ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
		pGraphicDev->GetViewport(&ViewPort);
		pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		CGameUtilMgr::GetPickingRay(vOrigin, vRayDir, g_hWnd, matView, matProj, ViewPort);
		
		_vec3 vCam = dynamic_cast<CDynamicCamera*>(pCamera)->GetEys();
		_vec3 vFaceVtx[FACE_END][4];

		_vec3	dwVtxIdxRU[3]{};
		_vec3	dwVtxIdxLD[3]{};
		_float	fU, fV, fDist;


		_float	MinDist = 10000000;
		_int eFace = FACE_END;
		size_t iCurCube = 0;

		for (size_t i = 0; i < pMap->m_vecLand.size(); ++i)
		{
			_vec3 vPos;
			vPos.x = pMap->m_vecLand[i].matWorld._41;
			vPos.y = pMap->m_vecLand[i].matWorld._42;
			vPos.z = pMap->m_vecLand[i].matWorld._43;
			_vec3 vDiff = vPos - vCam;
			if (D3DXVec3LengthSq(&vDiff) > 900.f) continue; // 큐브 거리가 카메라부터 30 초과면 체크 안함

			memcpy(&vFaceVtx, &CMapTool::s_vFaceVtx, sizeof(vFaceVtx));
			for (int j = 0; j < FACE_END; ++j)
			{
				for (int k = 0; k < 4; ++k)
					D3DXVec3TransformCoord(&vFaceVtx[j][k], &vFaceVtx[j][k], &pMap->m_vecLand[i].matWorld);
			}

			for (_ulong j = 0; j < FACE_END; ++j)
			{
					// 오른쪽 위
					dwVtxIdxRU[0] = vFaceVtx[j][0];
					dwVtxIdxRU[1] = vFaceVtx[j][1];
					dwVtxIdxRU[2] = vFaceVtx[j][2];

					// 왼쪽 아래
					dwVtxIdxLD[0] = vFaceVtx[j][0];
					dwVtxIdxLD[1] = vFaceVtx[j][2];
					dwVtxIdxLD[2] = vFaceVtx[j][3];

					//광선과 닿은 면이 있다면 vRayPos와의 거리가 최소값인지 확인한다.
					BOOL RUtriCheck = D3DXIntersectTri(&dwVtxIdxRU[1], &dwVtxIdxRU[2], &dwVtxIdxRU[0], &vOrigin, &vRayDir, &fU, &fV, &fDist);
					if (RUtriCheck && MinDist > fDist)
					{
						iCurCube = i;
						eFace = j;
						MinDist = fDist;
						continue;
					}

					BOOL LDtriCheck = D3DXIntersectTri(&dwVtxIdxLD[0], &dwVtxIdxLD[1], &dwVtxIdxLD[2], &vOrigin, &vRayDir, &fU, &fV, &fDist);
					if (LDtriCheck && MinDist > fDist)
					{				
						iCurCube = i;
						eFace = j;
						MinDist = fDist;		
					}

			}
		}

		if (eFace != FACE_END) // 찾음
		{
			_vec3 vCenter{ 0.f, 0.f, 0.f };
			D3DXVec3TransformCoord(&vCenter, &vCenter, &pMap->m_vecLand[iCurCube].matWorld);
			vCenter.y += 1.f;

			if (iFactoryNum == 0 && strEnemyFactoryTag.empty() == false)
			{
				wstring tmp;
				tmp.assign(strEnemyFactoryTag.begin(), strEnemyFactoryTag.end());
				CEnemyFactory::Create<CGameObject>(strEnemyFactoryTag, tmp + L"_" + to_wstring(iObjNumber++), pMap->m_vecLand[iCurCube].matWorld);
			}

			if (iFactoryNum == 1 && strobjFactoryTag.empty() == false)
			{
				wstring tmp;
				tmp.assign(strobjFactoryTag.begin(), strobjFactoryTag.end());
				CObjectFactory::Create<CGameObject>(strobjFactoryTag, tmp + L"_" + to_wstring(iObjNumber++), pMap->m_vecLand[iCurCube].matWorld);
			}
		}
	}
}

// void CImGuiMgr::UiEditor(Engine::UiTool& tUitool, CTerrainCubeMap* cubemap)
// {
// #ifndef _DEBUG
// 	return;
// #endif
// 	ImGui::Separator();
//
// 	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
// 	{
// 		if (ImGui::BeginTabItem("Set Texture"))
// 		{
//
// 			ImGui::Text("Set UI Texture");
// 			ImGui::InputInt("UI Index", &tUitool.iTexIdx);
// 			ImGui::NewLine();
//
// 			ImGui::Text("Set Scale");
// 			ImGui::SliderFloat("X", &tUitool.fX, 1.f, 2.f, "ratio = %.3f");
// 			ImGui::SliderFloat("Y", &tUitool.fY, 1.f, 2.f, "ratio = %.3f");
// 			ImGui::SliderFloat("Z", &tUitool.fZ, 1.f, 2.f, "ratio = %.3f");
// 			ImGui::InputFloat("Insert X", &tUitool.fX);
// 			ImGui::InputFloat("Insert Y", &tUitool.fY);
// 			ImGui::InputFloat("Insert Z", &tUitool.fZ);
// 			ImGui::NewLine();
//
// 			ImGui::Text("Select Options");
// 			ImGui::RadioButton("PicKing", &tUitool.iPickingOption, PICK_CUBE); ImGui::SameLine();
// 			ImGui::RadioButton("Delete", &tUitool.iPickingOption, PICK_DELETE); ImGui::SameLine();
//
// 			ImGui::NewLine();
// 			ImGui::NewLine();
//
// 			ImGui::EndTabItem();
// 	}
//
// 		if (ImGui::BeginTabItem("Save / Load"))
// 		{
// 			if (ImGui::Button("Load UI"))
// 			{
// 				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToLoadUI", "Choose File", ".UI",
// 					"../Bin/Resource/UI/");
// 			}
// 			if (ImGuiFileDialog::Instance()->Display("ChooseFileToLoadUI"))
// 			{
// 				if (ImGuiFileDialog::Instance()->IsOk())
// 				{
// 					std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
//
// 					wstring tmp;
// 					tmp.assign(filePathName.begin(), filePathName.end());
// 					cubemap->LoadMap(tmp);
// 				}
// 				ImGuiFileDialog::Instance()->Close();
// 			}
// 			ImGui::SameLine();
//
// 			if (ImGui::Button("Save UI"))
// 			{
// 				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileToSaveUI", "Choose File", ".UI",
// 					"../Bin/Resource/UI/");
// 			}
// 			if (ImGuiFileDialog::Instance()->Display("ChooseFileToSaveUI"))
// 			{
// 				if (ImGuiFileDialog::Instance()->IsOk())
// 				{
// 					std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
//
// 					wstring tmp;
// 					tmp.assign(filePathName.begin(), filePathName.end());
// 					cubemap->SaveMap(tmp);
// 				}
// 				ImGuiFileDialog::Instance()->Close();
// 			}
//
// 			ImGui::EndTabItem();
// 		}
//
// 		ImGui::EndTabBar();
// }
// }

void CImGuiMgr::SkeletalRecursive(SkeletalPart* Part, string& strSelected, ImGuiTreeNodeFlags baseFlags)
{
#ifndef _DEBUG
	return;
#endif

	NULL_CHECK(Part);

	ImGuiTreeNodeFlags nodeFlags = baseFlags;
	if (strSelected == Part->strName)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx(Part->strName.c_str(), nodeFlags, Part->strName.c_str()))
	{
		if (ImGui::IsItemClicked())
			strSelected = Part->strName;

		for (const auto& child : Part->vecChild)
			SkeletalRecursive(child, strSelected, baseFlags);

		ImGui::TreePop();
	}
}
