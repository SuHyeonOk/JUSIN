#include "stdafx.h"
#include "..\public\M_Gary_Boss.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"

#include "B_3DBullet.h"
#include "Boss_S_Cage.h"
#include "Boss_Fan.h"
#include "Boss_S_Wind.h"

CM_Gary_Boss::CM_Gary_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CM_Gary_Boss::CM_Gary_Boss(const CM_Gary_Boss & rhs)
	: CGameObject(rhs)
{

}

HRESULT CM_Gary_Boss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gary_Boss::Initialize(void * pArg)
{
	m_wsTag = L"Not_UI";

	_float3   f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC      GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(6);
	m_f4CenterPos = _float4(2.8f, 0.0f, 12.6f, 1.0f);
	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(180.f));

	m_eState = IDLE;
	m_eAnimState = IDLE;
	m_fHP = CObj_Manager::GetInstance()->Get_Current_Player().fAttack * 1000.0f;
	m_fMaxHP = m_fHP;
	m_fAttack = 50.0f;
	m_fExp = 1000.0f;

	return S_OK;
}

void CM_Gary_Boss::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (false == m_bCutScene)							// 외부에서 컷씬이 끝났음을 알려준다.
	{
		CutScene_Effect();
		return;
	}
	else
		CutScene_Tick(TimeDelta);						// 컷 씬이 한 번 실행 되고 난 후에 더 이상 실행 되지 않을 함수 이다.

	if (true == m_bCutSceneEnd)							// 객체 내 에서 컷씬이 끝나면 한 번만 진행할 수 있도록 하고, 지속적으로 실행 되어야할 함수를 실행 시킨다.
	{
		Shader_Alpha(TimeDelta);
		Effect_Tick(TimeDelta);

		Monster_Tick(TimeDelta);
		Anim_Change();
	}
}

void CM_Gary_Boss::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CM_Gary_Boss::Render()
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
		{
 			if (true == m_bShader_Hit)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
			{
				if (1 != m_fAlpha)
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
				else
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
			}
		}
	}

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CM_Gary_Boss::On_Collision(CGameObject * pOther)
{
	//if (L"Object_Collider" == pOther->Get_Tag())	// 중간에 해골을 만나게 되면 Look 을 변경하라
	//	m_bLookChange = true;

	if (CObj_Manager::PLAYERINFO::STATE::ATTACK == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		if (L"Player_Weapon" == pOther->Get_Tag())
			m_bHit = true;

	if (L"Skill_Paint" == pOther->Get_Tag())
		m_bHit = true;
}

HRESULT CM_Gary_Boss::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_Gary"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC         ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.8f, 1.5f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gary_Boss::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (1 != m_fAlpha)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
			return E_FAIL;
	}

	return S_OK;
}

void CM_Gary_Boss::Monster_Tick(const _double & TimeDelta)
{
	// 체력이 0 이되면 죽는다.
	if (0 >= m_fHP)
		m_eState = DIE;

	Hit_Tick(TimeDelta);   // Hit 상태는 따로 다른 Tick 은 계속 돌아가기를 원한다.

	switch (m_eState)
	{
	case Client::CM_Gary_Boss::IDLE:
		// 거리 7 이내에 플레이어가 있는지 확인하고, 있으면 공격 시작
		Idle_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_MOVE:
		// 플레이어를 향해 빠르게 달려오고, 평태 친다.
		A_Move_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_BULLET:
		// 플레이어를 향한 불꽃 총알을 발사한다.
		A_Bullet_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_STUN:
		// 광역으로 플레이어를 stun 시킨다.
		A_Stun_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_CAGE:
		// 플레이어를 순간 이동 시켜서 케이지 안 에 가두고, 잡몹을 소환한다. (몬스터는 시야에 보이지 않는다.)
		A_Cage_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_DANCE:
		// 밴드 쪽 으로 올라가고, 아래 잡몹들이 춤을 추면 보스는 체력을 회복한다. 
		A_Dance_Tick(TimeDelta);
		break;
		//case Client::CM_Gary_Boss::HIT:
		//   Hit_Tick(TimeDelta);
		//   break;
	case Client::CM_Gary_Boss::DIE:
		Die_Tick(TimeDelta);
		break;
	}
}

