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
	CComponent*	Get_ComponentPtr(const _tchar* pComponentTag); // ��
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
	// �� ������Ʈ�� ������ �ִ� component�� Imgui_RenderProtpery�Լ��� �����ϴ� �Լ�.
	void Imgui_RenderComponentProperties();

	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
	virtual void Imgui_RenderProperty() {};

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	_bool					m_isCloned = false;

protected:
	wstring		m_wsTag = L"";		// �ڽ��� ObjTag �� �����ϱ� ���� �뵵

protected:
	/* ��ü���� ����ؾ� �� ������Ʈ���� �����Ѵ�. */
	map<const _tchar*, class CComponent*>			m_Components;
	class CTransform*								m_pTransformCom = nullptr;

protected:	
	HRESULT				Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	class CComponent*	Find_Component(const _tchar* pComponentTag);
	HRESULT				Compute_CamZ(_fvector vWorldPos);

private:
	_float		m_fCamDistance = 0.0f;	// ��ü�� ī�޶��� �Ÿ�
	_bool		m_bDead = false;		// ��ü�� ������� �Ѵٸ� , true

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END