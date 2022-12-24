#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Npc();
	HRESULT Ready_Layer_UI_Informacion(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Finn(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Jake(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map_Garden(const _tchar* pLayerTag);

private:
	void	ImGui();
	void	ImGui_Food();
	void	ImGui_Coin();
	void	ImGui_Page();
	void	ImGui_Item();
	void	ImGui_Npc();
	void	ImGui_Object();
	void	ImGui_Monster();

private:
	void	Load_Food();
	void	Load_Coin();
	void	Load_Page();
	void	Load_Item();
	void	Load_Npc();
	void	Load_Object();
	void	Load_Monster();

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
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END