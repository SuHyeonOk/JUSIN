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
	void			Collect_Obj();		// ����� ��ģ ������Ʈ�� ȸ���ϴ� �Լ�
	CGameObject*	Reuse_Obj(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);	// ���� Ǯ�� �ִ� ������Ʈ�� �����ϴ� �Լ�
	void			Fire(BULLETID _eID);


private:
	//vector<wchar_t*>		m_vecObjTags[BULLET_END];
	vector<wstring*>		m_vecObjTags[BULLET_END];
	vector<CGameObject*>	m_vecObjPool[BULLET_END];
	int				m_CurIdx[BULLET_END];	// ���� �ε���
	int				m_MaxIdx[BULLET_END];	// �ִ� �ε���(id�� �ִ밴ü����)
};

