#include "stdafx.h"
#include "..\public\UI_CutScene.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

#include "M_Gary_Boss.h"	// 몬스터에게 컷 씬이 끝났음을 알린다.

CUI_CutScene::CUI_CutScene(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_CutScene::CUI_CutScene(const CUI_CutScene & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_CutScene::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CUI_CutScene::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 1280.f;
	m_fSizeY = 720.f * 1.9f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_CutScene::Tick(_double TimeDelta)
{
	TalkTexture_Tick(TimeDelta);

	switch (CObj_Manager::GetInstance()->Get_Current_Level())
	{
	case LEVEL_GAMEPLAY:
	{
		CutSceneOne_Talk(TimeDelta);

		if (false == m_bSound)
		{
			m_bSound = true;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Stop_Sound(0);
			pGameInstance->Play_Sound(TEXT("Intro2_Loop.ogg"), 0.7f, true, 3);
			RELEASE_INSTANCE(CGameInstance);
		}
	}
		break;

	case LEVEL_SKELETON_BOSS:
	{
		CutSceneTwo_Talk(TimeDelta);

		if (false == m_bSound)
		{
			m_bSound = true;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Stop_Sound(0);
			pGameInstance->Play_Sound(TEXT("sfx_scream.ogg"), 0.7f, true, 3);
			RELEASE_INSTANCE(CGameInstance);
		}
	}
		break;
	}
}

void CUI_CutScene::Late_Tick(_double TimeDelta)
{

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_CutScene::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (false == m_bSize_Change)	// 검정색 이 다 내려오기 전 까지는 리턴 시킨다.
		return S_OK;

	// 대본
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	switch (CObj_Manager::GetInstance()->Get_Current_Level())
	{
	case LEVEL_GAMEPLAY:
	{
		switch (m_Script_Count)
		{
		case 0:
			pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("핀!!! 어디 있다가 이제 오는거야"), _float2(494.0f, g_iWinSizeY * 0.92f), 0.f, _float2(0.5f, 0.47f));
			break;

		case 1:
			pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("저기 할머니가 자꾸 날 깔아 뭉개려고해서 여기까지 도망쳤어"), _float2(381.0f, g_iWinSizeY * 0.92f), 0.f, _float2(0.5f, 0.47f));
			break;

		case 2:
			pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("핀 얼른 나좀 구하러 와줘!"), _float2(508.0f, g_iWinSizeY * 0.92f), 0.f, _float2(0.5f, 0.47f));
			break;
		}
	}
	break;

	case LEVEL_SKELETON_BOSS:
	{
		switch (m_Script_Count)
		{
		case 0:
			pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("음 하하하! 날 찾으러 여기까지 왔다니 대단하지만"), _float2(430.0f, g_iWinSizeY * 0.92f), 0.f, _float2(0.5f, 0.47f));
			break;

		case 1:
			pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("너희들은 여기까지다."), _float2(570.0f, g_iWinSizeY * 0.92f), 0.f, _float2(0.5f, 0.47f));
			break;

		case 2:
			pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("어디 한 번 즐겨볼까!!!"), _float2(558.0f, g_iWinSizeY * 0.92f), 0.f, _float2(0.5f, 0.47f));
			break;
		}
		break;
		}
	}

	return S_OK;
}

HRESULT CUI_CutScene::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CutScene"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CutScene::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

void CUI_CutScene::TalkTexture_Tick(const _double & TimeDelta)
{
	if (true == m_bSize_Change)
		return;

	if (720.0f < m_fSizeY)
		m_fSizeY -= _float(TimeDelta) * 100.0f;
	else
		m_bSize_Change = true;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
}

void CUI_CutScene::CutSceneOne_Talk(const _double & TimeDelta)
{
	if (false == m_bSize_Change)
		return;

	if (2 < m_Script_Count)
	{
		// 카메라 원래대로 되돌리기
		CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);						// 플레이어 보이도록 수정
		CObj_Manager::GetInstance()->Set_Camera(CObj_Manager::PLAYERINFO::PLAYER::FINN);	// 현재 플레이어를 핀으로 변경

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);					// 카메라의 좌표를 플레이어로 변경
		CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), m_pTransformComTag, 0));
		pCameraTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-36.4833f, 3.69427f, 36.0742f, 1.0f));	// 현재 플레이어의 좌표로 이동 시킨다.

		// 다음 사용을 위한 값 초기화
		m_bSound = false;
		m_Script_Count = 0;
		m_bSize_Change = false;
		m_fSizeY = 720.f * 1.3f;	// Next CutScene Ready
		m_Script_TimeAcc = 0.0;

		pGameInstance->Stop_Sound(3);
		pGameInstance->Play_Sound(TEXT("Garden2_Loop.ogg"), 0.1f, true, 0);
		RELEASE_INSTANCE(CGameInstance);

		return;
	}

	m_Script_TimeAcc += TimeDelta;
	if (2.5 < m_Script_TimeAcc)
	{
		++m_Script_Count;
		m_Script_TimeAcc = 0.0;
	}
}

void CUI_CutScene::CutSceneTwo_Talk(const _double & TimeDelta)
{
	if (false == m_bSize_Change)
		return;

	if (2 < m_Script_Count)
	{
		// 카메라 원래대로 되돌리기
		CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);						// 플레이어 보이도록 수정
		CObj_Manager::GetInstance()->Set_Camera(CObj_Manager::PLAYERINFO::PLAYER::FINN);	// 현재 플레이어를 핀으로 변경

		// 다음 사용을 위한 값 초기화
		m_bSound = false;
		m_Script_Count = 0;
		m_bSize_Change = false;
		m_fSizeY = 720.f * 1.3f;	// Next CutScene Ready
		m_Script_TimeAcc = 0.0;

		// 컷씬이 끝났음을 알려준다.
		CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);
		CM_Gary_Boss * pGameObject = dynamic_cast<CM_Gary_Boss*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_Gary_Boss"), TEXT("Prototype_GameObject_Boss_S_Cage"), 0));
		pGameObject->Set_CutScene();

		pGameInstance->Stop_Sound(3);
		pGameInstance->Play_Sound(TEXT("Boss1_Loop.ogg"), 0.1f, true, 0);
		RELEASE_INSTANCE(CGameInstance);

		return;
	}

	m_Script_TimeAcc += TimeDelta;
	if (2.5 < m_Script_TimeAcc)
	{
		++m_Script_Count;
		m_Script_TimeAcc = 0.0;
	}
}

CUI_CutScene * CUI_CutScene::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CutScene*		pInstance = new CUI_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_CutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CutScene::Clone(void * pArg)
{
	CUI_CutScene*		pInstance = new CUI_CutScene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_CutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CutScene::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
