#pragma once

#include "Engine_Include.h"
#include "Scene.h"
#include "TerrainCubeMap.h"

class CArrowCubeMgr;
class CPlayerUI;
class CPlayer;
class CTerrainWater;

class CStage_Kouku : public Engine::CScene
{
private:
	explicit CStage_Kouku(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage_Kouku();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;

	void CounterLightColor(_float _color)
	{

		D3DLIGHT9		tLightInfo;
		ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));
		tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
		tLightInfo.Diffuse = D3DXCOLOR(_color, _color, _color, 1.f);
		tLightInfo.Specular = D3DXCOLOR(_color, _color, _color, 1.f);
		tLightInfo.Ambient = D3DXCOLOR(_color, _color, _color, 1.f);
		tLightInfo.Direction = _vec3(0.3f, -1.f, 0.15f);
		m_pGraphicDev->SetLight(0, &tLightInfo);
		m_pGraphicDev->LightEnable(0, TRUE);
	}

	Atomic<_bool> m_bLoadingDone{false};

private:
	HRESULT			Ready_Layer_Environment();
	HRESULT			Ready_Layer_GameLogic();
	HRESULT			Ready_Layer_UI();

	_vec3			m_vDir;


public:
	static CStage_Kouku*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
	_bool m_bPlayerAlive = false;
	CPlayerUI* m_pPlayerUI = nullptr;
	CPlayer* m_pPlayer = nullptr;
	_float m_fDeadTime = 0.f;

	_float m_fLightColor = 0.8f;
};