void CM_Gary_Boss::Anim_Change()
{
	// 0 : 오른손 평타
	// 1 : 왼손 평타
	// 2 : 뒤로 넘어갈랑 DIE??
	// 3 : 왼쪽 Hit
	// 4 : 손 펼락 말랑
	// 5 : IDLE
	// 6 : 춤
	// 7 : MOVE

	if (true == m_bHit && CM_Gary_Boss::IDLE == m_eState)
		m_eAnimState = CM_Gary_Boss::HIT;

	switch (m_eAnimState)
	{
	case Client::CM_Gary_Boss::IDLE:
		m_pModelCom->Set_AnimIndex(5);
		break;
	case Client::CM_Gary_Boss::MOVE:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case Client::CM_Gary_Boss::A_ATTACK:
		m_pModelCom->Set_AnimIndex(1, false);
		break;
	case Client::CM_Gary_Boss::A_BULLET:
		m_pModelCom->Set_AnimIndex(0, false);
		break;
	case Client::CM_Gary_Boss::A_STUN:
		m_pModelCom->Set_AnimIndex(4, false);
		break;
	case Client::CM_Gary_Boss::A_DANCE:
		m_pModelCom->Set_AnimIndex(6);
		break;
	case Client::CM_Gary_Boss::HIT:
		m_pModelCom->Set_AnimIndex(3, false);
		break;
	case Client::CM_Gary_Boss::DIE:
		m_pModelCom->Set_AnimIndex(2, false);
		break;
	}
}

void CM_Gary_Boss::Idle_Tick(const _double & TimeDelta)
{
	// 무조건 적으로 이전에 MovePos 가 true 라면 처음 위치로 이동 시켜야 한다.
	if (true == m_bMovePos)
	{
		if (0 == m_dSkill_TimeAcc)
			m_iEffect_Count = 0;
		m_pTransformCom->LookAt(XMVectorSet(4.0f, 2.0f, 16.0f, 1.0f));
		if (1 < m_dSkill_TimeAcc)   // 너무 바로 이동해서 1초 있다가 이동
		{
			m_bShader_Alpha = false;

			m_pTransformCom->Set_Pos(_float3(4.0f, 0.2f, 17.0f));
			m_bMovePos = false;
			m_iEffect_Count = 0;
		}
	}

	m_eAnimState = IDLE;

	_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (7.0f < fDistance)      // 일정 범위 밖 이라면 그냥 이동하고 있는다.
		RandomMove(TimeDelta);
	
	Random_Skill(TimeDelta);
}

void CM_Gary_Boss::Random_Skill(const _double& TimeDelta)
{
	m_dSkill_TimeAcc += TimeDelta;   // 시간초를 딱 센다.

	static _int iRandom;
	static _bool bRandomSuccess;
	static _bool bPotalEffect;

	if (3 > m_dSkill_TimeAcc)      // 3초 이전에는 랜덤하게 스킬을 뽑는데
	{
		if (false == bRandomSuccess)
		{
			_int   iMinRandomNumber = 0;
			_int   iMaxRandomNumber = 0;

			// DANCE 스킬의 경우 보스의 체력이 30% 남았을 때 실행된다.
			_float fHP = m_fHP / m_fMaxHP;
			if (0.3f > fHP)
			{
				iMinRandomNumber = 2;
				iMaxRandomNumber = 4;
			}
			else if (0.8f > fHP)
			{
				iMinRandomNumber = 1;
				iMaxRandomNumber = 3;
			}
			else
			{
				iMinRandomNumber = 0;
				iMaxRandomNumber = 2;
			}

			iRandom = CUtilities_Manager::GetInstance()->Get_Random(iMinRandomNumber, iMaxRandomNumber);
		}

		// 이전이랑 다른 패턴을 뽑았다면, 더이상 스킬을 뽑지 않고, 그렇지 않은 경우에는 그냥 그 스킬을 실행시킨다.
		if (m_iSkill_Data != iRandom)
			bRandomSuccess = true;
		else
			bRandomSuccess = false;
	}
	else
	{
		if (3 == iRandom || 4 == iRandom)
		{
			if (false == bPotalEffect)
			{
				bPotalEffect = true;
				m_iEffect_Count = 0;
			}

			m_bShader_Alpha = true;
		}
	}


	if (5 < m_dSkill_TimeAcc)
	{
		bPotalEffect = false;
		bRandomSuccess = false;
		iRandom = 2;   // Test
		if (0 == iRandom)
			m_eState = A_MOVE;
		else if (1 == iRandom)
			m_eState = A_BULLET;
		else if (2 == iRandom)
			m_eState = A_STUN;
		else if (3 == iRandom)
			m_eState = A_CAGE;
		else if (4 == iRandom)
			m_eState = A_DANCE;

		m_iSkill_Data = iRandom;

		m_dSkill_TimeAcc = 0;
	}
}

