#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Monstser_BarBack final : public CUI_
{
private:
	CUI_Monstser_BarBack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Monstser_BarBack(const CUI_Monstser_BarBack& rhs);
	virtual ~CUI_Monstser_BarBack() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CTexture*			m_pTextureCom = { nullptr };

private:
	virtual			HRESULT SetUp_Components() override;
	virtual			HRESULT SetUp_ShaderResources() override;

private:
	_float			m_fHPGauge = 1.0f;
	_double			m_dHPGauge_TimeAcc = 0;

public:
	static CUI_Monstser_BarBack*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END