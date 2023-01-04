#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Obj_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
class CCollider;
END

BEGIN(Client)

class CFinn_Weapon final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;

		CObj_Manager::PLAYERINFO::PLAYERWEAPON	eSwordType;

	}WEAPONDESC;

private:
	CFinn_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFinn_Weapon(const CFinn_Weapon& rhs);
	virtual ~CFinn_Weapon() = default;

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

	WEAPONDESC				m_WeaponDesc;

	_float4x4				m_SocketMatrix;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	_bool		m_bMonster_Crash = false;

public:
	static CFinn_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END