#include "stdafx.h"
#include "..\public\E_FlyingEnvironment.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"

CE_FlyingEnvironment::CE_FlyingEnvironment(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_FlyingEnvironment::CE_FlyingEnvironment(const CE_FlyingEnvironment & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_FlyingEnvironment::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_FlyingEnvironment::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tEffectInfo, pArg, sizeof(m_tEffectInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tEffectInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (BUTTERFLIES_BLUE == m_tEffectInfo.eType || BUTTERFLIES_RED == m_tEffectInfo.eType || BUTTERFLIES_YELLOW == m_tEffectInfo.eType)
	{
		m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 1.f));
		m_bNeat_TextureTime = 0.07f;
		m_bFindDistance = 3.0f;
		Ready_Butterflies();
	}
	else if (BUTTERFLIES_LUMINOUS == m_tEffectInfo.eType)
	{
		m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 1.f));
		m_bNeat_TextureTime = 0.09f;
		m_bFindDistance = 3.0f;
		Ready_Butterflies();
	}
	else if (CANFIRE_BIG == m_tEffectInfo.eType)
	{
		m_bFindDistance = 7.0f;
		m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 1.f));
	}
	else if (CANFIRE_MEDIUM == m_tEffectInfo.eType)
	{
		m_bFindDistance = 7.0f;
		m_pTransformCom->Set_Scaled(_float3(0.4f, 0.4f, 1.f));
	}
	else if (CANFIRE_SMALL == m_tEffectInfo.eType)
	{
		m_bFindDistance = 7.0f;
		m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 1.f));
	}
	else if (FIRESPARKS == m_tEffectInfo.eType)
	{
		m_bFindDistance = 5.0f;
		m_pTransformCom->Set_Scaled(_float3(0.6f, 0.6f, 1.f));
	}
	else if (FIRESPARKS_SMALL == m_tEffectInfo.eType)
	{
		m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 1.f));
		m_bFindDistance = 5.0f;
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CE_FlyingEnvironment::Tick(_double TimeDelta)
{
	if (BUTTERFLIES_BLUE == m_tEffectInfo.eType || BUTTERFLIES_RED == m_tEffectInfo.eType || BUTTERFLIES_YELLOW == m_tEffectInfo.eType ||
		FIRESPARKS == m_tEffectInfo.eType || FIRESPARKS_SMALL == m_tEffectInfo.eType || BUTTERFLIES_LUMINOUS == m_tEffectInfo.eType)
	{
		// 플레이어가 가까이 있을 때만 실행된다.
		if (false == m_bFindPlayer)
		{
			// 그냥 3보다 크다면 return 하면 되잖아! Late_Tick() 에서도 체크해애야 하기 때문에 여기서 한 번만 확인하는 것이 저렴하다고 생각하기 때문에
			_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			if (m_bFindDistance < fDistance)
				return;
			else
				m_bFindPlayer = true;
		}
	}

	__super::Tick(TimeDelta);

	// ★ 객체마다 다르게 주돌아야 하는 Tick
	if (BUTTERFLIES_BLUE == m_tEffectInfo.eType || BUTTERFLIES_RED == m_tEffectInfo.eType || BUTTERFLIES_YELLOW == m_tEffectInfo.eType || BUTTERFLIES_LUMINOUS == m_tEffectInfo.eType)
		Butterflies_Tick(TimeDelta);
	else if (CANFIRE_BIG == m_tEffectInfo.eType || CANFIRE_MEDIUM == m_tEffectInfo.eType || CANFIRE_SMALL == m_tEffectInfo.eType)
		CanFire_Tick(TimeDelta);
	else if (FIRESPARKS == m_tEffectInfo.eType)
		FireSparks_Tick(TimeDelta);
	else if (FIRESPARKS_SMALL == m_tEffectInfo.eType)
		FireSparksSmall_Tick(TimeDelta);

	// 추가적으로 하나의 Tick() 을 돌아야 하는 함수
	if (BUTTERFLIES_LUMINOUS == m_tEffectInfo.eType)
	{
		ButterfliesLuminous_Light();
	}
}

