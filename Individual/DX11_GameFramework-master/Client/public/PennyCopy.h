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

class CPennyCopy final : public CGameObject
{
public:
	enum STATE { IDLE, MOVE, HIT, DIE, STATE_END };

private:
	CPennyCopy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPennyCopy(const CPennyCopy& rhs);
	virtual ~CPennyCopy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

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
	void			Anim_Change(const _double & TimeDelta);
	void			Move_Tick(const _double& TimeDelta);
	void			Die_Tick(const _double& TimeDelta);

	_bool			Random_Move(_double TimeDelta, _float fRange = 3.5f);
	void			PennyDead_Check();

private:
	STATE				m_eState = STATE_END;

	_float				m_fAlpha = 1.0f;
	_float4				m_f4CenterPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	// RandomMove();
	_bool				m_bRandomTime = false;
	_double				m_bRandom_TimeAcc = 0;
	_double				m_bRandomMove_TimeAcc = 0;

public:
	static	CPennyCopy*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END