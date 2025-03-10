#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MiniGame final : public CLevel
{
private:
	CLevel_MiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MiniGame() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Game();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_FinnAndJake(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);

private:
	void	ImGui();
	void	ImGui_Food();
	void	ImGui_Coin();
	void	ImGui_Item();
	void	ImGui_Object();
	void	ImGui_Monster();
	void	ImGui_Envionment();

	void	ImGui_MiniGame();

private:
	HRESULT	Load_Food();
	HRESULT	Load_Coin();
	HRESULT	Load_Item();
	HRESULT	Load_Object();
	HRESULT	Load_Monster();
	HRESULT	Load_Envionment();

	HRESULT	Load_MiniGame();

private:
	const _tchar*	m_szObjName;
	wstring			m_wstObjName;
	_float3			m_f3ClickPos = _float3(0.f, 0.f, 0.f);

	// ▶ @ ImGui 를 사용하지 않을 때 주석!
	// Food
	_uint			m_iRoyal_Tart_Count = 0;
	_uint			m_iBurrito_Count = 0;

	// Coin
	_uint			m_iCoinBronze_Count = 0;
	_uint			m_iCoinSilver_Count = 0;
	_uint			m_iCoinGold_Count = 0;

	// Page
	_uint			m_iPage_1_Count = 0;

	_uint			m_iItem_Count = 0;
	_uint			m_iNpc_Count = 0;
	_uint			m_iObject_Count = 0;
	_uint			m_iMonster_Count = 0;

	// ▶ 끝

public:
	static CLevel_MiniGame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END