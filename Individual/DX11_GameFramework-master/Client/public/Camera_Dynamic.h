#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	typedef struct tagCameraInfo
	{
		LEVEL		eLevel = LEVEL_END;
		_float3		f3Pos = _float3(0.f, 0.f, 0.f);
	}CAMERAINFO;

private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	void Shake_Camera(_double fTime, _int iPower)
	{
		m_bShake = true;
		m_dShakeTime = fTime;
		m_iShakePower = iPower;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();

private:
	void	Key_Input(_double TimeDelta);
	void	ToFollow(_double TimeDelta);
	void	Shake_Camera(_double TimeDelta);

private:
	CCamera::CAMERADESC			m_CameraDesc;
	CAMERAINFO					m_eCameraInfo;

	// Shake_Camera 와 관련된 변수
	_bool		m_bShake = false;
	_double		m_dShakeTime = 0;
	_double		m_dShakeTimeNow = 0;
	_int		m_iShakePower = 0;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END