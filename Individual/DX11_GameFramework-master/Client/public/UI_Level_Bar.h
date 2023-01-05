#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Level_Bar final : public CUI_
{
private:
	CUI_Level_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Level_Bar(const CUI_Level_Bar& rhs);
	virtual ~CUI_Level_Bar() = default;

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
	_float			m_fAlpha = 1.f;
	_double			m_dAlpha_TimeAcc = 0;

public:
	static CUI_Level_Bar*Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END