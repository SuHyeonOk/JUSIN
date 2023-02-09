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

class CKnives_Rain final : public CGameObject
{
public:
	void	Set_Rain() {
		m_bRain = true;
	}

private:
	CKnives_Rain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKnives_Rain(const CKnives_Rain& rhs);
	virtual ~CKnives_Rain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	void			Rain_Tick(const _double & TimeDelta);

private:
	_bool			m_bRain = false;
	
	_double			m_dTimeAcc = { 0.0 };
	_double			m_dRain_TimeAcc = { 0.0 };
	_int			m_iRain_Count = { 0 };

public:
	static	CKnives_Rain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END