#pragma once
#include <functional>
#include "Arrow.h"
#include "Particle.h"
#include "SkeletalCube.h"
#include "SkeletalGhostTrail.h"

class CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T, typename... Args>
	static T* Create(Args... args)
	{
		return T::Create(std::forward<Args>(args)...);
	}

	// read factory after graphic dev inited
	static void Ready_Factories(LPDIRECT3DDEVICE9 pGraphicDev);
	static void ReleaseGraphicDev() { Safe_Release(s_pGraphicDev); }

protected:
	static LPDIRECT3DDEVICE9 s_pGraphicDev;
};

class CPlayerFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag)
	{
		T* pCasted = dynamic_cast<T*>(s_mapPlayerSpawner.find(strFactoryTag)->second());
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_PLAYER, wstrObjTag, pCasted);
		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _matrix& matWorld)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_WorldDecompose(matWorld);

		return pCasted;
	}

	static void Ready_PlayerFactory();

private:
	static map<string, std::function<CGameObject*()>> s_mapPlayerSpawner;
};

class CEnemyFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _matrix& matWorld)
	{
		T* pCasted = dynamic_cast<T*>(s_mapEnemySpawner.find(strFactoryTag)->second());
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_ENEMY, wstrObjTag, pCasted);

		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_WorldDecompose(matWorld);

		return pCasted;
	}

	static void Ready_EnemyFactory();

private:
	static map<string, std::function<CGameObject*()>> s_mapEnemySpawner;
};

class CEffectFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag)
	{
		T* pCasted = dynamic_cast<T*>(s_mapEffectSpawner.find(strFactoryTag)->second());
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_EFFECT, wstrObjTag, pCasted);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _matrix& matWorld)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		CTransform* pTrans = pCasted->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_WorldDecompose(matWorld);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _vec3& vPos)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->m_vInfo[INFO_POS] = vPos;
		pTrans->Update_Component(0.f);

		return pCasted;
	}

	static CAttack_Range_Circle* AttackRange_Create(const string& strFactoryTag, const wstring& wstrObjTag, const _vec3& vPos, 
		ATTACKCIRCLETYPE _type, _vec3 _minsize,_vec3 _maxsize, _uint _totalframe, _uint _nextframe)
	{
		ATKRNGOPTION circleoption;

		circleoption._eRangeType = _type;
		circleoption._vMinSize = _minsize;
		circleoption._vMaxSize = _maxsize;
		circleoption._iNextFrame = _nextframe;
		circleoption._iTotalFrame = _totalframe;
		circleoption._fAcc = 0.f;
		circleoption._fMaxAcc = (_float)_nextframe / 60.f; // 37/60 0.5

		circleoption._fLifeTime = (_float)_totalframe / 60.f;
		//pCasted->SetLerp(&circleoption);

		CAttack_Range_Circle* pCasted = dynamic_cast<CAttack_Range_Circle*>(s_mapKoukuEffectSpawner.find(strFactoryTag)->second(circleoption));
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_EFFECT, wstrObjTag, pCasted);


		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->m_vInfo[INFO_POS] = vPos;
		pTrans->m_vInfo[INFO_POS].y += 1.f;
		pTrans->m_vScale *= 1.5f;
		pTrans->Update_Component(0.f);

		return pCasted;
	}

	static void Ready_EffectFactory();

private:
	static map<string, std::function<CGameObject*()>> s_mapEffectSpawner;
	static map<string, std::function<CGameObject*(const ATKRNGOPTION& circleOption)>> s_mapKoukuEffectSpawner;
};

class CEnvFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag)
	{
		T* pCasted = dynamic_cast<T*>(s_mapEnvSpawner.find(strFactoryTag)->second());
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_ENV, wstrObjTag, pCasted);

		return pCasted;
	}
	static void Ready_EnvFactory();

private:
	static map<string, std::function<CGameObject*()>> s_mapEnvSpawner;
};

class CBulletFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, ArrowParams tArrowParams)
	{
		T* pCasted = dynamic_cast<T*>(s_mapBulletSpawner.find(strFactoryTag)->second(tArrowParams));
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_BULLET, wstrObjTag, pCasted);
	
		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, ArrowParams tArrowParams, const _vec3& vPos, const _vec3& vLookAt)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag, tArrowParams);

		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->m_vInfo[INFO_POS] = vPos;
		pTrans->RotateToLookAt(vLookAt);

		return pCasted;
	}

	static void Ready_BulletFactory();

private:
	static map<string, std::function<CGameObject*(ArrowParams)>> s_mapBulletSpawner;

};

class CObjectFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag)
	{
		T* pCasted = dynamic_cast<T*>(s_mapObjectSpawner.find(strFactoryTag)->second());
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_GAMEOBJ, wstrObjTag, pCasted);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _matrix& matWorld)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		if (strFactoryTag == "Box")
		{
			Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			pTrans->Set_WorldDecompose(matWorld);
			pTrans->Update_Component(0.f);
		}
		else
		{
			Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			pTrans->Set_WorldDecompose(matWorld);
			pTrans->Update_Component(0.f);
		}

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _vec3& vPos)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		if (strFactoryTag == "Box")
		{
			Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			pTrans->m_vInfo[INFO_POS] = vPos;
			pTrans->Update_Component(0.f);
		}
		else
		{
			Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			pTrans->m_vInfo[INFO_POS] = vPos;
			pTrans->Update_Component(0.f);
		}

		return pCasted;
	}

	static CSkeletalGhostTrail* CreateGhostTrail(const string& strFactoryTag, const wstring& wstrObjTag, CSkeletalCube* pTarget, const _matrix& matWorld)
	{
		CSkeletalGhostTrail* pCasted = dynamic_cast<CSkeletalGhostTrail*>(s_mapGhostSpawner.find(strFactoryTag)->second(pTarget));
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_GAMEOBJ, wstrObjTag, pCasted);

		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_WorldDecompose(matWorld);
		pTrans->Update_Component(0.f);

		return pCasted;
	}

	static void Ready_ObjectFactory();

private:
	static map<string, std::function<CGameObject*()>> s_mapObjectSpawner;
	static map<string, std::function<CGameObject*(CSkeletalCube*)>> s_mapGhostSpawner;
};

class CItemFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag)
	{
		T* pCasted = dynamic_cast<T*>(s_mapItemSpawner.find(strFactoryTag)->second());
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_ITEM, wstrObjTag, pCasted);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _matrix& matWorld)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_WorldDecompose(matWorld);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _vec3& vPos)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		CTransform* pTrans = pCasted->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->m_vInfo[INFO_POS] = vPos;
		pTrans->Update_Component(0.f);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const ITEMSTATE& eState)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		pCasted->SetState(eState);
		return pCasted;
	}

	static void Ready_ItemFactory();

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, const _vec3& vPos, const ITEMSTATE& eState)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag);

		pCasted->SetState(eState);
		Engine::CTransform* pTrans = pCasted->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->m_vInfo[INFO_POS] = vPos;
		pTrans->Update_Component(0.f);

		return pCasted;
	}


private:
	static map<string, std::function<CGameObject*()>> s_mapItemSpawner;
};

class CUIFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, _uint iTexNum)
	{
		T* pCasted = dynamic_cast<T*>(s_mapUISpawner.find(strFactoryTag)->second(iTexNum));
		_ASSERT_CRASH(pCasted != nullptr);
		Engine::AddGameObject(LAYER_UI, wstrObjTag, pCasted);

		return pCasted;
	}

	template<typename T>
	static T* Create(const string& strFactoryTag, const wstring& wstrObjTag, _uint iTexNum, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY)
	{
		T* pCasted = Create<T>(strFactoryTag, wstrObjTag, iTexNum);

		CTransform* pTrans = pCasted->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_Scale(fSizeX, fSizeY, 1.f);
		pTrans->Set_Pos(fX - WINCX * 0.5f, -fY + WINCY * 0.5f, 0.f);
		pTrans->Update_Component(0.f);

		return pCasted;
	}

	template<typename T>
	static T* CreateNoLayer(const string& strFactoryTag, _uint iTexNum, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY)
	{
		T* pCasted = dynamic_cast<T*>(s_mapUISpawner.find(strFactoryTag)->second(iTexNum));
		_ASSERT_CRASH(pCasted != nullptr);

		CTransform* pTrans = pCasted->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		pTrans->Set_Scale(fSizeX, fSizeY, 1.f);
		pTrans->Set_Pos(fX - WINCX * 0.5f, -fY + WINCY * 0.5f, 0.f);
		pTrans->Update_Component(0.f);
		return pCasted;
	}




	static void Ready_UIFactory();

private:
	static map<string, std::function<CGameObject*(_uint)>> s_mapUISpawner;
};

class CSceneFactory : CAbstFactory
{
	friend class CImGuiMgr;
public:
	static void LoadScene(const string& strLoadingTag, const string& strSceneTag, bool bDeletePrev = true, long long delay = 1000)
	{
		CScene* pLoading = s_mapLoadingSpawner.find(strLoadingTag)->second();

		if (bDeletePrev)
		{
			Engine::SwitchSceneLoadingDeletePrev(pLoading, s_mapSceneSpawner.find(strSceneTag)->second, delay);
		}
		else
		{
			Engine::SwitchSceneLoading(pLoading, s_mapSceneSpawner.find(strSceneTag)->second, delay);
		}
	}

	static void Ready_SceneFactory();

private:
	static map<string, std::function<CScene*()>> s_mapLoadingSpawner;
	static map<string, std::function<CScene*()>> s_mapSceneSpawner;

};