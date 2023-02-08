#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CMap_Skleton_Boss final : public CGameObject
{
private:
	CMap_Skleton_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Skleton_Boss(const CMap_Skleton_Boss& rhs);
	virtual ~CMap_Skleton_Boss() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_ShadowDepth() override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

public:
	static	CMap_Skleton_Boss*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END