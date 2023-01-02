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

class CS_PaintWork_Parents final : public CGameObject
{
public:
	typedef struct tagPaintWorkInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum PAINTWORK { BLUE, MAGENTA, YELLOW, TYPE_END };

		PAINTWORK	ePaintWork = TYPE_END;

		_float3		f3Pos;
		_float4		f4Look;

	}PAINTWORKINFO;

public:
	CS_PaintWork_Parents::CS_PaintWork_Parents(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_PaintWork_Parents::CS_PaintWork_Parents(const CS_PaintWork_Parents& rhs);
	virtual ~CS_PaintWork_Parents() = default;

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
	PAINTWORKINFO		m_tBulletInfo;
	
	_float4				m_f4Distance;
	_double				m_dBullet_TimeAcc = 0;

	_float4				m_f4Blue_Look;
	_float4				m_f4Maenta_Look;
	_float4				m_f4Yellow_Look;

public:
	static	CS_PaintWork_Parents*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END