#pragma once

#include "GameObject.h"

enum CamMode
{
	CAM_NORMAL,
	CAM_ANIMATION,
	CAM_LOGO_FOLLOW,
	CAM_WAIT,
	CAM_PERSONAL,
	CAM_END
};
class CCamAnimation;
class CStaticCamera : public CGameObject
{
private:
	explicit CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticCamera();

public:
	HRESULT Ready_Object() override;

	virtual _int Update_Object(const _float& fTimeDelta) override;
	void SetMatProj(const _float& fFov = D3DXToRadian(60.f),
	                const _float& fAspect = (float)WINCX / WINCY,
	                const _float& fNear = 0.1f,
	                const _float& fFar = 1000.f);

	void SetTarget(CGameObject* pTarget);
	void LerpDistanceTo(_float fDistance);
	void PlayShake(_float fDuration, _float fMagnitude);

	void PlayeCamAnimation(const wstring& wstrAnim);
	void Set_Mode(void);
	void ResetPosition();
	void SetSkeletalFollowMode(SkeletalPart* pPart) { m_pFollowPart = pPart; m_eMode = CAM_LOGO_FOLLOW;}

	void SetPersonalMode(bool bPersonal);
private:
	void Update_DefaultFollow(const _float& fTimeDelta);
	void Update_Personal(const _float& fTimeDelta);

private:
	CamMode m_eMode = CAM_NORMAL;

	_matrix m_matView, m_matProj;
	Engine::CTransform* m_pTransform = nullptr;

	CGameObject* m_pTarget = nullptr;
	Engine::CTransform* m_pTargetTrans = nullptr;

	// shake
	_float m_fShakeTime;
	_float m_fCurShakeTime;
	_float m_fMagnitude;
	// shake

	// normal mode
	_float m_fDistance;
	_float m_fSmoothSpeed;
	// normal mode

	// anim mode
	CCamAnimation* m_pCamAnim = nullptr;
	// anim mode

	// logo mode
	SkeletalPart* m_pFollowPart = nullptr;
	//

public:
	static CStaticCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
};


