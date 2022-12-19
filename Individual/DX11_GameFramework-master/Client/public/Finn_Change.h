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

class CFinn_Change final : public CGameObject
{
public:
	typedef struct tagPlayerChangeInfo
	{
		enum CHANGE { MAGIC, CHANGE_END };
		CHANGE		eChange;

		_float3		Pos;

	}CHANGEINFO;

private:
	CFinn_Change(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFinn_Change(const CFinn_Change& rhs);
	virtual ~CFinn_Change() = default;

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

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	CHANGEINFO				m_tChangeInfo;

public:
	static	CFinn_Change* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END