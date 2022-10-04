#pragma once

#include "Engine_Include.h"
#include "Scene.h"

#include "Terrain.h"
#include "DynamicCamera.h"
#include "StaticCamera.h"
#include "TestPlayer.h"
#include "Player.h"
#include "SkyBox.h"
#include "Wand.h"

// Monster
#include "GreenSlime.h"
#include "PinkSlime.h"
#include "Fist.h"
#include "BlueBat.h"
#include  "BrownBat.h"
// Boss
#include "SongBoss.h"

// NPC
#include "Dog.h"
#include "Grandfather.h"
#include "GuitarMan.h"
#include "HoodMan.h"
#include "SmileMan.h"
#include "WinkMan.h"

// UI
#include "HealthBar.h"
#include "HPGauge.h"
#include "QuickSlot.h"
#include "Inventory.h"
#include "EquipWindow.h"
// Font
#include "HPGauge.h"
#include "TalkWindow.h"

class CStage : public CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Scene(void) override;
	virtual void	Render_Scene(void) override;

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);

	HRESULT			Ready_Proto(void);
	HRESULT			Ready_Light(void);

public:
	static CStage*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free(void);

private:
	CHPGauge*			m_pHpNumber = nullptr;
};

