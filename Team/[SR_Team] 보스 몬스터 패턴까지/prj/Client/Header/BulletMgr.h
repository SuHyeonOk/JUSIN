#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CLayer;
END

class CBulletMgr
{
	DECLARE_SINGLETON(CBulletMgr)

private:
	CBulletMgr();
	~CBulletMgr();

public:
	HRESULT			Ready_Proto(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT			Ready_Clone(CLayer* pLayer, LPDIRECT3DDEVICE9 pGraphicDev);
	void			Collect_Obj();		// 사용을 마친 오브젝트를 회수하는 함수
	CGameObject*	Reuse_Obj(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);	// 현재 풀에 있는 오브젝트를 재사용하는 함수
	void			Fire(BULLETID _eID);


private:
	//vector<wchar_t*>		m_vecObjTags[BULLET_END];
	vector<wstring*>		m_vecObjTags[BULLET_END];
	vector<CGameObject*>	m_vecObjPool[BULLET_END];
	int				m_CurIdx[BULLET_END];	// 현재 인덱스
	int				m_MaxIdx[BULLET_END];	// 최대 인덱스(id의 최대객체개수)
};

