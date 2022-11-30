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
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map_Garden(const _tchar* pLayerTag);

private:
	void	ImGuiTest();

private:
	void	FoodLoad();

private:
	typedef struct tagObjectInfo
	{
		_tchar		ObjName[MAX_PATH];
		_float3		ObjPos;
	}OBJINFO;

	vector<OBJINFO>	m_vecObjInfo;
	OBJINFO			m_eObjeInfo;

	const _tchar*	m_szFoodName;
	wstring			m_wstFoodName;

	_float3	m_f3ClickPos = _float3(0.f, 0.f, 0.f);

	_uint	m_iRoyal_Tart_Count = 0;
	_uint	m_iBurrito_Count = 0;

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END