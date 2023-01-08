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

class CS_Marceline final : public CGameObject
{
	enum STATE { DOWN, SONG, UP, STATE_END };

private:
	CS_Marceline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Marceline(const CS_Marceline& rhs);
	virtual ~CS_Marceline() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	void			Animation_Tick();
	void			State_Tick();

private:
	void			Effect_Create(const _double& TimeDelta);

private:
	STATE			m_eState = STATE_END;
	_int			m_eAnim_Count = 0;

	_double			m_dEffect_Sound_TimeAcc = 0;
	_double			m_dEffect_Waves_TimeAcc = 0;

public:
	static	CS_Marceline* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free() override;
};

END