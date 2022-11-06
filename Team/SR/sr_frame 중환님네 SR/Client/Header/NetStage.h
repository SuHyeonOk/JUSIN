#pragma once
#include "Scene.h"

class CArrowCubeMgr;
class CTerrainWater;
class CNetStage : public CScene
{
private:
	explicit CNetStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNetStage();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;
private:


public:
	static CNetStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
};

