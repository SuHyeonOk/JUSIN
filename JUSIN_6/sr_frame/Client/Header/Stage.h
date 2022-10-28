#pragma once

#include "Engine_Include.h"
#include "Scene.h"
#include "TerrainCubeMap.h"

class CArrowCubeMgr;
class CPlayerUI;
class CPlayer;
class CTerrainWater;

class CStage : public Engine::CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;
private:
	HRESULT			Ready_Layer_Environment();
	HRESULT			Ready_Layer_GameLogic();
	HRESULT			Ready_Layer_UI();

	_vec3			m_vDir;

public:
	static CStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
	void CreateTrigger();
	_bool m_bPlayerAlive = false;
	CPlayerUI* m_pPlayerUI = nullptr;
	CPlayer* m_pPlayer = nullptr;
	_float m_fDeadTime = 0.f;
};