void CE_FlyingEnvironment::Late_Tick(_double TimeDelta)
{
	if (BUTTERFLIES_BLUE == m_tEffectInfo.eType || BUTTERFLIES_RED == m_tEffectInfo.eType || BUTTERFLIES_YELLOW == m_tEffectInfo.eType ||
		FIRESPARKS == m_tEffectInfo.eType || FIRESPARKS_SMALL == m_tEffectInfo.eType || BUTTERFLIES_LUMINOUS == m_tEffectInfo.eType)
	{
		if (false == m_bFindPlayer)
			return;
	}
	
	__super::Late_Tick(TimeDelta);

	CGameObject::Compute_CamZ();

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CE_FlyingEnvironment::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_FlyingEnvironment::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	_tchar	m_szTextureName[MAX_PATH] = L"";

	if (BUTTERFLIES_BLUE == m_tEffectInfo.eType || BUTTERFLIES_RED == m_tEffectInfo.eType || BUTTERFLIES_YELLOW == m_tEffectInfo.eType)
	{
		if (150 > m_fRandomAxis || 320 < m_fRandomAxis)
		{
			// 오른쪽으로 날아가는 나비
			if (BUTTERFLIES_BLUE == m_tEffectInfo.eType)
				wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Bule"));
			else if (BUTTERFLIES_RED == m_tEffectInfo.eType)
				wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Red"));
			else if (BUTTERFLIES_YELLOW == m_tEffectInfo.eType)
				wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Yellow"));
		}
		else
		{
			// 왼쪽으로 날아가는 나비
			if (BUTTERFLIES_BLUE == m_tEffectInfo.eType)
				wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_BuleL"));
			else if (BUTTERFLIES_RED == m_tEffectInfo.eType)
				wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_RedL"));
			else if (BUTTERFLIES_YELLOW == m_tEffectInfo.eType)
				wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_YellowL"));
		}
	}
	else if (CANFIRE_BIG == m_tEffectInfo.eType || CANFIRE_MEDIUM == m_tEffectInfo.eType || CANFIRE_SMALL == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_CutScene_Fire"));
	else if (FIRESPARKS == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_FireSparks"));
	else if (FIRESPARKS_SMALL == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_FireSparks_Small"));
	else if (BUTTERFLIES_LUMINOUS == m_tEffectInfo.eType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_Butterflies_Luminous"));
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_FlyingEnvironment::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	// 텍스처 넘기기
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexture_Index)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

void CE_FlyingEnvironment::Ready_Butterflies()
{
	m_fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 360.0f);
	_matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(m_fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);
	XMStoreFloat4(&m_f4RandomLook, vLook);

	m_pTransformCom->Set_Pos(CUtilities_Manager::GetInstance()->Get_Random(1.0f, 2.0f));
}

void CE_FlyingEnvironment::Butterflies_Tick(const _double & TimeDelta)
{
	// 이미지를 원하는 만큼 돌리고, 랜덤한 방향으로 날아가며, 알파값이 줄어들면서 사라진다.

	// ★ 카메라를 바라보고 랜덤한 곳으로 회전하면서 이동하기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);
	//m_pTransformCom->Set_Pos(1.0f);

	// 입력한 Look 방향으로 이동하기
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = XMLoadFloat4(&m_f4RandomLook);
	vMyPos += XMVector3Normalize(vDistance) * (_float(TimeDelta) * 0.3f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vMyPos, 1.0f));

	m_dChange_TimeAcc += TimeDelta;
	if (m_bNeat_TextureTime < m_dChange_TimeAcc)
	{
		++m_iTexture_Index;
		m_dChange_TimeAcc = 0;
	}
	if (4 <= m_iTexture_Index)
	{
		m_iTexture_Index = 0;
		++m_iTextureDead_Count;
	}

	if (10 < m_iTextureDead_Count)
		m_fAlpha -= _float(TimeDelta) * 0.5f;
	
	if (0.0f > m_fAlpha)
	{
		//m_fAlpha = 1.0f;
		//m_iTexture_Index = 0;
		CGameObject::Set_Dead();
	}
}

void CE_FlyingEnvironment::CanFire_Tick(const _double & TimeDelta)
{
	m_dChange_TimeAcc += TimeDelta;
	if (0.07 < m_dChange_TimeAcc)
	{
		++m_iTexture_Index;
		m_dChange_TimeAcc = 0;
	}
	if (4 <= m_iTexture_Index)
	{
		m_iTexture_Index = 0;
	}
}

void CE_FlyingEnvironment::FireSparks_Tick(const _double & TimeDelta)
{
	// 올라간다.
	m_pTransformCom->Go_Up(TimeDelta * 0.3f);

	// 이미지 변한다.
	m_dChange_TimeAcc += TimeDelta;
	if (0.1 < m_dChange_TimeAcc)
	{
		++m_iTexture_Index;
		m_dChange_TimeAcc = 0;
	}
	if (4 <= m_iTexture_Index)
	{
		m_iTexture_Index = 0;
	}

	// 알파값 줄어든다.
	m_fAlpha -= _float(TimeDelta) * 0.15f;

	if (0.0f > m_fAlpha)
	{
		m_fAlpha = 1.0f;
		m_pTransformCom->Set_Pos(0.5f);

		m_pTransformCom->Set_Scaled(_float3(CUtilities_Manager::GetInstance()->Get_Random(0.5f, 0.6f), CUtilities_Manager::GetInstance()->Get_Random(0.5f, 0.6f), 1.f));
	}
}

void CE_FlyingEnvironment::FireSparksSmall_Tick(const _double & TimeDelta)
{
	// 올라간다.
	m_pTransformCom->Go_Up(TimeDelta * 0.3f);

	// 알파값 줄어든다.
	m_fAlpha -= _float(TimeDelta) * m_fAlphaSpeed;

	if (0.0f > m_fAlpha)
	{
		m_fAlpha = 1.0f;
		m_fAlphaSpeed = CUtilities_Manager::GetInstance()->Get_Random(0.15f, 0.2f);
		m_pTransformCom->Set_Pos(CUtilities_Manager::GetInstance()->Get_Random(0.1f, 0.5f));

		_float fRandomSize = CUtilities_Manager::GetInstance()->Get_Random(0.1f, 0.3f);
		m_pTransformCom->Set_Scaled(_float3(fRandomSize, fRandomSize, 1.f));
	}
}

void CE_FlyingEnvironment::ButterfliesLuminous_Light()
{

}

CE_FlyingEnvironment * CE_FlyingEnvironment::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_FlyingEnvironment*		pInstance = new CE_FlyingEnvironment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_FlyingEnvironment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_FlyingEnvironment::Clone(void * pArg)
{
	CE_FlyingEnvironment*		pInstance = new CE_FlyingEnvironment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_FlyingEnvironment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_FlyingEnvironment::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
