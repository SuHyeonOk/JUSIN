#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CBoss_S_Cage final : public CGameObject
{
private:
	CBoss_S_Cage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_S_Cage(const CBoss_S_Cage& rhs);
	virtual ~CBoss_S_Cage() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	HRESULT			Skeleton_Create();
	void			Skeleton_Dead_Check();

private:
	_int			m_iSkeleton_Create_Count = 0;
	_bool			m_arrSkeleton_Dead_Count[5] = { false };
	_bool			m_bDead = false;
	_float			m_fAlpha = 0.6f;

public:
	static	CBoss_S_Cage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END