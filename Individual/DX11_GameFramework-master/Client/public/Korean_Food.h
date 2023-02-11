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

class CKorean_Food final : public CGameObject
{
public:
	enum TYPE { IDLE, PLAYER, SQUIRREL, TYPE_END };

	typedef struct tagObjectInfo
	{
		TYPE			eType = TYPE_END;
		_float3			f3Position = { 0.0f, 0.0f, 0.0f };
	}OBJECTINFO;

private:
	CKorean_Food(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKorean_Food(const CKorean_Food& rhs);
	virtual ~CKorean_Food() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

	virtual void	On_Collision(CGameObject* pOther) override;

private:
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	void			Player_Follow(const _double & TimeDelta);

private:
	OBJECTINFO		m_ObjectInfo = {};

public:
	static	CKorean_Food*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END