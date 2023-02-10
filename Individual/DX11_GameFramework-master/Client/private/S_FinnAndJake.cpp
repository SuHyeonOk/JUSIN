#include "stdafx.h"
#include "..\public\S_FinnAndJake.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Skill_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

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

	m_wsTag = L"Finn";
	m_eAnim_State = STATE_END;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(3);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Stop_Sound(0);
	pGameInstance->Play_Sound(TEXT("Scroll_fiona.mp3"), 0.5f, false, 3);
	RELEASE_INSTANCE(CGameInstance);

	CObj_Manager::GetInstance()->Set_Camera(CObj_Manager::PLAYERINFO::PLAYER::FINNANDJAKE);

	return S_OK;
}

void CS_FinnAndJake::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	End_Tick();
	Return_Tick();
	Hit_Tick(TimeDelta);
	KeyInput(TimeDelta);
	AnimatedMovie_Tick();






	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_B))
	{
		m_bEnd = true;
	}

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
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
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
			if (m_bShader_Hit)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
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

void CS_FinnAndJake::On_Collision(CGameObject * pOther)
{
	if (L"Knives_Rain" == pOther->Get_Tag())
	{
		m_OnHit = true;
		CObj_Manager::GetInstance()->Set_Player_MinusHP(10.0f);
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
		m_pModelCom->Set_AnimIndex(1);
		break;
	case Client::CS_FinnAndJake::FINISH:
		m_pModelCom->Set_AnimIndex(2);
		break;
	}
}

void CS_FinnAndJake::KeyInput(const _double & TimeDelta)
{
	if (true == m_OnHit)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_OnMove)
	{
		m_eAnim_State = MOVE;
		pGameInstance->Stop_Sound(5);
		m_pTransformCom->Go_Straight(TimeDelta);
		m_pTransformCom->PlayerMove(XMVectorSet(m_f4NewLook.x, m_f4NewLook.y, m_f4NewLook.z, m_f4NewLook.w), TimeDelta);
	}
		
#pragma region 이동
	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_OnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, 1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_OnMove = true;
		m_f4NewLook = { 1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_OnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, -1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_OnMove = true;
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
		m_OnMove = false;
		pGameInstance->Stop_Sound(7);
	}

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		pGameInstance->Play_Sound(TEXT("sfx_finn_sword_1.ogg"), 0.7f, false, 1);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CS_FinnAndJake::Hit_Tick(const _double & TimeDelta)
{
	if (false == m_OnHit)
		return;

	m_eAnim_State = HIT;
	m_bShader_Hit = true;
	m_OnMove = false;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (0.7 < m_dShader_Hit_TimeAcc)
	{
		m_OnHit = false;
		m_eAnim_State = IDLE;
		m_bShader_Hit = false;
		m_dShader_Hit_TimeAcc = 0.0;
	}
}

void CS_FinnAndJake::Return_Tick()
{
	if (0.0f == CObj_Manager::GetInstance()->Get_Current_Player().fHP)
	{
		CObj_Manager::GetInstance()->Set_Player_PlusHP(CObj_Manager::GetInstance()->Get_Current_Player().fHPMax);

		// 플레이어 처음 위치로
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_f4StartPoition));
		// 카메라 처음 위치로
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform * pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
		pObjTransformCom->Set_Pos(_float3(-5.0f, 3.7f, -20.0f));
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CS_FinnAndJake::End_Tick()
{
	if (false == m_bEnd)
		return;

	CObj_Manager::GetInstance()->Set_Loading_Count();
	CObj_Manager::GetInstance()->Set_NextLevel(true);
	CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);
	CObj_Manager::GetInstance()->Set_Camera(CObj_Manager::PLAYERINFO::PLAYER::FINN);
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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
