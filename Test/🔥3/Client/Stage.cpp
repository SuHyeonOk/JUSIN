#include "stdafx.h"
#include "Stage.h"

#include "ObjMgr.h"
#include "Terrain.h"
#include "TextureMgr.h"
#include "Player.h"

#include "Monster.h"
#include "UI_Inventory.h"
#include "Mouse.h"


CStage::CStage()
{
}


CStage::~CStage()
{
	Release_Scene();
}

HRESULT CStage::Ready_Scene()
{
	if (FAILED(CTextureMgr::Get_Instance()->Read_ImgPath(L"../Data/ImgPath.txt")))
		return E_FAIL;
	
	CObj*	pObj = new CTerrain;

	if (FAILED(pObj->Initialize()))
		return E_FAIL;
	CObjMgr::Get_Instance()->Add_Object(TERRAIN, pObj);

	pObj = new CPlayer;

	if (FAILED(pObj->Initialize()))
		return E_FAIL;
	CObjMgr::Get_Instance()->Add_Object(PLAYER, pObj);

	pObj = new CMonster;

	if (FAILED(pObj->Initialize()))
		return E_FAIL;
	CObjMgr::Get_Instance()->Add_Object(MONSTER, pObj);

	pObj = new CUI_Inventory;

	if (FAILED(pObj->Initialize()))
		return E_FAIL;
	CObjMgr::Get_Instance()->Add_Object(UI, pObj);

	pObj = new CMouse;

	if (FAILED(pObj->Initialize()))
		return E_FAIL;
	CObjMgr::Get_Instance()->Add_Object(MOUSE, pObj);









	return S_OK;
}

void CStage::Update_Scene()
{
	CObjMgr::Get_Instance()->Update();
}

void CStage::Late_Update_Scene()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CStage::Render_Scene()
{
	CObjMgr::Get_Instance()->Render();
}

void CStage::Release_Scene()
{
	
}

CStage* CStage::Create(void)
{
	CStage*	pScene = new CStage;

	if (FAILED(pScene->Ready_Scene()))
		return nullptr;

	return pScene;
}
