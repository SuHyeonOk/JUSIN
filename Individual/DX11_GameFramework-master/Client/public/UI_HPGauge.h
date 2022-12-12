#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_HPGauge final : public CUI_
{
private:
	CUI_HPGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_HPGauge(const CUI_HPGauge& rhs);
	virtual ~CUI_HPGauge() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CTexture*				m_pTextureCom = nullptr;

private:
	virtual			HRESULT SetUp_Components() override;
	virtual			HRESULT SetUp_ShaderResources() override;

public:
	static		CUI_HPGauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END