#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_CurrentIcon final : public CUI_
{
private:
	CUI_CurrentIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CurrentIcon(const CUI_CurrentIcon& rhs);
	virtual ~CUI_CurrentIcon() = default;

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
	_float			m_fCurrentIndexY = 123.333f;

	INVENTORYICON	m_eCurrentIcon_Index = INVENTORYICON::ICON_END;			// 실제 사용용
	_int			m_iCurrentIcon_Index = 0;								// 연산용

public:
	static CUI_CurrentIcon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END