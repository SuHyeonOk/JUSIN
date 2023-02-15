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

class CB_AnimBullet final : public CGameObject
{
public:
	typedef struct tagRandomBulletInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum ANIMBULLETTYPE { TYPE_ROOTS, TYPE_END };

		ANIMBULLETTYPE	eBulletType = TYPE_END;

		_float3		f3Start_Pos;
		_float3		f3Target_Pos;

		_float		fMonsterAttack = 0.0f;

	}ANIMBULLETINFO;

private:
	CB_AnimBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CB_AnimBullet(const CB_AnimBullet& rhs);
	virtual ~CB_AnimBullet() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	ANIMBULLETINFO		m_tBulletInfo;

	_float4				m_f4Distance;
	_double				m_dBullet_TimeAcc = 0;

public:
	static CB_AnimBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END