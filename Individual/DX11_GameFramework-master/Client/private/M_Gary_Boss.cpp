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

	if (false == m_bCutScene)							// �ܺο��� �ƾ��� �������� �˷��ش�.
	{
		CutScene_Effect();
		return;
	}
	else
		CutScene_Tick(TimeDelta);						// �� ���� �� �� ���� �ǰ� �� �Ŀ� �� �̻� ���� ���� ���� �Լ� �̴�.

	if (true == m_bCutSceneEnd)							// ��ü �� ���� �ƾ��� ������ �� ���� ������ �� �ֵ��� �ϰ�, ���������� ���� �Ǿ���� �Լ��� ���� ��Ų��.
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
	//if (L"Object_Collider" == pOther->Get_Tag())	// �߰��� �ذ��� ������ �Ǹ� Look �� �����϶�
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
	// ü���� 0 �̵Ǹ� �״´�.
	if (0 >= m_fHP)
		m_eState = DIE;

	Hit_Tick(TimeDelta);   // Hit ���´� ���� �ٸ� Tick �� ��� ���ư��⸦ ���Ѵ�.

	switch (m_eState)
	{
	case Client::CM_Gary_Boss::IDLE:
		// �Ÿ� 7 �̳��� �÷��̾ �ִ��� Ȯ���ϰ�, ������ ���� ����
		Idle_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_MOVE:
		// �÷��̾ ���� ������ �޷�����, ���� ģ��.
		A_Move_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_BULLET:
		// �÷��̾ ���� �Ҳ� �Ѿ��� �߻��Ѵ�.
		A_Bullet_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_STUN:
		// �������� �÷��̾ stun ��Ų��.
		A_Stun_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_CAGE:
		// �÷��̾ ���� �̵� ���Ѽ� ������ �� �� ���ΰ�, ����� ��ȯ�Ѵ�. (���ʹ� �þ߿� ������ �ʴ´�.)
		A_Cage_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_DANCE:
		// ��� �� ���� �ö󰡰�, �Ʒ� ������� ���� �߸� ������ ü���� ȸ���Ѵ�. 
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
	// 0 : ������ ��Ÿ
	// 1 : �޼� ��Ÿ
	// 2 : �ڷ� �Ѿ�� DIE??
	// 3 : ���� Hit
	// 4 : �� ��� ����
	// 5 : IDLE
	// 6 : ��
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
	// ������ ������ ������ MovePos �� true ��� ó�� ��ġ�� �̵� ���Ѿ� �Ѵ�.
	if (true == m_bMovePos)
	{
		if (0 == m_dSkill_TimeAcc)
			m_iEffect_Count = 0;
		m_pTransformCom->LookAt(XMVectorSet(4.0f, 2.0f, 16.0f, 1.0f));
		if (1 < m_dSkill_TimeAcc)   // �ʹ� �ٷ� �̵��ؼ� 1�� �ִٰ� �̵�
		{
			m_bShader_Alpha = false;

			m_pTransformCom->Set_Pos(_float3(4.0f, 0.2f, 17.0f));
			m_bMovePos = false;
			m_iEffect_Count = 0;
		}
	}

	m_eAnimState = IDLE;

	_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (7.0f < fDistance)      // ���� ���� �� �̶�� �׳� �̵��ϰ� �ִ´�.
		RandomMove(TimeDelta);
	
	Random_Skill(TimeDelta);
}

