#include "stdafx.h"
#include "..\public\S_FinnAndJake.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Skill_Manager.h"
#include "Effect_Manager.h"
#include "Utilities_Manager.h"

#include "Korean_Food.h"

CS_FinnAndJake::CS_FinnAndJake(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_FinnAndJake::CS_FinnAndJake(const CS_FinnAndJake & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_FinnAndJake::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_FinnAndJake::Initialize(void * pArg)
{
	_float3 fPosition = { 0.0f, 0.0f, 0.0f };

	if (nullptr != pArg)
		memcpy(&fPosition, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = fPosition;
	m_f4StartPoition = { fPosition.x, fPosition.y, fPosition.z, 1.0f };

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_wsTag = L"Jake";
	m_eAnim_State = STATE_END;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(3);

	CObj_Manager::GetInstance()->Set_Camera(CObj_Manager::PLAYERINFO::PLAYER::FINNANDJAKE);

	return S_OK;
}

void CS_FinnAndJake::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Return_Tick(TimeDelta);		// 체력이 0이 되는 순간 처음으로 돌아간다.
	Hit_Tick(TimeDelta);		// 공격 받았을 때
	AnimatedMovie_Tick();		// 애니메이션 돌아가는 함수
	KeyInput(TimeDelta);		// 플레이어 키 입력 8방향으로만 이동가능

	Rainicorn(TimeDelta);		// 무지개 콘을 만났을 때 실행 될 함수 
	KnockBack_Tick(TimeDelta);	// 트랩을 밟았을 때의 처리

	Shader_Alpha(TimeDelta);	// 알파값 증가 및 감소

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_V))
	{
		//////////////////////////// 디버그용
		_vector vddMyPos;
		vddMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float4	f4ddMyPos;
		XMStoreFloat4(&f4ddMyPos, vddMyPos);

		cout << "게임 플레이어 위치" << f4ddMyPos.x << " | " << f4ddMyPos.y << " | " << f4ddMyPos.z << endl;
		//////////////////////////// 디버그용

	}

	RELEASE_INSTANCE(CGameInstance);



}

void CS_FinnAndJake::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);		// 충돌처리
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
	{
		if (1 != m_fAlpha)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		else
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_XRAYBLEND, this);
		}
	}
}

HRESULT CS_FinnAndJake::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (3 == i)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
		{
			if (1 != m_fAlpha)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);	// 죽을 때
			else
			{
				if (m_bShader_Hit)
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
				else
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");		// 평소
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

HRESULT CS_FinnAndJake::Render_XRay()
{
	if (true == m_bShader_Hit || 1 != m_fAlpha)
		return S_OK;

	if (FAILED(__super::Render_XRay()))
		return E_FAIL;

	if (nullptr == m_pShaderXRayCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderXRayCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float	fObjectID = 4.0f;
	if (FAILED(m_pShaderXRayCom->Set_RawValue("g_ObjectID", &fObjectID, sizeof _float)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (3 == i)
			return S_OK;

		m_pModelCom->Bind_Material(m_pShaderXRayCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderXRayCom, i, "g_BoneMatrices", 0);
	}
	return S_OK;
}

void CS_FinnAndJake::On_Collision(CGameObject * pOther)
{
	if (L"Knives_Rain" == pOther->Get_Tag())
	{
		m_bOnHit = true;
		CObj_Manager::GetInstance()->Set_Player_MinusHP(20.0f);
	}

	if (L"Squirrel" == pOther->Get_Tag())
	{
		m_bOnHit = true;
		CObj_Manager::GetInstance()->Set_Player_MinusHP(30.0f);
		_float4 f4Position = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		CEffect_Manager::GetInstance()->Efect_MiniGame_Squirrel({ f4Position.x, f4Position.y + 2.0f, f4Position.z - 1.0f });
	}

	if (L"Lady_Rainicorn" == pOther->Get_Tag())
	{
		m_bRainicorn = true;
	}
	
	if(L"Cake_Blue" == pOther->Get_Tag())
	{
		// 달팽이가 있는 위치로 순간 이동
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform * pSnail_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_MINIGAME, TEXT("Layer_Snail"), TEXT("Com_Transform"), 0));
		_float4 f4Snail_Position = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4Snail_Position, pSnail_TransformCom->Get_State(CTransform::STATE_TRANSLATION));

		// 내 위치를 달팽이 위치로 변경
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, pSnail_TransformCom->Get_State(CTransform::STATE_TRANSLATION));
		
		CTransform * pCamra_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
		
		// 카메라 위치를 달팽이 위치로 변경
		pCamra_TransformCom->Set_Pos({ f4Snail_Position.x, f4Snail_Position.y + 3.7f, f4Snail_Position.z - 6.0f});
		
		RELEASE_INSTANCE(CGameInstance);

		// 이펙트
		CEffect_Manager::GetInstance()->Effect_Smoke_Count({ f4Snail_Position.x, f4Snail_Position.y + 1.0f, f4Snail_Position.z }, { 0.352f, 0.694f, 0.619f }, { 20 }, { 0.5f, 1.5f });
	}

	if (L"Cake_Magenta" == pOther->Get_Tag())
	{
		// 출발점 으로
		m_pTransformCom->Set_Pos({ -4.0f, 0.0f, -20.0f });			// 내위치

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform * pCamra_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
		pCamra_TransformCom->Set_Pos({ -4.0f, 3.7f, -26.0f });		// 카메라 위치
		RELEASE_INSTANCE(CGameInstance);
	
		// 이펙트
		CEffect_Manager::GetInstance()->Effect_Smoke_Count({ -4.0f, 2.0f, -20.0f }, { 0.352f, 0.694f, 0.619f }, { 20 }, { 0.5f, 1.5f });
	}

	if (L"Cake_Yellow" == pOther->Get_Tag())
	{
		// 도착지점 조금 전 으로
		m_pTransformCom->Set_Pos({ 85.342f, 0.0f, 31.9937f });		// 내위치

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform * pCamra_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
		pCamra_TransformCom->Set_Pos({ 85.342f, 3.7f, 26.0f });		// 카메라 위치
		RELEASE_INSTANCE(CGameInstance);
	
		// 이펙트
		CEffect_Manager::GetInstance()->Effect_Smoke_Count({ 85.342f, 1.0f, 31.9937f }, { 0.352f, 0.694f, 0.619f }, { 20 }, { 0.5f, 1.5f });
	}

	if (L"Korean_Food" == pOther->Get_Tag())
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4Position = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4Position, vPosition);

		CKorean_Food::OBJECTINFO	m_Korean_Food;
		m_Korean_Food.eType = CKorean_Food::PLAYER;
		m_Korean_Food.f3Position = _float3(f4Position.x, f4Position.y, f4Position.z);
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Korean_Food"), TEXT("Prototype_GameObject_Korean_Food"), &m_Korean_Food)))
			return;
		RELEASE_INSTANCE(CGameInstance);
	}

	if (L"Object_BeapTrap" == pOther->Get_Tag())
	{
		m_OnKnockBack = true;
	}
}

