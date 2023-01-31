#include "stdafx.h"
#include "..\public\UI_CutScene.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

#include "Camera_Dynamic.h"

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

	if (CObj_Manager::PLAYERINFO::PLAYER::CUTSCENE_ONE == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		CutSceneOne_Talk(TimeDelta);

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

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_A))
		--m_fX;
	if (pGameInstance->Key_Pressing(DIK_D))
		++m_fX;
	
	if (pGameInstance->Key_Down(DIK_R))
	{
		m_bSize_Change = false;
		m_Script_Count = 0;
		m_fSizeY = 720.f * 1.9f;
	}

	if (pGameInstance->Key_Down(DIK_E))
		++m_Script_Count;
	if (pGameInstance->Key_Down(DIK_W))
		m_Script_Count = 0;

	cout << "m_fX : " << m_fX << endl;



	// 대본

	if (true == m_bSize_Change)
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
	
	RELEASE_INSTANCE(CGameInstance);

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
		pCameraTransformCom->Set_State(CTransform::STATE_TRANSLATION, CObj_Manager::GetInstance()->Get_Player_Transform());	// 현재 플레이어의 좌표로 이동 시킨다.
		RELEASE_INSTANCE(CGameInstance);

		// 다음 사용을 위한 값 초기화
		m_Script_Count = 0;
		m_bSize_Change = false;
		m_fSizeY = 720.f * 1.9f;
		m_Script_TimeAcc = 0.0;

		return;
	}

	m_Script_TimeAcc += TimeDelta;
	if (3.0 < m_Script_TimeAcc)
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
