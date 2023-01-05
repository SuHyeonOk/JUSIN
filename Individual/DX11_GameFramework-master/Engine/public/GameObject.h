#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc
	{	
		CTransform::TRANSFORMDESC		TransformDesc;
	}GAMEOBJECTDESC;

protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static const _tchar*			m_pTransformComTag;

public:
	CComponent*	Get_ComponentPtr(const _tchar* pComponentTag); // ◈
	_float Get_CamDistance() const { return m_fCamDistance; }

	_bool		Get_Dead() { return m_bDead; }
	void		Set_Dead() { m_bDead = true; }
	wstring		Get_Tag() { return m_wsTag; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void	On_Collision(CGameObject* pOther) {};

public: /* imgui */
	// 이 오브젝트가 가지고 있는 component의 Imgui_RenderProtpery함수를 실행하는 함수.
	void Imgui_RenderComponentProperties();

	// 이 오브젝트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty() {};

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	_bool					m_isCloned = false;

protected:
	wstring		m_wsTag = L"";		// 자식의 ObjTag 를 저장하기 위한 용도

protected:
	/* 객체들이 사용해야 할 컴포넌트들을 보관한다. */
	map<const _tchar*, class CComponent*>			m_Components;
	class CTransform*								m_pTransformCom = nullptr;

protected:	
	HRESULT				Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	class CComponent*	Find_Component(const _tchar* pComponentTag);
	HRESULT				Compute_CamZ(_fvector vWorldPos);

private:
	_float		m_fCamDistance = 0.0f;	// 객체와 카메라의 거리
	_bool		m_bDead = false;		// 객체가 사라져야 한다면 , true

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END