void CM_Gary_Boss::Random_Skill(const _double& TimeDelta)
{
	m_dSkill_TimeAcc += TimeDelta;   // �ð��ʸ� �� ����.

	static _int iRandom;
	static _bool bRandomSuccess;
	static _bool bPotalEffect;

	if (3 > m_dSkill_TimeAcc)      // 3�� �������� �����ϰ� ��ų�� �̴µ�
	{
		if (false == bRandomSuccess)
		{
			_int   iMinRandomNumber = 0;
			_int   iMaxRandomNumber = 0;

			// DANCE ��ų�� ��� ������ ü���� 30% ������ �� ����ȴ�.
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

		// �����̶� �ٸ� ������ �̾Ҵٸ�, ���̻� ��ų�� ���� �ʰ�, �׷��� ���� ��쿡�� �׳� �� ��ų�� �����Ų��.
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

		// ������ ��ǥ�� �� �� ���Ѵ�.
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

	// ������ ��ǥ�� �̵��Ѵ�.
	_vector vMovePos = XMVectorSet(m_f4MovemPos.x, m_f4MovemPos.y, m_f4MovemPos.z, m_f4MovemPos.w);
	m_pTransformCom->Chase(vMovePos, TimeDelta, 0.0f);
	m_pTransformCom->LookAt(vMovePos);

	_vector vDistance = vMovePos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float   fDistace = XMVectorGetX(XMVector3Length(vDistance));

	// ������ ��ġ�� ���� �� ��ǥ�� 0.5f ���϶��! ���̵� ���·� �ִٰ� �ٽ� �̵��Ѵ�.
	if (0.5f > fDistace)
	{
		m_eAnimState = STATE::IDLE;
		m_bMove = false;				// �ٽ� ������ ��ǥ�� ���Ѵ�.
	}
}

HRESULT CM_Gary_Boss::A_Move_Tick(const _double & TimeDelta)
{
	// �÷��̾� ��ó���� �޷��ͼ� �ٶ��� ������.
	m_eAnimState = STATE::MOVE;

	if (false == m_bSkill)
	{
		m_bSkill = true;
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		XMStoreFloat4(&m_f4PlayerPos, vPlayerPos);
	}

	_vector vPlayerPos = XMVectorSet(m_f4PlayerPos.x, 0.0f, m_f4PlayerPos.z, m_f4PlayerPos.w);
	m_pTransformCom->Chase(vPlayerPos, TimeDelta * 1.7, 1.9f);   // �Ÿ��� �� ���� �÷��̾ ���󰣴�.
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
			// �� ��ǥ
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

	// �÷��̾���� ��ǥ�� 3�� �޾Ƽ� �� �� �� �Ѿ��� �߻��Ѵ�.
	if (m_pModelCom->Get_Finished())
	{
		// �� ��ǥ
		_vector   vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4   f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4(&f4MyPos, vMyPos);

		// �÷��̾� ��ǥ
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
		m_pTransformCom->Set_Pos(_float3(2.43f, 0.0f, 10.75f));      // Ư�� ��ġ�� �����̵�
	}

	if (2 < m_dSkill_TimeAcc)
	{
		if (false == m_bEffect)   // �� ���� ȣ��Ǳ� ���ؼ�
		{
			CEffect_Manager::GetInstance()->Effect_Boss_SkeletonBig(_float3(f4MyPos.x, f4MyPos.y + 3.0f, f4MyPos.z));				 // ��
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x, f4MyPos.y + 0.6f, f4MyPos.z + 1.0f));         // ��
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z + 1.0f));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // ����
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x - 2.0f, f4MyPos.y + 1.0f, f4MyPos.z));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // ���� �밢��
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x - 1.1f, f4MyPos.y + 1.0f, f4MyPos.z + 0.6f));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // ������
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x + 2.0f, f4MyPos.y + 1.0f, f4MyPos.z));
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // ������ �밢��
			CEffect_Manager::GetInstance()->Effect_Burn_Fire_Create(_float3(f4MyPos.x + 1.1f, f4MyPos.y + 1.0f, f4MyPos.z + 0.6f));

			m_bEffect = true;
		}

		m_dEffect_TimeAcc += TimeDelta;
		if (0.5 < m_dEffect_TimeAcc)
		{
			// �ڿ� �� ����Ʈ
			// ���� ������ ��ġ
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4(&f4MyPos, vMyPos);

			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x, f4MyPos.y + 0.6f, f4MyPos.z + 1.0f));         // ��
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // ����
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x - 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // ���� �밢��
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 2.0f, f4MyPos.y + 0.6f, f4MyPos.z));         // ������
			CEffect_Manager::GetInstance()->Effect_Wave_Fire_Create(_float3(f4MyPos.x + 1.1f, f4MyPos.y + 0.6f, f4MyPos.z + 0.6f));  // ������ �밢��

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

	if (0 == m_dSkill_TimeAcc)   // �� ����!
	{
		// Cage �ȿ� �������� Ȯ���ϱ� ���� ����
		CObj_Manager::GetInstance()->Set_BossCage(true);

		// �÷��̾� ��ġ�� �����Ѵ�.
		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform * pFinn_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));
		CNavigation* pFinn_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));

		pFinn_TransformCom->Set_Pos(_float3(8.0f, 0.0f, 14.0f));
		pFinn_NavigationCom->Set_CellIndex(400);

		CTransform * pJake_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
		CNavigation* pJake_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));

		pJake_TransformCom->Set_Pos(_float3(8.0f, 0.0f, 14.5f));
		pJake_NavigationCom->Set_CellIndex(400);

		// �÷��̾� ���� ����Ʈ
		CEffect_Manager::GetInstance()->Effect_Boss_Potal_Create(_float3(8.0f, 1.5f, 14.0f - 1.0f));
		CEffect_Manager::GetInstance()->Effect_Potal_Star_Create(_float3(8.0f, 1.5f, 14.0f - 1.3f));
		CEffect_Manager::GetInstance()->Effect_Boss_Potals_Create(_float3(8.0f, 1.5f, 14.0f - 1.2f));


		// Cage �� �����Ѵ�.
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

	if (nullptr == pGameObject)      // �� ��ü�� nullptr �̶�� ������ �� �̴� �ٸ� ��ų�� ����Ѵ�.
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
	if (true == Fann_Dead_Check())   // ������ ���� ��� ���� �Ǿ��ٸ�, �ٸ� ����
	{
		m_bShader_Alpha = true;
		m_eState = IDLE;
		m_dSkill_TimeAcc = 0;
	}
}

HRESULT CM_Gary_Boss::Fann_Create()
{
	if (true == m_bSkill)   // ��ü�� �� ������ �� ȣ����� �ʴ´�.
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
	if (false == m_bSkill)   // ��ü�� �� �����Ǹ� ������ �����Ѵ�.
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

		if (nullptr == pGameObject)      // �� ��ü�� nullptr �̶�� ������ �� �̴� �ٸ� ��ų�� ����Ѵ�.
			arrSkeleton_Dead[i] = true;
	}

	for (_int i = 0; i < 5; ++i)
	{
		if (true == arrSkeleton_Dead[i])
			++iDead_Count;
	}

	if (5 <= iDead_Count)
	{
		m_bSkill = false;         // �ʱ�ȭ
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
		//m_iEffect_Count = 0; �ٸ� �Լ����� ����Ʈ�� ����ϰ��� �� �� 0���� ����� �ָ� �ȴ�.
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
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(4.0f, 0.0f, 17.0f, 1.0f));	// ��ǥ ó������ �̵�
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