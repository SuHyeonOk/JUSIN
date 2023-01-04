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
public:
	typedef struct tagPlayerChangeInfo
	{
		enum CHANGE { FINN, JAKE, CHANGE_END };
		CHANGE		eChange;

		_float3		f3Pos;

	}CHANGEINFO;

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

private:
	void				KeyInput(const _double & TimeDelta);

	void				Skill_Tick(const _double & TimeDelta);
	void				Attack_Tick();
	void				Hit_Tick();

private:
	CHANGEINFO				m_tChangeInfo;
	
	_bool					m_OnMove = false;
	_bool					m_bMonster_Crash = false;

public:
	static CS_Change_Magic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END