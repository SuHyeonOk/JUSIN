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

class CW_PigWarrior final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;
		
		enum WARRIORTYPE { SWORD, CYLINDER, WARRIORTYPE_END };

		WARRIORTYPE			eWarriorType = WARRIORTYPE_END;
		_int				iAttack;

	}WEAPONDESC;

private:
	CW_PigWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CW_PigWarrior(const CW_PigWarrior& rhs);
	virtual ~CW_PigWarrior() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	On_Collision(CGameObject* pOther) override;

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

public:
	static CW_PigWarrior* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END