void CM_Gary_Boss::RandomMove(const _double & TimeDelta)
{
	if (false == m_bMove)
	{
		m_eAnimState = STATE::MOVE;

		// 랜덤한 좌표를 한 번 구한다.
		_float   fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
		_float   fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);

		_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, 0.0f, fRandomZ, 1.0f));

		_float fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(-5.0f, 5.0f);

		if (-1.0 > fRandomRange || 1.0 < fRandomRange)
		{
			_vector vRandomPos = vTempPos * fRandomRange;
			_float4 f4RandomPos;
			XMStoreFloat4(&f4RandomPos, vRandomPos);
			m_f4MovemPos = _float4((m_f4CenterPos.x + f4RandomPos.x), m_f4CenterPos.y, (m_f4CenterPos.z + f4RandomPos.z), m_f4CenterPos.w);

			m_bMove = true;
		}
		else
			return;
	}

	// 랜덤한 좌표로 이동한다.
	_vector vMovePos = XMVectorSet(m_f4MovemPos.x, m_f4MovemPos.y, m_f4MovemPos.z, m_f4MovemPos.w);
	m_pTransformCom->Chase(vMovePos, TimeDelta, 0.0f);
	m_pTransformCom->LookAt(vMovePos);

	_vector vDistance = vMovePos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float   fDistace = XMVectorGetX(XMVector3Length(vDistance));

	// 랜덤한 위치와 현재 내 좌표가 0.5f 이하라면! 아이들 상태로 있다가 다시 이동한다.
	if (0.5f > fDistace)
	{
		m_eAnimState = STATE::IDLE;
		m_bMove = false;				// 다시 랜덤한 좌표를 구한다.
	}
}

