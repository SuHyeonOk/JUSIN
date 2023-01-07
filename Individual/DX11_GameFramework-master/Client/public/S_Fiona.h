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

class CS_Fiona final : public CGameObject
{
private:
	CS_Fiona(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Fiona(const CS_Fiona& rhs);
	virtual ~CS_Fiona() = default;

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
	CNavigation*			m_pNavigationCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Parts();	// �� ���� ������ ��ü��

private:
	void				KeyInput(const _double & TimeDelta);

	void				Skill_Tick(const _double & TimeDelta);
	void				Attack_Tick();
	void				Cat_Tick();
	void				Hit_Tick(const _double & TimeDelta);
	void				Stun_Tick();
	void				Dance_Tick();

private:
	vector<CGameObject*>	m_SkillParts;
	_bool					m_OnMove = false;

	_double					m_dHit_TimeAcc = 0;

	_bool					m_bStun = false;			// ����
	_uint					m_iStun_Count = 0;			// ���� �ִϸ��̼� �� ��

	_float					m_fOriginal_Player_Attack = 0.0f;

public:
	static CS_Fiona* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END