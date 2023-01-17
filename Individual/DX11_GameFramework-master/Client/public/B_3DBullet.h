#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CB_3DBullet final : public CGameObject
{
public:
	typedef struct tagRandomBulletInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum BULLETTYPE { TYPE_MAGIC, TYPE_SKELETON, TYPE_ROCK, TYPE_END };

		BULLETTYPE	eBulletType = TYPE_END;

		_float3		f3Start_Pos;
		_float3		f3Target_Pos;

		_float		fMonsterAttack = 0.0f;

	}NONANIMBULLETINFO;

public:
	CB_3DBullet::CB_3DBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CB_3DBullet::CB_3DBullet(const CB_3DBullet& rhs);
	virtual ~CB_3DBullet() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	void					Magic_Tick(const _double & TimeDelta);
	void					Magic_LateTick(const _double & TimeDelta);

	void					Effect_Tick(const _double & TimeDelta);

private:
	NONANIMBULLETINFO		m_tBulletInfo;
	
	_float4				m_f4Distance;
	_double				m_dBullet_TimeAcc = 0;

	// Effect
	_double				m_dEffect_TimeAcc = 0;

public:
	static	CB_3DBullet*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END