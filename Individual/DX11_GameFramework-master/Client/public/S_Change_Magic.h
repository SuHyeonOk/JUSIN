#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CS_Change_Magic final : public CGameObject
{
private:
	CS_Change_Magic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Change_Magic(const CS_Change_Magic& rhs);
	virtual ~CS_Change_Magic() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;
	virtual	HRESULT		Render_XRay() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CShader*				m_pShaderXRayCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	CTransform*				m_pPlayer_TransformCom = nullptr;
	CNavigation*			m_pPlayer_NavigationCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Parts();	// �� ���� ������ ��ü��

private:
	HRESULT				Death_Set(const _double & TimeDelta);
	void				Effect_Create(const _double & TimeDelta);

private:
	void				KeyInput(const _double & TimeDelta);

	void				Skill_Tick(const _double & TimeDelta);
	void				Attack_Tick();
	void				Hit_Tick(const _double TimeDelta);

private:
	vector<CGameObject*>	m_SkillParts;
	_float3					m_f3Pos = _float3(0.f, 0.f, 0.f);
	
	_bool					m_OnMove = false;
	_float					m_fAngle = 0.0f;
	_float4					m_f4NewLook = { 0.0f, 0.0f, 0.0f, 0.0f };

	_double					m_dSkillClone_TimeAcc = 0;
	_double					m_dAttack_TimeAcc = 0;

	// ���̴�
	_bool		m_bShader_Hit = false;
	_double		m_dShader_Hit_TimeAcc = 0;

public:
	static CS_Change_Magic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END