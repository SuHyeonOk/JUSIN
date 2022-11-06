#pragma once
#include "Scene.h"
class CAnimationTool : public CScene
{
private:
	explicit CAnimationTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAnimationTool() override;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;

	virtual void Free() override;
	static CAnimationTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	class CDynamicCamera* m_pCam = nullptr;
	class CSkeletalCube* m_Skel = nullptr;
	struct SkeletalPart* m_part;
};