HRESULT CM_Gary_Boss::A_Move_Tick(const _double & TimeDelta)
{
	// 플레이어 근처까지 달려와서 바람을 날린다.
	m_eAnimState = STATE::MOVE;

	if (false == m_bSkill)
	{
		m_bSkill = true;
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		XMStoreFloat4(&m_f4PlayerPos, vPlayerPos);
	}

	_vector vPlayerPos = XMVectorSet(m_f4PlayerPos.x, 0.0f, m_f4PlayerPos.z, m_f4PlayerPos.w);
	m_pTransformCom->Chase(vPlayerPos, TimeDelta * 1.7, 1.9f);   // 거리가 멀 때만 플레이어를 따라간다.
	m_pTransformCom->LookAt(vPlayerPos);

	_vector vDistance = vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistace = XMVectorGetX(XMVector3Length(vDistance));

	if (2.0f > fDistace)
	{
		if (1 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
			m_bAttack_Animation = true;

		if (false == m_bAttack_Animation)
			m_eAnimState = STATE::A_ATTACK;
		else
		{
			// 내 좌표
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4(&f4MyPos, vMyPos);

			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			_float fRange = 1.5f;
			_vector vObjPos = vLook * fRange;
			_float4 f4ObjPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4(&f4ObjPos, vObjPos);

			vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float4 f4Look = { 0.0f, 0.0f, 1.0f, 0.0f };
			XMStoreFloat4(&f4Look, vLook);

			m_dSkill_TimeAcc += TimeDelta;
			if (0.3 < m_dSkill_TimeAcc)
			{
				CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

				CBoss_S_Wind::BOSSSKILLINFO tSkillInfo;
				tSkillInfo.f3Pos = _float3((f4MyPos.x + f4ObjPos.x), (f4MyPos.y + f4ObjPos.y), (f4MyPos.z + f4ObjPos.z));
				tSkillInfo.f4Look = _float4(f4Look.x, f4Look.y, f4Look.z, f4Look.w);
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Wind"), TEXT("Prototype_GameObject_Boss_S_Wind"), &tSkillInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				RELEASE_INSTANCE(CGameInstance);

				++m_iBullet_Count;
				m_dSkill_TimeAcc = 0;
			}

			if (3 <= m_iBullet_Count)
			{
				m_bAttack_Animation = false;
				m_bSkill = false;
				m_eState = STATE::IDLE;
				m_iBullet_Count = 0;
			}
		}
	}

	return S_OK;
}

HRESULT CM_Gary_Boss::A_Bullet_Tick(const _double & TimeDelta)
{
	if (3 <= m_iBullet_Count)
	{
		m_eState = IDLE;
		m_eAnimState = IDLE;
		m_iBullet_Count = 0;
		return S_OK;
	}

	m_eAnimState = A_BULLET;
	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	// 플레이어어의 좌표를 3번 받아서 따 따 따 총알을 발사한다.
	if (m_pModelCom->Get_Finished())
	{
		// 내 좌표
		_vector   vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4   f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 플레이어 좌표
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4   f4PlayerPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		CB_3DBullet::NONANIMBULLETINFO   tBulletInfo;
		tBulletInfo.fMonsterAttack = m_fAttack;
		tBulletInfo.eBulletType = tBulletInfo.TYPE_ROCK;
		tBulletInfo.f3Start_Pos = _float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z);
		tBulletInfo.f3Target_Pos = _float3(f4PlayerPos.x, f4PlayerPos.y + 1.0f, f4PlayerPos.z);

		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Bullet"), TEXT("Prototype_GameObject_B_RandomBullet"), &tBulletInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		RELEASE_INSTANCE(CGameInstance);

		++m_iBullet_Count;
	}

	return S_OK;
}

HRESULT CM_Gary_Boss::A_Stun_Tick(const _double & TimeDelta)
{
	m_bHit = false;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4MyPos, vMyPos);

	if (0 == m_dSkill_TimeAcc)
	{
		m_bShader_Alpha = true;
		CEffect_Manager::GetInstance()->Effect_Smoke_Count(_float3(f4MyPos.x, f4MyPos.y + 1.2f, f4MyPos.z - 1.5f), _float3(0.0f, 0.0f, 0.0f), 70, { 0.3f, 2.0f });
	}

	m_dSkill_TimeAcc += TimeDelta;
	if (1 < m_dSkill_TimeAcc)
	{
		m_bShader_Alpha = false;
		m_pTransformCom->Set_Pos(_float3(2.43f, 0.0f, 10.75f));      // 특정 위치로 순간이동
	}

	if (2 < m_dSkill_TimeAcc)
	{
		if (false == m_bEffect)   // 한 번만 호출되기 위해서
		{
			CEffect_Manager::GetInstance()->Effect_Boss_SkeletonBig(_float3(f4MyPos.x, f4MyPos.y + 3.0f, f4MyPos.z));				 // 위
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x, f4MyPos.y + 0.6f, f4MyPos.z + 1.0f));         // 뒤
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z + 1.0f));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // 왼쪽
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x - 2.0f, f4MyPos.y + 1.0f, f4MyPos.z));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // 왼쪽 대각선
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x - 1.1f, f4MyPos.y + 1.0f, f4MyPos.z + 0.6f));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // 오른쪽
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x + 2.0f, f4MyPos.y + 1.0f, f4MyPos.z));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // 오른쪽 대각선
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x + 1.1f, f4MyPos.y + 1.0f, f4MyPos.z + 0.6f));

			m_bEffect = true;
		}

		m_dEffect_TimeAcc += TimeDelta;
		if (0.5 < m_dEffect_TimeAcc)
		{
			// 뒤에 불 이펙트
			// 현재 보스의 위치
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4(&f4MyPos, vMyPos);

			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x, f4MyPos.y + 0.6f, f4MyPos.z + 1.0f));         // 뒤
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // 왼쪽
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // 왼쪽 대각선
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // 오른쪽
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // 오른쪽 대각선

			m_dEffect_TimeAcc = 0;
		}
	}

	if (5 < m_dSkill_TimeAcc)
	{
		m_eAnimState = A_STUN;

		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Scream"), TEXT("Prototype_GameObject_Boss_S_Scream"), &(_float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z)))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_eState = IDLE;
		m_bEffect = false;
		m_dSkill_TimeAcc = 0;
		m_dEffect_TimeAcc = 0;
	}

	return S_OK;
}

