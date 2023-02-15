#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Icon final : public CUI_
{
private:
	CUI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Icon(const CUI_Icon& rhs);
	virtual ~CUI_Icon() = default;

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
	void			Icon_SetPos();

private:
	INVENTORYICON		m_eIconIndex = INVENTORYICON::ICON_END;

public:
	static CUI_Icon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END