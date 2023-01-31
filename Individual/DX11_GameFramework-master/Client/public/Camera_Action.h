#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

BEGIN(Client)

class CCamera_Action final : public CCamera
{
public:
	typedef struct tagCameraInfo
	{
		LEVEL		eLevel = LEVEL_END;
		_float3		f3Pos = _float3(0.f, 0.f, 0.f);
	}CAMERAINFO;

private:
	CCamera_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Action(const CCamera_Action& rhs);
	virtual ~CCamera_Action() = default;

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

	_float		m_fSpeed = 0.f;

	_float4		m_vMinCamPos;

	_float		m_fYZ_Move = -12.0f;

public:
	static CCamera_Action*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END