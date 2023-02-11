#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Snail final : public CUI_
{
private:
	CUI_Snail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Snail(const CUI_Snail& rhs);
	virtual ~CUI_Snail() = default;

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
	static CUI_Snail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END