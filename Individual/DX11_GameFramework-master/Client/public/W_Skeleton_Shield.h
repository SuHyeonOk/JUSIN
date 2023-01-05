#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
class CCollider;
END

BEGIN(Client)

class CW_Skeleton_Shield final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;
		
		enum WARRIORTYPE { SWORD, SHIELD, WARRIORTYPE_END };

		WARRIORTYPE			eWarriorType = WARRIORTYPE_END;
		_float				fAttack = 0.0f;
		_float				fExp = 0.0f;

	}WEAPONDESC;

private:
	CW_Skeleton_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CW_Skeleton_Shield(const CW_Skeleton_Shield& rhs);
	virtual ~CW_Skeleton_Shield() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	On_Collision(CGameObject* pOther) override;

public:
	void			Set_Attack_Collision(_bool bAttack) { m_bAttack = bAttack; }

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
	_bool					m_bAttack = false;

public:
	static CW_Skeleton_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END