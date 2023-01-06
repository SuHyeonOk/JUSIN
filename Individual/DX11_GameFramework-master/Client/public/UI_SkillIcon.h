#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_SkillIcon final : public CUI_
{
public:
	enum SKILLICON { ICON_ONE, ICON_TWO, ICON_THREE, ICON_FOUR, SKILLICON_END };

private:
	CUI_SkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillIcon(const CUI_SkillIcon& rhs);
	virtual ~CUI_SkillIcon() = default;

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
	SKILLICON		m_eSkillIcon = SKILLICON_END;

public:
	static CUI_SkillIcon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END