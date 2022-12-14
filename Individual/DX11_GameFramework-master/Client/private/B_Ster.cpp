#include "stdafx.h"
#include "..\public\B_Star.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CB_Star::CB_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CB_Star::CB_Star(const CB_Star & rhs)
	: CGameObject(rhs)
{

}

HRESULT CB_Star::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_Star::Initialize(void * pArg)
{	
	m_wsTag = L"2DBullet";

	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(BULLETINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Start_Pos.x, m_tBulletInfo.f3Start_Pos.y, m_tBulletInfo.f3Start_Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 1.f));

	return S_OK;
}

void CB_Star::Tick(_double TimeDelta)
{
	_matrix PlayerWorld;
	PlayerWorld = m_pTransformCom->Get_WorldMatrix();
	_float4x4 f44PlayerWorld;
	XMStoreFloat4x4(&f44PlayerWorld, PlayerWorld);
		cout << "World_Right	: " << f44PlayerWorld._11 << " | " << f44PlayerWorld._12 << " | " << f44PlayerWorld._13 << " | " << f44PlayerWorld._14 << endl;
		cout << "World_Up		: " << f44PlayerWorld._21 << " | " << f44PlayerWorld._22 << " | " << f44PlayerWorld._23 << " | " << f44PlayerWorld._24 << endl;
		cout << "World_Look		: " << f44PlayerWorld._31 << " | " << f44PlayerWorld._32 << " | " << f44PlayerWorld._33 << " | " << f44PlayerWorld._34 << endl;
		cout << "World_Pos		: " << f44PlayerWorld._41 << " | " << f44PlayerWorld._42 << " | " << f44PlayerWorld._43 << " | " << f44PlayerWorld._44 << endl;
		cout << "----------------------------------------" << endl;

	__super::Tick(TimeDelta);

	// 플레이어의 몇 틱 전의 좌표를 받아와서 총알을 날리고, 일정시간 후에 총알이 사라지도록 해야한다.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	_vector vPlayerPos = XMVectorSet(m_tBulletInfo.f3Target_Pos.x, m_tBulletInfo.f3Target_Pos.y, m_tBulletInfo.f3Target_Pos.z, 1.f);
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vDistance = vPlayerPos - vMyPos;
	vMyPos += XMVector3Normalize(vDistance) * 4.f * _float(TimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vCameraPos);		// 카메라를 바라본다.
 	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);


	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CB_Star::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 총알이 생성된 뒤 3초가 지나면 삭제한다.
	m_dBullet_TimeAcc += TimeDelta;
	if (1 < m_dBullet_TimeAcc)
	{
		CGameObject::Set_Dead();
		m_dBullet_TimeAcc = 0;
	}

	//if (CObj_Manager::GetInstance()->Get_Player_Collider(&m_pColliderCom))
	//	CGameObject::Set_Dead();

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CB_Star::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);	// pass

	m_pVIBufferCom->Render();

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

void CB_Star::On_Collision(CGameObject * pOther)
{
	CGameObject::Set_Dead();
}

HRESULT CB_Star::SetUp_Components()
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

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Star"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.03f, 0.03f, 0.03f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_Star::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CB_Star * CB_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_Star*		pInstance = new CB_Star(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_Star::Clone(void * pArg)
{
	CB_Star*		pInstance = new CB_Star(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_Star::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
