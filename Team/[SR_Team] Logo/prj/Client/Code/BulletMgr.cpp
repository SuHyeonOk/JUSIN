#include "stdafx.h"
#include "..\Header\BulletMgr.h"
//#include "Bullet.h"
#include "WandBullet.h"
#include "Export_Function.h"
#include <string>

IMPLEMENT_SINGLETON(CBulletMgr)

CBulletMgr::CBulletMgr()
{
	for (int i = 0; i < BULLET_END; ++i)
	{
		m_CurIdx[i] = 0;
	}

	m_MaxIdx[BULLET_WAND] = 20;
}


CBulletMgr::~CBulletMgr()
{
	for (int i = 0; i < BULLET_END; ++i)
	{
		for (auto& tag : m_vecObjTags[i])
		{
			delete [] tag;
			tag = nullptr;
		}
		m_vecObjTags[i].swap(vector<wstring*>());

		m_vecObjPool[i].swap(vector<CGameObject*>());
	}

}

void CBulletMgr::Ready_Proto()
{
	// Loading ¾À¿¡ ³ÖÀ» bullet ¿øº» ÄÄÆ÷³ÍÆ®
}

HRESULT CBulletMgr::Ready_Clone(CLayer* pLayer, LPDIRECT3DDEVICE9 pGraphicDev)
{
	//pGraphicDev->AddRef();
	CGameObject*		pGameObject = nullptr;
	
	// wandbullet
	wstring*	objTags = new wstring[m_MaxIdx[BULLET_WAND]];
	m_vecObjTags[BULLET_WAND].push_back(objTags);

	//wchar_t*	obgTags = new wchar_t[20];

	for (int i = 0; i < m_MaxIdx[BULLET_WAND]; ++i)
	{
		pGameObject = CWandBullet::Create(pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);

		objTags[i]  = L"WandBullet";
		//wstring objTag(L"WandBullet");
		wchar_t index[10];
		_itow_s(i, index, 10);
		objTags[i] += index;
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(objTags[i].c_str(), pGameObject), E_FAIL);
	
		m_vecObjPool[BULLET_WAND].push_back(pGameObject);
	
	}

	//Safe_Release(pGameObject);
	return S_OK;
}


void CBulletMgr::Collect_Obj()
{

}

CGameObject * CBulletMgr::Reuse_Obj(const D3DXVECTOR3 & vPos, const D3DXVECTOR3 & vDir)
{

//	CGameObject*		pBullet = nullptr;
//
//	if (m_ObjectPool.empty())
//	{
//		pBullet = CBullet::Create(vPos, vDir);
//		++m_iCount;
//
//#ifdef _DEBUG
//		cout << "ÃÑ¾Ë °³¼ö : " << m_iCount << endl;
//#endif // _DEBUG
//
//	}
//
//	else
//	{
//		pBullet = m_ObjectPool.front();
//		m_ObjectPool.pop_front();
//	}
//
//	dynamic_cast<CBullet*>(pBullet)->Set_Pos(vPos);
//	dynamic_cast<CBullet*>(pBullet)->Set_Dir(vDir);
//
//	return pBullet;
	return nullptr;
}

void CBulletMgr::Fire(BULLETID _eID)
{

	static_cast<CBullet*>(m_vecObjPool[_eID][m_CurIdx[_eID]])->Set_Fire(true);

	m_CurIdx[_eID] = (m_CurIdx[_eID] + 1) % m_MaxIdx[_eID];
}
