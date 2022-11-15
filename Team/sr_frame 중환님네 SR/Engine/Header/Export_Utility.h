#ifndef Export_Utility_h__
#define Export_Utility_h__

#include "Engine_Include.h"
#include "RcTex.h"
#include "Texture.h"
#include "TerrainTex.h"
#include "CubeTex.h"
#include "VoxelTex.h"
#include "TerrainCubeTex.h"
#include "TerrainRcTex.h"
#include "CollisionCom.h"
#include "ParticleSystem.h"
#include "Collider.h"
#include "RcShader.h"
#include "DebugLine.h"
#include "SphereMesh.h"
#include "TerrainShader.h"
#include "Transform.h"
#include "Calculator.h"
#include "Camera.h"
#include "DebugSphere.h"
#include "TerrainShader.h"
#include "ProtoMgr.h"
#include "Management.h"
#include "Renderer.h"
#include "GameUtilMgr.h"
#include "Collider.h"
#include "RcBossHPTex.h"

#include "LightMgr.h"
#include "Shader.h"
#include "ShereBuffer.h"
#include "TargetTexture.h"


BEGIN(Engine)
	// Management
	inline void SwitchSceneLoading(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay = 1000/*0.5√ */);
	inline void SwitchSceneLoadingDeletePrev(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay = 1000/*0.5√ */);


	inline HRESULT Create_Management(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement);
	inline HRESULT Set_Scene(CScene* pScene);
	inline _int Update_Scene(const _float& fTimeDelta);
	inline void LateUpdate_Scene(void);
	inline void Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

	template <typename T>
	T* Get_Component(LAYERID eLayerID,
	                                 const wstring& pObjTag,
	                                 const wstring& pComponentTag,
	                                 COMPONENTID eID)
	{
		CComponent* pComponent = CManagement::GetInstance()->Get_Component(eLayerID, pObjTag, pComponentTag, eID);
		T* pCasted = dynamic_cast<T*>(pComponent);
		NULL_CHECK_RETURN(pCasted, nullptr);

		return pCasted;
	}
	template <typename T>
	T* Get_ComponentUnCheck(LAYERID eLayerID,
	                                 const wstring& pObjTag,
	                                 const wstring& pComponentTag,
	                                 COMPONENTID eID)
	{
		CComponent* pComponent = CManagement::GetInstance()->Get_Component(eLayerID, pObjTag, pComponentTag, eID);
		T* pCasted = dynamic_cast<T*>(pComponent);

		return pCasted;
	}

	template <typename T>
	T* Get_GameObjectUnCheck(LAYERID eLayerID, const wstring& pObjTag)
	{
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject(eLayerID, pObjTag);
		return dynamic_cast<T*>(pGameObject);
	}

	template <typename T>
	T* Get_GameObject(LAYERID eLayerID, const wstring& pObjTag)
	{
		T* pCasted = Get_GameObjectUnCheck<T>(eLayerID, pObjTag);
		NULL_CHECK_RETURN(pCasted, nullptr);

		return pCasted;
	}

	inline 	void Get_AllGameObject(LAYERID eLayerID, const wstring& pObjTag, list<CGameObject*>& outList);

	inline CLayer* Get_Layer(LAYERID eLayerID);
	inline void AddGameObject(LAYERID eLayerID, const wstring& pObjTag, CGameObject* pObject);


	// ProtoMgr
	inline HRESULT Ready_Proto(const wstring& pProtoTag, CComponent* pComponent);
	inline CComponent* Clone_Proto(const wstring& pProtoTag);
	inline CComponent* Find_Proto(const wstring& pProtoTag);


	// Renderer

	inline void Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
	inline void Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	inline void Clear_RenderGroup(void);

	// collider
	inline void Add_CollisionCom(CCollisionCom* pCollision);
	inline void Add_StaticCollision(const _vec3& vCenter, _float fRadius);
	// run at only late update
	inline void GetOverlappedObject(OUT set<CGameObject*>& objList, const _vec3& vPos, _float fRadius);
	inline void Clear_ColliderAll();

	inline HRESULT	Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex);

	inline void Release_Utility(void);


#include "Export_Utility.inl"

END
#endif
