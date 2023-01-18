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

public:
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);


protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static const _tchar*			m_pTransformComTag;	

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_isCloned = { false };
	_float					m_fCamDistance = { 0.0 };	// 알파 블렌드로 그리는 객체마다. 카메라와의 거리르 가지고 있는다.
		
protected:
	/* 객체들이 사용해야 할 컴포넌트들을 보관한다. */
	map<const _tchar*, class CComponent*>			m_Components;

	class CTransform*								m_pTransformCom = nullptr;

protected:	
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

	void Compute_CamDistance();			// 알파 블렌드로 그리는 객체에서 매 프레임 마다 호출해서, 카메라 와의 거리를 게산한다.

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END