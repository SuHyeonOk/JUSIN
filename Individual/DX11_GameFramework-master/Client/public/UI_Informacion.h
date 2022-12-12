#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Informacion final : public CUI_
{
public:
	enum TEXTTYPE { TYPE_FINN, TYPE_JAKE, TYPE_END };

private:
	CUI_Informacion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Informacion(const CUI_Informacion& rhs);
	virtual ~CUI_Informacion() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CTexture*			m_pTextureCom[TYPE_END] = { nullptr };

private:
	virtual			HRESULT SetUp_Components() override;
	virtual			HRESULT SetUp_ShaderResources() override;

public:
	static CUI_Informacion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END