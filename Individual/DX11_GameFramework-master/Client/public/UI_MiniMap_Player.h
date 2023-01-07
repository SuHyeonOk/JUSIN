#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_MninMap_Player final : public CUI_
{
private:
	CUI_MninMap_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MninMap_Player(const CUI_MninMap_Player& rhs);
	virtual ~CUI_MninMap_Player() = default;

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
	_float			m_fPlayer_X = 0.0f;
	_float			m_fPlayer_Y = 0.0f;

public:
	static CUI_MninMap_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END