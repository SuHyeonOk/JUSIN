#pragma once
#include "GameObject.h"

class CMapTable :
	public CGameObject
{
public:
	explicit CMapTable(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapTable(const CMapTable& rhs);
	virtual ~CMapTable();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CMapTable* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CTexture*	m_pTextureCom = nullptr;
	CCubeTex*	m_pBufferCom = nullptr;
	CTransform*	m_pTransCom = nullptr;

	_bool			m_bStart = false;
	_vec3			m_vDir;
	_bool			m_bReverse = false;

	_vec3 m_vPlayerPos;
	_float m_fTriggerFreq = 0.3f;
	_float m_fCurTriggerFreq = 0.3f;
};

