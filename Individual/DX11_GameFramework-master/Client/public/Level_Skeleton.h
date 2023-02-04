#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Skleton final : public CLevel
{
private:
	CLevel_Skleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Skleton() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT Ready_Lights(const _tchar* pLayerTag);
	HRESULT Ready_PreviousData();
	HRESULT Ready_Layer_Penny();
	HRESULT Ready_Layer_Npc();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Finn(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Jake(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Jake_Son(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);

private:
	void	ImGui();
	void	ImGui_Light();
	void	ImGui_Food();
	void	ImGui_Coin();
	void	ImGui_Page();
	void	ImGui_Item();
	void	ImGui_Object();
	void	ImGui_Monster();
	void	ImGui_Envionment();

private:
	HRESULT	Load_Light();
	HRESULT	Load_Food();
	HRESULT	Load_Coin();
	HRESULT	Load_Page();
	HRESULT	Load_Item();
	HRESULT	Load_Object();
	HRESULT	Load_Monster();
	HRESULT	Load_Envionment();

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

	// Light
	vector<_float3>		m_vecfloat3;

	// ▶ 끝

public:
	static CLevel_Skleton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END