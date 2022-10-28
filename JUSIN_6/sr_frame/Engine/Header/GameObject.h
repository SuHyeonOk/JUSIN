#pragma once

#include "Component.h"
#include "Engine_Include.h"
#include "ProtoMgr.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	template<typename T>
	T* Get_Component(const wstring& pComponentTag, COMPONENTID eID)
	{
		CComponent*		pComponent = Find_Component(pComponentTag, eID);

		NULL_CHECK_RETURN(pComponent, nullptr);

		T* pT = dynamic_cast<T*>(pComponent);
		NULL_CHECK_RETURN(pT, nullptr); //"insert component fail : Fail to cast"

		return pT;
	}

	template<typename T>
	T* Add_Component(const wstring& pProtoTag, const wstring& pComponentTag, COMPONENTID eID)
	{
		// get proto check
		CComponent* pComponent = CProtoMgr::GetInstance()->Clone_Proto(pProtoTag);
		NULL_CHECK_RETURN(pComponent, nullptr);// "insert component fail : pProtoTag is not exist"

		// cast check
		T* pT = dynamic_cast<T*>(pComponent);
		NULL_CHECK_RETURN(pT, nullptr); //"insert component fail : Fail to cast"

		_bool bSuccess = m_mapComponent[eID].insert({ pComponentTag, pComponent }).second;

		// insert check(duplicated)
		_ASSERT_CRASH(bSuccess == true); // "insert component fail : pComponentTag is duplicated"

		return pT;
	}

	bool Has_Component(const wstring& pComponentTag, COMPONENTID eID)
	{
		return Find_Component(pComponentTag, eID) != nullptr;
	}

	_bool IsRemote() const { return m_bRemote; }


public:
	virtual		HRESULT		Ready_Object(void);
	virtual		_int		Update_Object(const _float& fTimeDelta);
	virtual		void		LateUpdate_Object(void);
	virtual		void		Render_Object(void);
	void		Delete_Component(const wstring& pComponentTag, COMPONENTID eID);
	void SetID(_uint iID) {m_iID = iID;}
	_uint GetID() const { return m_iID; }

private:
	CComponent*		Find_Component(const wstring& pComponentTag, COMPONENTID eID);

protected:
	LPDIRECT3DDEVICE9					m_pGraphicDev;
	map<wstring, CComponent*>		m_mapComponent[ID_END];

	_bool m_bRemote = false;
	_uint m_iID = 0;

public:
	virtual void	Free(void);

public:
	_uint m_iRenderPriority = 10;
	// float GetRandomFloat(float lowBound, float highBound);
	//
	// void GetRandomVector(
	// 	_vec3* out,
	// 	_vec3* min,
	// 	_vec3* max);
	//
	// DWORD FtoDw(float f);

};

END