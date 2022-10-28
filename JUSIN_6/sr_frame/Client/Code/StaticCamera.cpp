#include "stdafx.h"
#include "..\Header\StaticCamera.h"
#include "Player.h"
#include "CamAnimation.h"

CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	D3DXMatrixIdentity(&m_matProj);
}


CStaticCamera::~CStaticCamera()
{
}

HRESULT CStaticCamera::Ready_Object()
{
	m_fDistance = 16.f;
	m_fSmoothSpeed = 0.125f;
	m_pTransform = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	SetMatProj();

	
	m_pTransform->Rotation(ROT_Y, D3DXToRadian(50.f));
	m_pTransform->Rotation(ROT_X, D3DXToRadian(55.f));

	m_fShakeTime = 0.f;
	m_fCurShakeTime = 0.f;


	return S_OK;
}

void CStaticCamera::Set_Mode()
{
	m_pTransform->Rotation(ROT_Y, D3DXToRadian(-50.f));
	m_pTransform->Rotation(ROT_X, D3DXToRadian(-10.f));

}


Engine::_int CStaticCamera::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (DIKeyDown(DIK_F5))
	{
		SetPersonalMode(true);
	}
	if (DIKeyDown(DIK_F6))
	{
		SetPersonalMode(false);
	}

	switch (m_eMode)
	{
	case CAM_NORMAL:
		Update_DefaultFollow(fTimeDelta);
		break; 
	case CAM_ANIMATION:
		m_pCamAnim->GetCamWorld(m_pTransform->m_matWorld);
		if (m_pCamAnim->IsFinish())
		{
			Safe_Release(m_pCamAnim);
			m_eMode = CAM_NORMAL;
			ResetPosition();
			// cam reset pos to normal
		}
		break;
	case CAM_LOGO_FOLLOW:
		if (m_pFollowPart)
			m_pTransform->m_matWorld = m_pFollowPart->GetWorldMat();
		break;
	case CAM_PERSONAL:
		Update_Personal(fTimeDelta);
		break;
	case CAM_WAIT:

	default: ;
	}

	// CTransform*	pSkyBoxTransform = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_Environment", L"SkyBox", L"Proto_TransformCom", ID_DYNAMIC));
	// NULL_CHECK_RETURN(pSkyBoxTransform, -1);

	// pSkyBoxTransform->m_vInfo[INFO_POS] = m_vEye;

	if (m_fShakeTime > m_fCurShakeTime)
	{
		m_fCurShakeTime += fTimeDelta;
		_float fX = CGameUtilMgr::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
		_float fY = CGameUtilMgr::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;

		_matrix matLocal;
		D3DXMatrixTranslation(&matLocal, fX, fY, 0.f);
		m_pTransform->m_matWorld = matLocal * m_pTransform->m_matWorld;
	}

	D3DXMatrixInverse(&m_matView, nullptr, &m_pTransform->m_matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return OBJ_NOEVENT;
}

void CStaticCamera::SetMatProj(const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, fFov, fAspect, fNear, fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}


void CStaticCamera::SetTarget(CGameObject* pTarget)
{
	Safe_Release(m_pTarget);
	Safe_Release(m_pTargetTrans);

	m_pTarget = pTarget;
	m_pTarget->AddRef();
	m_pTargetTrans = m_pTarget->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
	m_pTargetTrans->AddRef();

	m_pTransform->m_vInfo[INFO_POS] = m_pTargetTrans->m_vInfo[INFO_POS] + (m_pTransform->m_vInfo[INFO_LOOK] * -
		m_fDistance);
}

void CStaticCamera::LerpDistanceTo(_float fDistance)
{
}

void CStaticCamera::PlayeCamAnimation(const wstring& wstrAnim)
{
	m_pCamAnim = CCamAnimation::Create(m_pGraphicDev, wstrAnim);
	Engine::Get_Layer(LAYER_GAMEOBJ)->Add_GameObject(L"CamAnim", m_pCamAnim);
	m_pCamAnim->AddRef();
	m_eMode = CAM_ANIMATION;
}

void CStaticCamera::ResetPosition()
{
	if (m_pTargetTrans)
		m_pTransform->m_vInfo[INFO_POS] = m_pTargetTrans->m_vInfo[INFO_POS] + (m_pTransform->m_vInfo[INFO_LOOK] * -m_fDistance);
}

void CStaticCamera::PlayShake(_float fDuration, _float fMagnitude)
{
	m_fCurShakeTime = 0.f;
	m_fShakeTime = fDuration;
	m_fMagnitude = fMagnitude;
}

void CStaticCamera::Free(void)
{
	Safe_Release(m_pCamAnim);

	Safe_Release(m_pTarget);
	Safe_Release(m_pTargetTrans);
	CGameObject::Free();
}

CStaticCamera* CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStaticCamera* pInstance = new CStaticCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CStaticCamera::SetPersonalMode(bool bPersonal)
{
	if (bPersonal)
	{
		m_eMode = CAM_PERSONAL;
		_matrix matTargetWorld = m_pTargetTrans->m_matWorld;
		matTargetWorld._42 += 2.3f;
		m_pTransform->Set_WorldDecompose(matTargetWorld);
	}
	else
	{
		m_eMode = CAM_NORMAL;
	}
}

void CStaticCamera::Update_DefaultFollow(const _float& fTimeDelta)
{
	if (m_pTargetTrans == nullptr) return;

	_vec3 vDesiredPos = m_pTargetTrans->m_vInfo[INFO_POS] + (m_pTransform->m_vInfo[INFO_LOOK] * -m_fDistance);
	_vec3 vSmoothPos;
	D3DXVec3Lerp(&vSmoothPos, &m_pTransform->m_vInfo[INFO_POS], &vDesiredPos, m_fSmoothSpeed);

	m_pTransform->m_vInfo[INFO_POS] = vSmoothPos;
}

void CStaticCamera::Update_Personal(const _float& fTimeDelta)
{
	if (m_pTargetTrans == nullptr) return;
	m_pTransform->m_vInfo[INFO_POS] = m_pTargetTrans->m_vInfo[INFO_POS] + _vec3{0.f, 2.3f, 0.f};

	_long		dwMouseMove = 0;

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		m_pTransform->m_vAngle.y += D3DXToRadian(dwMouseMove / 10.f);
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		m_pTransform->m_vAngle.x += D3DXToRadian(dwMouseMove / 10.f);
	}
}
