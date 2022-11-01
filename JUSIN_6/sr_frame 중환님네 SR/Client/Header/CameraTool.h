#pragma once
#include "CamAnimation.h"
#include "Scene.h"

enum CamType
{
	CAM_WORKING,
	CAM_PLAYANIM
};

class CCameraTool :	public CScene
{
private:
	explicit CCameraTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraTool() override;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;

	virtual void Free() override;
	static CCameraTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	class CDynamicCamera* m_pCam = nullptr;
	class CSkeletalCube* m_Skel = nullptr;
	struct SkeletalPart* m_part;

	_bool m_bCamAnimPlay;

	CCamAnimation* m_pCamAnim = nullptr;

	CamType	m_eType;
};

