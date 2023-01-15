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

class CBoss_Skeleton final : public CGameObject
{
	enum STATE { DANCE, HIT, DIE, STATE_END };

private:
	CBoss_Skeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Skeleton(const CBoss_Skeleton& rhs);
	virtual ~CBoss_Skeleton() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

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
	void			Monster_Tick(const _double& TimeDelta);

private:
	STATE			m_eState = STATE_END;

	_float			m_fAlpha = 1.0f;

public:
	static	CBoss_Skeleton*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END