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

class CS_PaintWork final : public CGameObject
{
public:
	typedef struct tagPaintWorkInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum PAINTWORK { BLUE, MAGENTA, YELLOW, TYPE_END };

		PAINTWORK	ePaintWork = TYPE_END;

		_float3		f3Pos = { 0.0f, 0.0f, 0.0f };
		_float4		f4Look = { 0.0f, 0.0f, 0.0f, 0.0f };
		_float		fAttack = 0.0f;

	}PAINTWORKINFO;

public:
	CS_PaintWork::CS_PaintWork(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_PaintWork::CS_PaintWork(const CS_PaintWork& rhs);
	virtual ~CS_PaintWork() = default;

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
	void				Effect_Tick(const _double & TimeDelta);

private:
	PAINTWORKINFO		m_tBulletInfo;
	
	_float4				m_f4Distance;
	_double				m_dBullet_TimeAcc = 0;

	_bool				m_bMove = false;
	_double				m_dMove_TimeAcc = 0;

	_double				m_dEffect_TimeAcc = { 0.0 };

public:
	static	CS_PaintWork*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END