HRESULT CM_Gary_Boss::A_Cage_Tick(const _double & TimeDelta)
{
	if (false == m_bMovePos)
		m_iEffect_Count = 0;

	m_bShader_Alpha = false;
	m_bMovePos = true;

	m_eAnimState = IDLE;
	m_pTransformCom->Set_Pos(_float3(6.2f, 1.5f, 20.0f));
	m_pTransformCom->LookAt(XMVectorSet(6.2f, 1.5f, 19.0f, 1.0f));

	if (0 == m_dSkill_TimeAcc)   // 한 번만!
	{
		// Cage 안에 갇혔음을 확인하기 위한 변수
		CObj_Manager::GetInstance()->Set_BossCage(true);

		// 플레이어 위치를 변경한다.
		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform * pFinn_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));
		CNavigation* pFinn_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));

		pFinn_TransformCom->Set_Pos(_float3(8.0f, 0.0f, 14.0f));
		pFinn_NavigationCom->Set_CellIndex(400);

		CTransform * pJake_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
		CNavigation* pJake_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));

		pJake_TransformCom->Set_Pos(_float3(8.0f, 0.0f, 14.5f));
		pJake_NavigationCom->Set_CellIndex(400);

		// 플레이어 에게 이펙트
		CEffect_Manager::GetInstance()->Effect_Boss_Potal_Create(_float3(8.0f, 1.5f, 14.0f - 1.0f));
		CEffect_Manager::GetInstance()->Effect_Potal_Star_Create(_float3(8.0f, 1.5f, 14.0f - 1.3f));
		CEffect_Manager::GetInstance()->Effect_Boss_Potals_Create(_float3(8.0f, 1.5f, 14.0f - 1.2f));


		// Cage 를 생성한다.
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Cage"), TEXT("Prototype_GameObject_Boss_S_Cage"), &(_float3(5.1f, 0.0f, 16.0f)))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	m_dSkill_TimeAcc += TimeDelta;

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);
	CBoss_S_Cage * pGameObject = dynamic_cast<CBoss_S_Cage*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Cage"), TEXT("Prototype_GameObject_M_Gary_Boss"), 0));
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == pGameObject)      // 이 객체가 nullptr 이라면 삭제된 것 이니 다른 스킬을 사용한다.
	{
		CObj_Manager::GetInstance()->Set_BossCage(false);

		m_bShader_Alpha = true;
		m_eState = IDLE;
		m_dSkill_TimeAcc = 0;
	}

	return S_OK;
}

