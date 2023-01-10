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
	CTransform*				m_pPlayer_TransformCom = nullptr;
	CNavigation*			m_pPlayer_NavigationCom = nullptr;
	CCollider*				m_pPlayer_ColliderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Parts();	// 나 에서 생성할 객체들

private:
	void	Parts_Tick(const _double & TimeDelta);
	void	Parts_LateTick(const _double & TimeDelta);

private:
	void				Death_Set(const _double & TimeDelta);
	void				Effect_Create(const _double & TimeDelta);

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
	_float3					m_f3Pos = _float3(0.f, 0.f, 0.f);

	_bool					m_OnMove = false;

	_bool					m_bStun = false;			// 스턴
	_uint					m_iStun_Count = 0;			// 스턴 애니메이션 두 번

	_float					m_fOriginal_Player_Attack = 0.0f;
	_float					m_fSword_Attack = 0;
	_float					m_fCat_Attack = 0;

	_double					m_bSkillClone_TimeAcc = 0;
	_double					m_dCat_Attack_TimeAcc = 0;
	_double					m_dEffect_TimeAcc = 0;
	_int					m_bEffect_Count = 0;

	// 셰이더
	_bool		m_bShader_Hit = false;
	_double		m_dShader_Hit_TimeAcc = 0;

public:
	static CS_Fiona* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END