#include "stdafx.h"
#include "..\Header\Logo.h"
#include "AbstFactory.h"
#include "DynamicCamera.h"
#include "UI.h"
#include "EditBox.h"
#include "StaticCamera.h"
#include "TerrainCubeMap.h"
#include "ObjectStoreMgr.h"
#include "SkeletalCube.h"
#include "SkyBox.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(Engine::CScene::Ready_Scene()))
		return E_FAIL;



	CGameObject* pGameObject = nullptr;
	_matrix matWorld;

	m_vCamSelectPos = {2.5f, 3.f, 2.5f};
	m_Idle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_idle.anim");
	m_Jump = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/jump.anim");
	
	CGameUtilMgr::MatWorldComposeEuler(matWorld, {1.f, 1.f, 1.f}, {0.f, D3DXToRadian(0.f), 0.f}, {2.5f, 1.f, 4.7f});
	pGameObject = m_arrChar[CHAR_STEVE] = CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Steve.cube");
	m_arrLayer[LAYER_ENV]->Add_GameObject(L"Steve", pGameObject);
	pGameObject->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->Set_WorldDecompose(matWorld);
	dynamic_cast<CSkeletalCube*>(pGameObject)->PlayAnimationLoop(&m_Idle);
	dynamic_cast<CSkeletalCube*>(pGameObject)->Get_SkeletalPart("weapon_r")->pBuf = nullptr;

	CGameUtilMgr::MatWorldComposeEuler(matWorld, {1.f, 1.f, 1.f}, {0.f, D3DXToRadian(90.f), 0.f}, {4.7f, 1.f, 2.5f});
	pGameObject = m_arrChar[CHAR_ESHE] = CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Eshe.cube");
	m_arrLayer[LAYER_ENV]->Add_GameObject(L"Eshe", pGameObject);
	pGameObject->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->Set_WorldDecompose(matWorld);
	dynamic_cast<CSkeletalCube*>(pGameObject)->PlayAnimationLoop(&m_Idle);
	dynamic_cast<CSkeletalCube*>(pGameObject)->Get_SkeletalPart("weapon_r")->pBuf = nullptr;

	CGameUtilMgr::MatWorldComposeEuler(matWorld, {1.f, 1.f, 1.f}, {0.f, D3DXToRadian(180.f), 0.f}, {2.5f, 1.f, 0.3f});
	pGameObject = m_arrChar[CHAR_COPPER] = CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Copper.cube");
	m_arrLayer[LAYER_ENV]->Add_GameObject(L"Copper", pGameObject);
	pGameObject->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->Set_WorldDecompose(matWorld);
	dynamic_cast<CSkeletalCube*>(pGameObject)->PlayAnimationLoop(&m_Idle);
	dynamic_cast<CSkeletalCube*>(pGameObject)->Get_SkeletalPart("weapon_r")->pBuf = nullptr;

	CGameUtilMgr::MatWorldComposeEuler(matWorld, {1.f, 1.f, 1.f}, {0.f, D3DXToRadian(270.f), 0.f}, {0.3f, 1.f, 2.5f});
	pGameObject = m_arrChar[CHAR_PRIDE] = CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Pride.cube");
	m_arrLayer[LAYER_ENV]->Add_GameObject(L"Pride", pGameObject);
	pGameObject->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->Set_WorldDecompose(matWorld);
	dynamic_cast<CSkeletalCube*>(pGameObject)->PlayAnimationLoop(&m_Idle);
	dynamic_cast<CSkeletalCube*>(pGameObject)->Get_SkeletalPart("weapon_r")->pBuf = nullptr;


	m_pCamMove = CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/Object/logocam.cube");
	m_arrLayer[LAYER_ENV]->Add_GameObject(L"CamMove", m_pCamMove);
	CEnvFactory::Create<CSkyBox>("SkyBox", L"SkyBox")
		->SetTex(3);


	m_pCamMove->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS] = m_vCamSelectPos +
		_vec3{0.f, 10.f, 0.f};

	CStaticCamera* pcam = CEnvFactory::Create<CStaticCamera>("StaticCamera", L"StaticCamera");
	pcam->SetSkeletalFollowMode(m_pCamMove->Get_SkeletalPart("body"));


	// pGameObject = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f));
	// NULL_CHECK_RETURN(pGameObject, E_FAIL);
	// FAILED_CHECK_RETURN(m_arrLayer[LAYER_GAMEOBJ]->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);


	pGameObject = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/selectCharacter.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);

	m_pLogo = CUIFactory::Create<CUI>("DefaultUI", L"UI", 27, WINCX / 2, WINCY / 3.6f , WINCX * 0.8, WINCY * 0.3);
	m_pEdit = CUIFactory::Create<CEditBox>("EditBox", L"EditBox", 26, WINCX / 2, WINCY /1.44f, WINCX * 0.3, WINCY * 0.07f);
	m_pEdit->SetText(L"Gothic_Regular25", L"이름을 입력하세요.", _vec2{- 100.f, -60.f}, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_pButton = CUIFactory::Create<CUI>("DefaultUI", L"DefaultUI", 28, WINCX / 2, 600, 100, 50);
	m_pButton->SetText(L"Gothic_Bold30", L"확인", _vec2{-30.f, -15.f}, D3DCOLOR_ARGB(255, 255, 255, 255));
	m_pButton->SetButton([this]()
	{
		static _bool bDoor = true;
		if (bDoor == false) return;
		CObjectStoreMgr::GetInstance()->SetPlayerName(m_pEdit->GetInputs());

		eState = MOVETO_PLAYER;
		m_fLogoTo = _vec3{0.f, 1000.f, 0.f} + m_pLogo->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->
		                                               m_vInfo[INFO_POS];
		m_fEditTo = _vec3{0.f, 1000.f, 0.f} + m_pEdit->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->
		                                               m_vInfo[INFO_POS];
		m_fButtonTo = _vec3{0.f, 1000.f, 0.f} + m_pButton->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
		                                                 ->m_vInfo[INFO_POS];
		bDoor = false;
	});

	m_pSelectButton = CUIFactory::Create<CUI>("DefaultUI", L"DefaultUI", 30, WINCX / 2, WINCY /1.1338f, 400, 250);
	m_pSelectButton->SetButton([this]()
	{
		m_arrChar[m_iCharNum]->PlayAnimationOnce(&m_Jump, true);
		CObjectStoreMgr::GetInstance()->SetPlayerSkin((CharacterSelect)m_iCharNum);
		m_bSelected = true;
		m_pSelectButton->SetVisible(false);
	});
	m_pSelectButton->SetVisible(false);
	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type		= D3DLIGHT_POINT;
	tLightInfo.Diffuse	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Specular	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Ambient	= D3DXCOLOR(0.6f, 0.6f, 0.5f, 1.f);
	tLightInfo.Attenuation0 = 0.5f;
	tLightInfo.Range = 8.f;
	
	_matrix view;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
	D3DXMatrixInverse(&view, nullptr, &view);
	
	tLightInfo.Position = {view._41, view._42 + 3.f, view._43};
	m_pGraphicDev->SetLight(0, &tLightInfo);
	m_pGraphicDev->LightEnable(0, TRUE);


	switch (eState)
	{
	case INPUT_NAME:
		break;
	case MOVETO_PLAYER:
		{
			_vec3& vLogoPos = m_pLogo->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3& vEditPos = m_pEdit->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3& vButtonPos = m_pButton->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[
				INFO_POS];

			if (!CGameUtilMgr::Vec3Cmp(vLogoPos, m_fLogoTo))
			{
				D3DXVec3Lerp(&vLogoPos, &vLogoPos, &m_fLogoTo, 0.01f);
				D3DXVec3Lerp(&vEditPos, &vEditPos, &m_fEditTo, 0.01f);
				D3DXVec3Lerp(&vButtonPos, &vButtonPos, &m_fButtonTo, 0.01f);
			}

			_vec3& vCamPos = m_pCamMove->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[
				INFO_POS];
			if (!CGameUtilMgr::Vec3Cmp(m_vCamSelectPos, vCamPos, 0.01f))
			{
				D3DXVec3Lerp(&vCamPos, &vCamPos, &m_vCamSelectPos, 0.05f);
			}
			else
			{
				eState = SELECT_PLAYER;
				switch (static_cast<CharacterSelect>(m_iCharNum))
				{
				case CHAR_STEVE:
					m_wstrCharName = L"스티브";
					break;
				case CHAR_PRIDE:
					m_wstrCharName = L"프라이드";
					break;
				case CHAR_COPPER:
					m_wstrCharName = L"코퍼";
					break;
				case CHAR_ESHE:
					m_wstrCharName = L"애쉬";
					break;
				case CHAR_END:
					break;
				default: ;
				}
				_float fWidth = 40.f;
				_float fHalf = fWidth * _float(m_wstrCharName.size()) / 2;
				m_pSelectButton->SetText(L"Gothic_Bold40", m_wstrCharName, _vec2{-fHalf, -20.f} , D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}
		break;
	case SELECT_PLAYER:
		if (m_bMove)
		{
			m_fS += fTimeDelta * 1.2f;
			if (m_fS > 1.f)
				m_bMove = false;
			_float& fYaw = m_pCamMove->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vAngle.y;
			fYaw = CGameUtilMgr::FloatLerp(m_fStartYaw, m_fDestYaw, m_fS);
			m_pSelectButton->SetVisible(false);
		}
		else
		{
			if (m_bSelected == false)
			{
				m_pSelectButton->SetVisible(true);

				if (DIKeyDown(DIK_W))
				{
					m_fStartYaw = m_pCamMove->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vAngle.y;
					m_fDestYaw = m_fStartYaw + D3DXToRadian(90.f);
					m_bMove = true;
					m_fS = 0.f;

					++m_iCharNum;
					m_iCharNum = m_iCharNum % CHAR_END;

					switch (static_cast<CharacterSelect>(m_iCharNum))
					{
					case CHAR_STEVE:
						m_wstrCharName = L"스티브";
						break;
					case CHAR_PRIDE:
						m_wstrCharName = L"프라이드";
						break;
					case CHAR_COPPER:
						m_wstrCharName = L"코퍼";
						break;
					case CHAR_ESHE:
						m_wstrCharName = L"애쉬";
						break;
					case CHAR_END:
						break;
					default: ;
					}
					_float fWidth = 40.f;
					_float fHalf = fWidth * _float(m_wstrCharName.size()) / 2;
					m_pSelectButton->SetText(L"Gothic_Bold40", m_wstrCharName, _vec2{-fHalf, -20.f} , D3DCOLOR_ARGB(255, 255, 255, 255));
					m_pSelectButton->SetVisible(false);
				}
			}
			else
			{
				if (m_arrChar[m_iCharNum]->IsStop())
				{
					CSceneFactory::LoadScene("Loading1", "Stage_Start", true ,300);
				}
			}
		}
		break;
	case LOGO_STATE_END:
		break;
	default: ;
	}

	return CScene::Update_Scene(fTimeDelta);
}

void CLogo::LateUpdate_Scene()
{
	CScene::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	CScene::Render_Scene();

}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo::Free()
{
	m_pGraphicDev->LightEnable(0, FALSE);
	CScene::Free();
}