void CM_Gary_Boss::A_Dance_Tick(const _double & TimeDelta)
{
	if (false == m_bMovePos)
		m_iEffect_Count = 0;

	m_bShader_Alpha = false;
	m_bMovePos = true;

	m_eAnimState = A_DANCE;
	m_pTransformCom->Set_Pos(_float3(6.2f, 2.0f, 20.5f));
	m_pTransformCom->LookAt(XMVectorSet(6.2f, 2.0f, 20.0f, 1.0f));

	m_dSkill_TimeAcc += TimeDelta;
	if (0.5 < m_dSkill_TimeAcc)
	{
		m_fHP += 10.0f;
		m_dSkill_TimeAcc = 0;
	}

	Fann_Create();
	if (true == Fann_Dead_Check())   // 생성한 팬이 모두 삭제 되었다면, 다른 패턴
	{
		m_bShader_Alpha = true;
		m_eState = IDLE;
		m_dSkill_TimeAcc = 0;
	}
}

HRESULT CM_Gary_Boss::Fann_Create()
{
	if (true == m_bSkill)   // 객체가 다 생성된 후 호출되지 않는다.
		return S_OK;

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Fan_0"), TEXT("Prototype_GameObject_Boss_Fan"), &_float3(2.5f, 0.0f, 17.8f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Fan_1"), TEXT("Prototype_GameObject_Boss_Fan"), &_float3(4.0f, 0.0f, 17.4f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Fan_2"), TEXT("Prototype_GameObject_Boss_Fan"), &_float3(5.5f, 0.0f, 17.0f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Fan_3"), TEXT("Prototype_GameObject_Boss_Fan"), &_float3(6.8f, 0.0f, 16.7f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Boss_Fan_4"), TEXT("Prototype_GameObject_Boss_Fan"), &_float3(8.0f, 0.0f, 16.3f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	m_bSkill = true;

	return S_OK;
}

_bool CM_Gary_Boss::Fann_Dead_Check()
{
	if (false == m_bSkill)   // 객체다 다 생성되면 다음을 실행한다.
		return false;

	_bool   arrSkeleton_Dead[5] = { false };
	_int   iDead_Count = 0;

	for (_int i = 0; i < 5; ++i)
	{
		_tchar   szName[20];
		wsprintf(szName, TEXT("Layer_Boss_Fan_%d"), i);

		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);
		CBoss_Fan * pGameObject = dynamic_cast<CBoss_Fan*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS, szName, TEXT("Prototype_GameObject_Boss_Fan"), 0));
		RELEASE_INSTANCE(CGameInstance);

		if (nullptr == pGameObject)      // 이 객체가 nullptr 이라면 삭제된 것 이니 다른 스킬을 사용한다.
			arrSkeleton_Dead[i] = true;
	}

	for (_int i = 0; i < 5; ++i)
	{
		if (true == arrSkeleton_Dead[i])
			++iDead_Count;
	}

	if (5 <= iDead_Count)
	{
		m_bSkill = false;         // 초기화
		return true;
	}

	return false;
}

void CM_Gary_Boss::Hit_Tick(const _double & TimeDelta)
{
	if (false == m_bHit)
		return;

	if (0 == m_dShader_Hit_TimeAcc)
	{ 
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4MyPos, vMyPos);
		CEffect_Manager::GetInstance()->Effect_Color_Hit_Create(_float3(f4MyPos.x, f4MyPos.y + 1.2f, f4MyPos.z - 0.7f), _float3(0.9f, 0.0f, 0.1f));

		m_fHP -= CObj_Manager::GetInstance()->Get_Current_Player().fAttack;
		
		if(m_eState == STATE::IDLE || m_eState == STATE::MOVE)
			m_eAnimState = STATE::HIT;

		m_bShader_Hit = true;
	}

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.2 > m_dShader_Hit_TimeAcc)
	{
		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.2f);
	}

	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (0.4 < m_dShader_Hit_TimeAcc)
	{
		m_dShader_Hit_TimeAcc = 0;
		m_bShader_Hit = false;

		m_bHit = false;

		if(STATE::HIT == m_eAnimState)
			m_eState = STATE::IDLE;
	}
}

void CM_Gary_Boss::Die_Tick(const _double & TimeDelta)
{
	m_eAnimState = STATE::DIE;

}

