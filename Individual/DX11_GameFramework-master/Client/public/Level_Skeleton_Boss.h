#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Skleton_Boss final : public CLevel
{
private:
	CLevel_Skleton_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Skleton_Boss() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_PreviousData();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Finn(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Jake(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Boss(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);

private:
	void	ImGui();

private:
	wstring			m_wstObjName;
	_float3			m_f3ClickPos = _float3(0.f, 0.f, 0.f);

public:
	static CLevel_Skleton_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END