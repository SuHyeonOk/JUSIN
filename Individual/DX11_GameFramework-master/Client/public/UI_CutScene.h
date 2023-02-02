#pragma once

#include "Client_Defines.h"
#include "UI_.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_CutScene final : public CUI_
{
private:
	CUI_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CutScene(const CUI_CutScene& rhs);
	virtual ~CUI_CutScene() = default;

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
	void			TalkTexture_Tick(const _double & TimeDelta);
	void			CutSceneOne_Talk(const _double & TimeDelta);
	void			CutSceneTwo_Talk(const _double & TimeDelta);

private:
	_bool			m_bSize_Change = false;
	_int			m_Script_Count = { 0 };
	_double			m_Script_TimeAcc = { 0.0 };

	_bool			m_bSound = false;

public:
	static CUI_CutScene*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END