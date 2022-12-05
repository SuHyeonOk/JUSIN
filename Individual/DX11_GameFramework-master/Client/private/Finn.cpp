#include "stdafx.h"
#include "..\public\Finn.h"

#include "GameInstance.h"

#include "M_Monster.h"
#include "M_PigWarrior_BEE.h"

CFinn::CFinn(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CFinn::CFinn(const CFinn & rhs)
	: CGameObject(rhs)
{

}

HRESULT CFinn::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CFinn::Initialize(void * pArg)
{
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	m_tPlayerInfo.eState	= m_tPlayerInfo.IDLE;

	return S_OK;
}

void CFinn::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Player_Info();

}

void CFinn::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Current_Player(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFinn::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (1 == i) // �ʺ� �� : 2 /  ���� �� 1
			continue;

		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����Ѵ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CFinn::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_Finn"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CFinn::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_F))
	{
		_bool	bHit = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_bHit", &bHit, sizeof _bool)))
			return E_FAIL;
	}
	else
	{
		_bool	bHit = false;
		if (FAILED(m_pShaderCom->Set_RawValue("g_bHit", &bHit, sizeof _bool)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

void CFinn::Player_Info()
{
	//CObj_Manager::PLAYERINFO	tPlayerInfo;
	//tPlayerInfo.iHp = 

	//CObj_Manager::GetInstance()->Set_Current_Player();

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_R))
	//{
	//	CM_PigWarrior_BEE* pObj; 
	//	// TODO : Get_Object �� ����ų�, �浹ó�� �Ͽ� ��ü�� �޾ƿ� �� ������ ����

	//	CM_Monster* pMonster = dynamic_cast<CM_Monster*>(pObj);
	//	CObj_Manager::GetInstance()->Set_Player_PushHp(pMonster->Get_Monster_Attack());
	//}

	//RELEASE_INSTANCE(CGameInstance);

}

void CFinn::Current_Player(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CObj_Manager::PLAYERINFO	ePlayerInfo;
	ePlayerInfo = CObj_Manager::GetInstance()->Get_Current_Player();

	if (ePlayerInfo.ePlayer == ePlayerInfo.FINN)	// Player �����
	{
		CObj_Manager::GetInstance()->Tick_Player_Transform();		// ���� �÷��̾��� ��ǥ�� Tick
		Key_Input(TimeDelta);						// �̵��ϰ�
		Check_Follow(TimeDelta);					// ��ó�� Jake �� �ִ��� Ȯ���Ѵ�.
	}
	else
		Player_Follow(TimeDelta);		// Player �� ���� �ƴ϶�� ���󰣴�.

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Player_Follow(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CObj_Manager::PLAYERINFO	ePlayerInfo;
	ePlayerInfo = CObj_Manager::GetInstance()->Get_Current_Player();

	// Jake ���Է�
	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(ePlayerInfo.ePlayer_Level, TEXT("Layer_Jake"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�

	m_pTransformCom->LookAt(vPlayerPos);
	m_pTransformCom->Chase(vPlayerPos, TimeDelta, 2.f);

	//_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	//_vector		vDir = vPlayerPos - vMyPos;											// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	//_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X ���� �̾ƿ� �Ÿ� Ȯ��

	//if (2.0f < fDistanceX)	// �Ÿ��� 2.0f �̻��̶�� �ٶ󺸸� ���󰣴�. �ݴ�� 2.0f ���϶�� ������ �ʰ�, �ٶ����� �ʴ´�.
	//{
	//	m_pTransformCom->LookAt(vPlayerPos);
	//	m_pTransformCom->Chase(vPlayerPos, TimeDelta);
	//}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Check_Follow(_double TimeDelta)
{
	// �����ð� ���� Jake �� ��ó�� ���� �ʴٸ� Jake �� �� ��ó�� �̵���Ų��.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CObj_Manager::PLAYERINFO	ePlayerInfo;
	ePlayerInfo = CObj_Manager::GetInstance()->Get_Current_Player();

	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(ePlayerInfo.ePlayer_Level, TEXT("Layer_Jake"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	_vector		vDir = vPlayerPos - vMyPos; // �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ���� (Jaek <- Finn)

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (3.f < fDistanceX)
	{
		m_dNotfollow_TimeAcc += TimeDelta;
		if (5 < m_dNotfollow_TimeAcc) // ������� �� �ϴ� �ð��� 5 �ʸ� �Ѿ�ٸ�
		{
			_vector		vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float fLookX = XMVectorGetX(vMyLook);
			_float fLookZ = XMVectorGetZ(vMyLook);

			_float fAddX, fAddZ;
			if (0 < fLookX)		// +
				fAddX = 1.2f;
			else				// -
				fAddX = -1.2f;

			if (0 < fLookZ)
				fAddZ = 1.2f;
			else
				fAddZ = -1.2f;

			_float4 f4MyPos;
			XMStoreFloat4(&f4MyPos, vMyPos);
			pJakeTransformCom->Set_Pos(_float3(f4MyPos.x - fAddX, f4MyPos.y, f4MyPos.z - fAddZ));	// �� ������ �ű�

			m_dNotfollow_TimeAcc = 0;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Key_Input(_double TimeDelta)
{
	if (m_OnMove)
		m_pTransformCom->Go_Straight(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
	}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(270.f));

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	}

	if (pGameInstance->Key_Up(DIK_UP) || pGameInstance->Key_Up(DIK_RIGHT) || pGameInstance->Key_Up(DIK_DOWN) || pGameInstance->Key_Up(DIK_LEFT))
		m_OnMove = false;

	RELEASE_INSTANCE(CGameInstance);
}

CFinn * CFinn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFinn*		pInstance = new CFinn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFinn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFinn::Clone(void * pArg)
{
	CFinn*		pInstance = new CFinn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFinn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFinn::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
