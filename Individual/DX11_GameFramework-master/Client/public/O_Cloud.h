#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CO_Cloud final : public CGameObject
{
public:
	enum CLOUDTYPE { ONE, TWO, THREE, CLOUDTYPE_END };

private:
	CO_Cloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CO_Cloud(const CO_Cloud& rhs);
	virtual ~CO_Cloud() = default;

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
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

private:
	void				Ready_CloudType();
	void				Move_Tick(const _double & TimeDelta);

private:
	CLOUDTYPE			eCloudType = CLOUDTYPE_END;

	_double				m_dMove_TimeAcc = { 0.0 };

public:
	static	CO_Cloud*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END