HRESULT CS_FinnAndJake::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_ShaderXRay */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel_XRay"), TEXT("Com_ShaderXRay"),
		(CComponent**)&m_pShaderXRayCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_FinnAndJake"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 2.0f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_FinnAndJake::SetUp_ShaderResources()
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

	if (1 != m_fAlpha)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
			return E_FAIL;
	}

	return S_OK;
}

void CS_FinnAndJake::AnimatedMovie_Tick()
{
	// [0] 올라가기
	// [1] 손 점프
	// [2] 핀 손흔들기
	// [3] 가만히
	// [4] 점프
	// [5] 달리기
	// [6] 내려가기
	// [7] 똥땅똥땅 걷기

	switch (m_eAnim_State)
	{
	case Client::CS_FinnAndJake::IDLE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CS_FinnAndJake::MOVE:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case Client::CS_FinnAndJake::HIT:
		m_pModelCom->Set_AnimIndex(1, false);
		break;
	case Client::CS_FinnAndJake::FINISH:
		m_pModelCom->Set_AnimIndex(2);
		break;
	}
}

void CS_FinnAndJake::KeyInput(const _double & TimeDelta)
{
	if (true == m_bOnHit)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_bOnMove)
	{
		m_eAnim_State = MOVE;
		m_pTransformCom->Go_Straight(TimeDelta);
		m_pTransformCom->PlayerMove(XMVectorSet(m_f4NewLook.x, m_f4NewLook.y, m_f4NewLook.z, m_f4NewLook.w), TimeDelta);
	}
		