void CM_Gary_Boss::LookChange(const _double & TimeDelta)
{
	m_pTransformCom->Go_Backward(_float(TimeDelta) * 5.0f);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.0f);

	m_dLookChange_TimeAcc += TimeDelta;
	if (0.5 < m_dLookChange_TimeAcc)
	{
		m_bLookChange = false;
		m_dLookChange_TimeAcc = 0.0;
	}
}

void CM_Gary_Boss::Effect_Tick(const _double & TimeDelta)
{
	if (3 <= m_iEffect_Count)
	{
		//m_iEffect_Count = 0; 다른 함수에서 이펙트를 사용하고자 할 때 0으로 만들어 주면 된다.
		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4Pos, vPos);

	if (0 == m_iEffect_Count)
	{
		CEffect_Manager::GetInstance()->Effect_Boss_Potal_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.0f));
		CEffect_Manager::GetInstance()->Effect_Potal_Star_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.3f));
		++m_iEffect_Count;
	}

	m_dEffect_TimeAcc += TimeDelta;
	if (0.3 < m_dEffect_TimeAcc)
	{
		CEffect_Manager::GetInstance()->Effect_Boss_Potal_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.0f));
		CEffect_Manager::GetInstance()->Effect_Potal_Star_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.3f));
		CEffect_Manager::GetInstance()->Effect_Boss_Potals_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.2f));

		++m_iEffect_Count;
		m_dEffect_TimeAcc = 0;
	}
}

void CM_Gary_Boss::Shader_Alpha(const _double & TimeDelta)
{
	if (false == m_bShader_Alpha && 1 <= m_fAlpha)
		return;

	m_bHit = false;

	if (true == m_bShader_Alpha)
	{
		if (0.0f < m_fAlpha)
			m_fAlpha -= _float(TimeDelta) * 1.5f;
	}
	else
	{
		if (1.0f > m_fAlpha)
			m_fAlpha += _float(TimeDelta) * 1.5f;
	}
}

void CM_Gary_Boss::CutScene_Tick(const _double & TimeDelta)
{
	if (true == m_bCutSceneEnd)
		return;

	Shader_Alpha(TimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4Pos, vPos);

	if (0 == m_dSkill_TimeAcc)
	{
		CEffect_Manager::GetInstance()->Effect_Boss_Potal_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.0f));
		CEffect_Manager::GetInstance()->Effect_Potal_Star_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.3f));
		CEffect_Manager::GetInstance()->Effect_Boss_Potals_Create(_float3(f4Pos.x, f4Pos.y + 1.5f, f4Pos.z - 1.2f));
	}
	
	if(0.9 < m_dSkill_TimeAcc)
		Effect_Tick(TimeDelta);

	m_dSkill_TimeAcc += TimeDelta;
	if (1.0 < m_dSkill_TimeAcc)
	{
		m_pModelCom->Set_AnimIndex(5);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(4.0f, 0.0f, 17.0f, 1.0f));	// 좌표 처음으로 이동
	}
	if (2.5 < m_dSkill_TimeAcc)
	{
		m_bCutSceneEnd = true;
		m_dEffect_TimeAcc = 0.0;
	}
}

void CM_Gary_Boss::CutScene_Effect()
{
	static _bool	bCutSceneEffect;

	if (false == bCutSceneEffect)
	{
		bCutSceneEffect = true;

		CEffect_Manager::GetInstance()->Effect_Boss_CutScene({ 6.2f, 1.5f, 21.0f });
		CEffect_Manager::GetInstance()->Effect_Boss_CutScene({ 7.2f, 1.5f, 20.5f });
		CEffect_Manager::GetInstance()->Effect_Boss_CutScene({ 5.2f, 1.5f, 20.5f });
	}
}

CM_Gary_Boss * CM_Gary_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Gary_Boss*      pInstance = new CM_Gary_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Gary_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Gary_Boss::Clone(void * pArg)
{
	CM_Gary_Boss*      pInstance = new CM_Gary_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Gary_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Gary_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}