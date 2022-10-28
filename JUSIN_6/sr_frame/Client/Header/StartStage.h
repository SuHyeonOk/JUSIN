#pragma once
#include "Scene.h"
#include "TerrainCubeMap.h"

class CPlayer;
class CMapUI;

class CStartStage : public CScene
{
private:
	explicit CStartStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStartStage();

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
	static CStartStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
	CPlayer* m_pPlayer = nullptr;
	CMapUI* m_pMapUI = nullptr;

	_bool m_bSpawn = false;
	_float m_SpawnCounter = 2.f;
};

