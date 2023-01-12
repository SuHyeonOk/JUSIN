#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Skill_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CS_Jake_Son final : public CGameObject
{
public:
	typedef struct tagJakeSonInfo
	{
		enum JAKESON { JAKE_SON_A, JAKE_SON_B, JAKE_SON_C, JAKE_SON_D, JAKE_SON_E, JAKE_SON_END };
		JAKESON		eJakeSon = JAKE_SON_END;

		_float3		fPos = _float3(0.f, 0.f, 0.f);

	}JAKESONINFO;

private:
	CS_Jake_Son(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Jake_Son(const CS_Jake_Son& rhs);
	virtual ~CS_Jake_Son() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	JAKESONINFO		m_tJakeSonInfo;

public:
	static	CS_Jake_Son*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END