#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CSnail final : public CGameObject
{
private:
	CSnail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnail(const CSnail& rhs);
	virtual ~CSnail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual	HRESULT	Render_XRay() override;

	virtual void	On_Collision(CGameObject* pOther) override;

private:
	CShader*		m_pShaderCom = nullptr;
	CShader*		m_pShaderXRayCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	HRESULT			Load_Position();

	void			NextPosition(const _double & TimeDelta);

	void			UI_Tick(const _double & TimeDelta);

private:
	vector<_float3>	m_vecNextPosition;

	_int			m_iIndex = { 0 };
	_bool			m_bArrive = { false };

	_float			m_fCurrent_Percentage = { 0.0f };
	_float			m_fNext_Percentage = { 0.0f };

	_double			m_bMove_TimeAcc = { 0.0 };

public:
	static	CSnail*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END