#pragma region 이동
	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_bOnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, 1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_bOnMove = true;
		m_f4NewLook = { 1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_bOnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, -1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_bOnMove = true;
		m_f4NewLook = { -1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}
#pragma endregion

	if (pGameInstance->Key_Up(DIK_UP) || pGameInstance->Key_Up(DIK_RIGHT) || pGameInstance->Key_Up(DIK_DOWN) || pGameInstance->Key_Up(DIK_LEFT))
	{
		m_eAnim_State = IDLE;
		m_bOnMove = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CS_FinnAndJake::Hit_Tick(const _double & TimeDelta)
{
	if (false == m_bOnHit)
		return;

	m_eAnim_State = HIT;
	m_bShader_Hit = true;
	m_bOnMove = false;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (1 == m_pModelCom->Get_AnimIndex() && 4 == m_pModelCom->Get_Keyframes())
	{
		m_bOnHit = false;
		m_eAnim_State = IDLE;
		m_bShader_Hit = false;
		m_dShader_Hit_TimeAcc = 0.0;
	}
}

void CS_FinnAndJake::KnockBack_Tick(const _double & TimeDelta)
{
	if (false == m_OnKnockBack)
		return;

	m_bShader_Hit = true;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;
	if (0.2 < m_dShader_Hit_TimeAcc)
		m_dShader_Hit_TimeAcc = 0.0;

	if (0.0 == m_dKnockBack_TimeAcc)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_Sound(TEXT("Cine_Witch_Death_Fall_Wood.ogg"), 0.6f);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_dKnockBack_TimeAcc += TimeDelta;
	if (0.7 < m_dKnockBack_TimeAcc)
	{
		m_OnKnockBack = false;
		m_eAnim_State = IDLE;
		m_bShader_Hit = false;
		m_dShader_Hit_TimeAcc = 0.0;
		m_dKnockBack_TimeAcc = 0.0;
	}
}

void CS_FinnAndJake::Return_Tick(const _double & TimeDelta)
{
	if (0.0f >= CObj_Manager::GetInstance()->Get_Current_Player().fHP)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (1.0f == m_fAlpha)
		{
			pGameInstance->Play_Sound(TEXT("BMO_gameover.ogg"), 1.0f);

			m_bAlpha_Down = true;
		}

		if (true == m_bAlpha_Change)
		{
			pGameInstance->Play_Sound(TEXT("BMO_startover.ogg"), 1.0f);
			
			m_bAlpha_Change = false;

			m_eAnim_State = IDLE;
			CObj_Manager::GetInstance()->Set_Player_PlusHP(CObj_Manager::GetInstance()->Get_Current_Player().fHPMax);

			// 플레이어 처음 위치로
			m_pTransformCom->Set_Pos({ -4.0f, 0.0f, -20.0f });
			// 카메라 처음 위치로
			CTransform * pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
			pObjTransformCom->Set_Pos({ -4.0f, 3.7f, -26.0f, });
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CS_FinnAndJake::Rainicorn(const _double & TimeDelta)
{
	if (false == m_bRainicorn)
		return;

	if (0.0 == m_dRainicorn_TimeAcc)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_Sound(TEXT("Lady_Rainicorn.mp3"), 0.8f);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_dRainicorn_TimeAcc += TimeDelta;
	if (5.0 < m_dRainicorn_TimeAcc)		// 5초가 지난 후에 서야 키 입력을 할 수 있다.
	{
		m_bRainicorn = false; 
		m_dEffect_TimeAcc = 0.0;
		m_dRainicorn_TimeAcc = 0;
	}
	else
	{
		m_eAnim_State = MOVE;
		if (1.5 < m_dRainicorn_TimeAcc)
			m_eAnim_State = IDLE;

		m_bOnMove = false;
		m_pTransformCom->LookAt(XMVectorSet(62.8778f, 0.0f, 27.6776f, 1.0f));
		m_pTransformCom->Chase(XMVectorSet(62.8778f, 0.0f, 27.6776f, 1.0f), TimeDelta, 2.0f);

		m_dEffect_TimeAcc += TimeDelta;
		if (0.2 < m_dEffect_TimeAcc)
		{
			CEffect_Manager::GetInstance()->Effect_MiniGame_Heart({ 61.5f, 0.5f, 26.0f },
			{ 1.0f, CUtilities_Manager::GetInstance()->Get_Random(0.0f, 0.3f), CUtilities_Manager::GetInstance()->Get_Random(0.0f, 0.5f) });

			m_dEffect_TimeAcc = 0.0;
		}
	}
}

void CS_FinnAndJake::Shader_Alpha(const _double & TimeDelta)
{
	// 알파값이 줄어들어서  0.0f 보다 작아디면 그 때 카메라를 이동시키고, 그 다음 알파값이 서서히 증가하기를 원한다.

	if (false == m_bAlpha_Down)
		return;

	if (true == m_bAlpha_Down)
	{
		if (false == m_bAlpha_Up)
		{
			if (0.0f < m_fAlpha)
				m_fAlpha -= _float(TimeDelta);

			if (0.0f > m_fAlpha)
			{
				m_bAlpha_Up = true;
				m_bAlpha_Change = true;	// 외부에서 알파값이 다 줄었을 때가 필요해서 사용한 변수
			}
		}
	}

	if (true == m_bAlpha_Up)
	{
		if (1.0f > m_fAlpha)
			m_fAlpha += _float(TimeDelta);

		if (1.0f < m_fAlpha)
		{
			m_fAlpha = 1.0f;
			m_bAlpha_Up = false;
			m_bAlpha_Down = false;
		}
	}
}

CS_FinnAndJake * CS_FinnAndJake::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_FinnAndJake*		pInstance = new CS_FinnAndJake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_FinnAndJake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_FinnAndJake::Clone(void * pArg)
{
	CS_FinnAndJake*		pInstance = new CS_FinnAndJake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_FinnAndJake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_FinnAndJake::Free()
{
	__super::Free();

	Safe_Release(m_pShaderXRayCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
