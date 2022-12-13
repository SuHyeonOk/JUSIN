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

class CB_3DAnimBullet final : public CGameObject
{
public:
	typedef struct tagRandomBulletInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum ANIMBULLETTYPE { TYPE_ROOTS, TYPE_END };

		ANIMBULLETTYPE	eBulletType;

		_float3		f3Pos;

	}ANIMBULLETINFO;

private:
	CB_3DAnimBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CB_3DAnimBullet(const CB_3DAnimBullet& rhs);
	virtual ~CB_3DAnimBullet() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	
private:
	ANIMBULLETINFO		m_tBulletInfo;

private:
	_bool				m_bPlayer_Collider = false;
	_bool				m_bDead_TimeAcc = 0;

public:
	static	CB_3DAnimBullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END