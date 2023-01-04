#include "stdafx.h"
#include "..\public\M_Gronmes.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"

#include "B_2DBullet.h"
#include "UI_3DTexture.h"

CM_Gronmes::CM_Gronmes(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Gronmes::CM_Gronmes(const CM_Gronmes & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Gronmes::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gronmes::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Gronmes";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_BLUE)
	{
		MonsterDesc.TransformDesc.fSpeedPerSec = 2.5f;
		MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
		m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_RED)
	{
		MonsterDesc.TransformDesc.fSpeedPerSec = 2.5f;
		MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
		m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_YELLOW)
	{
		MonsterDesc.TransformDesc.fSpeedPerSec = 2.5f;
		MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
		m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);
	}

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.MOVE;
	m_tMonsterInfo.iHp		= 40;
	m_tMonsterInfo.iExp		= 40;
	m_tMonsterInfo.iAttack	= 10;

	return S_OK;
}

void CM_Gronmes::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);
}

void CM_Gronmes::Late_Tick(_double TimeDelta)
{


	__super::Late_Tick(TimeDelta);
}

HRESULT CM_Gronmes::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 0)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CM_Gronmes::On_Collision(CGameObject * pOther)
{
	__super::On_Collision(pOther);
}

HRESULT CM_Gronmes::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_BLUE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gnomes_BLUE"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_RED)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gonmes_RED"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_YELLOW)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gnomes_YELLOW"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.7f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gronmes::SetUp_ShaderResources()
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

	return S_OK;
}

void CM_Gronmes::Monster_Tick(const _double& TimeDelta)
{
	if (0 >= m_tMonsterInfo.iHp)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(6, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(7, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(2, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		m_pModelCom->Set_AnimIndex(5, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick();
		m_pModelCom->Set_AnimIndex(3, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(2);
		break;
	}
}

void CM_Gronmes::Idle_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	if (m_bAttack && 50 <= m_pModelCom->Get_Keyframes())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Gronmes::Move_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	_double dMoveTime;
	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_BLUE)
		dMoveTime = 2;
	else  if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_RED)
		dMoveTime = 2.2;
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.G_YELLOW)
		dMoveTime = 2.4;

	if (!CM_Monster::Random_Move(m_pTransformCom, m_f4CenterPos, TimeDelta, dMoveTime))
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
		m_bAttack = false;
	}
}

void CM_Gronmes::Find_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_FIND;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z - 0.5f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Find_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Gronmes::Attack_Tick(const _double& TimeDelta)
{
	_int	iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(0, 1);
	if (0 != iRandomNum && m_pModelCom->Get_Finished())	// 랜덤으로 0이 들어오면 바로 MOVE로 가고, 1일 때는 ATTACK 이다.
		return;

	m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	m_bAttack = true;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 내 좌표
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	// 플레이어 좌표
	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
	_float4	f4PlayerPos;
	XMStoreFloat4(&f4PlayerPos, vPlayerPos);

	CB_2DBullet::BULLETINFO		tBulletInfo;
	tBulletInfo.iMonsterAttack = m_tMonsterInfo.iAttack;
	tBulletInfo.eToodyBullet = tBulletInfo.CIRCLE_BULLET;
	tBulletInfo.f3Start_Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
	tBulletInfo.f3Target_Pos = _float3(f4PlayerPos.x, f4PlayerPos.y + 0.7f, f4PlayerPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_B_Circle_0"), TEXT("Prototype_GameObject_B_ToodyBullet"), &tBulletInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Gronmes::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Gronmes::Die_Tick()
{
	// 몬스터가 죽고 나면 할 행동

	CGameObject::Set_Dead();
	CObj_Manager::GetInstance()->Set_Player_Exp(m_tMonsterInfo.iExp);	// 플레이어에게 경험치 증가

	if (!m_OneCoin)	// 동전 생성
	{
		// Item
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), 5, 2, 1);

		m_OneCoin = true;
	}
}

CM_Gronmes * CM_Gronmes::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Gronmes*		pInstance = new CM_Gronmes(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Gronmes");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Gronmes::Clone(void * pArg)
{
	CM_Gronmes*		pInstance = new CM_Gronmes(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Gronmes");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Gronmes::Free()
{
	__super::Free();


}