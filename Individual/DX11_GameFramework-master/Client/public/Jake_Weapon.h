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

class CJake_Weapon final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;

		CObj_Manager::PLAYERINFO::JAKEWEAPON	eWeaponType;

	}WEAPONDESC;

private:
	CJake_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJake_Weapon(const CJake_Weapon& rhs);
	virtual ~CJake_Weapon() = default;

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

	_bool					m_bEffect_Shielddddddddddddddddd = false;
	_double					m_dEffect_Shield_TimeAcc = 0;
	UINT32 m_Test = 27000;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CJake_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END