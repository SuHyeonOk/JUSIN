#include "stdafx.h"
#include "..\Header\NetStage.h"

#include "AbstFactory.h"
#include "ArrowCubeMgr.h"
#include "CoolTimeUI.h"
#include "Particle.h"
#include "Player.h"
#include "StaticCamera.h"
#include "TerrainCubeMap.h"
#include "Terrain.h"
#include "DamageFontMgr.h"

CNetStage::CNetStage(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}

CNetStage::~CNetStage()
{
}

HRESULT CNetStage::Ready_Scene()
{
	if (FAILED(Engine::CScene::Ready_Scene()))
		return E_FAIL;

	CGameObject*		pGameObject = nullptr;
	CEnvFactory::Create<CStaticCamera>("StaticCamera", L"StaticCamera");
	CEnvFactory::Create<CTerrainWater>("WaterTerrain", L"WaterTerrain");

	// TerrainCubeMap
	pGameObject = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/Stage1.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);


	_matrix matWorld;
	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(90.f) ,0.f }, { 0.f, 0.f ,0.f });
	CPlayerFactory::Create<CPlayer>("Steve", L"Player", matWorld);

	CEffectFactory::Create<C3DBaseTexture>("3D_Base", L"3D_Base");
	CEffectFactory::Create<CAttack_P>("Attack_Basic", L"Attack_Basic");
	CEffectFactory::Create<CFireWork_Fuze>("FireWork_Fuze", L"FireWork_Fuze");
	CEffectFactory::Create<CFireWork>("FireWork", L"FireWork");
	CEffectFactory::Create<CSpeedBoots>("Speed_Boots", L"Speed_Boots");
	CEffectFactory::Create<CSpeedBoots_Particle>("Speed_Boots_Particle", L"Speed_Boots_Particle");


	CUIFactory::Create<CUI>("HPUI", L"HPUI", -1, WINCX/2, WINCY - 50, 100, 80);
	CUIFactory::Create<CCoolTimeUI>("PotionCoolTime", L"PotionCoolTime", -1, WINCX/2 + 90, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("RollCoolTime", L"RollCoolTime", -1, WINCX/2 + 140, WINCY - 30, 30, 30);
	CUIFactory::Create<CCoolTimeUI>("Legacy1CoolTime", L"Legacy1CoolTime", -1, WINCX/2 - 210, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("Legacy2CoolTime", L"Legacy2CoolTime", -1, WINCX/2 - 150, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("Legacy3CoolTime", L"Legacy3CoolTime", -1, WINCX/2 - 90, WINCY - 40, 50, 50);

	CUIFactory::Create<CCountUI>("ArrowUI", L"ArrowUI", -1, WINCX/2 + 190, WINCY - 30, 50, 50);
	CUIFactory::Create<CCountUI>("EmeraldUI", L"EmeraldUI", -1, WINCX/2 + 250, WINCY - 30, 20, 30);



	CClientServiceMgr::GetInstance()->ReadyClientService();

	return S_OK;
}

_int CNetStage::Update_Scene(const _float& fTimeDelta)
{
	CSoundMgr::GetInstance()->Update_Listener(LAYER_ENV, L"StaticCamera");
	CDamageFontMgr::GetInstance()->Update_DamageFontMgr(fTimeDelta);
	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CNetStage::LateUpdate_Scene()
{
	CScene::LateUpdate_Scene();
}

void CNetStage::Render_Scene()
{
	CDamageFontMgr::GetInstance()->Render_DamageFontMgr();
	CArrowCubeMgr::GetInst().Render_Buffer(); // todo : 렌더러에서 동작하게 바꾸기
}

CNetStage* CNetStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNetStage *	pInstance = new CNetStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNetStage::Free()
{
	CClientServiceMgr::DestroyInstance();
	CScene::Free();
}
