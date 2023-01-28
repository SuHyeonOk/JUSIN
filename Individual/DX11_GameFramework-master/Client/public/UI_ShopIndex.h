#pragma once

#include "Client_Defines.h"
#include "UI_.h"
#include "Skill_Manager.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_ShopIndex final : public CUI_
{
private:
	CUI_ShopIndex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShopIndex(const CUI_ShopIndex& rhs);
	virtual ~CUI_ShopIndex() = default;

public:
	void		Reset() {
		m_iIndexTexture = 1;
		m_iShopIndex = 0;
		m_eShopIndex = SHOPINDEX::SHOP_ONE;
	}

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
	void			IndexPosition();
	_bool			Calculation();

private:
	_int			m_iIndexTexture = 1;
	
	CSkill_Manager::PLAYERSKILL::SKILL			m_arr_ShopIndx[_int(SHOPINDEX::SHOP_END)] = { CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END };
	
	_int			m_iShopIndex = 0;
	SHOPINDEX		m_eShopIndex = { SHOPINDEX::SHOP_END };

public:
	static CUI_ShopIndex*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END