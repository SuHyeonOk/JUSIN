#pragma once

#ifdef _DEBUG
#define IM_LOG(...) CImGuiMgr::Logging(__VA_ARGS__)
#define IM_BEGIN(win_name) ImGui::Begin(win_name)
#define IM_END ImGui::End()
#define DEBUG_SPHERE(vPos, fRadius, fAlive) if (CImGuiMgr::IsShowColl()) Engine::AddGameObject(LAYER_GAMEOBJ, L"DebugSphere", CDebugSphere::Create(m_pGraphicDev, vPos, fRadius, fAlive))
#define DEBUG_LINE(vOrigin, vDir, fLen, fAlive) if (CImGuiMgr::IsShowColl()) Engine::AddGameObject(LAYER_GAMEOBJ, L"DebugLine", CDebugLine::Create(m_pGraphicDev, vOrigin, vDir, fLen, fAlive))
#else
#define IM_LOG(...) 
#define IM_BEGIN(win_name) 
#define IM_END 
#define DEBUG_SPHERE(vPos, fRadius, fAlive) 
#endif


class CSkeletalCube;
class CTerrainCubeMap;
struct SkeletalPart;

class CImGuiMgr
{
public:
	static void TransformEditor(CCamera* pCamera, CTransform* pTransform); // for CTransform
	static void LocalTransformEditor(CCamera* pCamera, _matrix& matLocal); // for SkeletalPart
	static void LoggerWindow();
	static void Logging(const char* fmt, ...);
	static void SkeletalEditor(CCamera* pCamera, CSkeletalCube* pSkeletal); // for CSkeletalCube
	static void TextureSelector(wstring& strTex, _uint& iTexIdx);
	static void VIBufferSelector(wstring& strBuf);
	static void AnimationEditor(CSkeletalCube* pSkeletal);
	static void MapControl(Engine::MapTool& tMaptool, _float& _far, CTerrainCubeMap* cubemap);

	static void SceneSwitcher();
	static void BatchControl(CCamera* pCamera, CTransform*& pTransform, CTerrainCubeMap* pMap, LPDIRECT3DDEVICE9 pGraphicDev);
	static _bool IsShowColl() { return s_bShowColl; };
	// static void UiEditor(Engine::UiTool& tUitool, CTerrainCubeMap* cubemap);

private:
	static void SkeletalRecursive(SkeletalPart* Part, string& strSelected, ImGuiTreeNodeFlags baseFlags);

public:
	static ImGuiTextBuffer s_log;
	static SkeletalPart* s_SelectedPart; // for AnimationEditor
	static _bool s_bShowColl;


};
