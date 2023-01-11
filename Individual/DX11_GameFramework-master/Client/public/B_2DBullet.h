#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CB_2DBullet final : public CGameObject
{
public:
	typedef struct tagBulletInfo
	{
		enum TOODYBULLET { STAR_BULLET, CIRCLE_BULLET, TOODYBULLET_END };
		TOODYBULLET	eToodyBullet;

		_float3		f3Start_Pos;
		_float3		f3Target_Pos;

		_float		fMonsterAttack = 0.0f;

		_uint		iCircle_Color = 0;	// CIRCLE_BULLET 을 사용하는 경우에만 몬스터 종류에 따라 다른 색

	}BULLETINFO;

private:
	CB_2DBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CB_2DBullet(const CB_2DBullet& rhs);
	virtual ~CB_2DBullet() = default;

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
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	BULLETINFO				m_tBulletInfo;

	_float4					m_f4Distance;
	_double					m_dBullet_TimeAcc = 0;
	_double					m_dEffect_TimeAcc = 0;

public:
	static	CB_2DBullet*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END