#pragma once
#include "UI.h"
class CKoukuHpUI :	public CUI
{
public:
	explicit CKoukuHpUI(LPDIRECT3DDEVICE9 m_pGraphicDev);
	explicit CKoukuHpUI(LPDIRECT3DDEVICE9 m_pGraphicDev, wstring wstrObj);
	virtual ~CKoukuHpUI();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CKoukuHpUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);

	//생성 할 때 사용
	void SetOwner(const wstring& wstrName, CGameObject* pOwner, _int iMaxHp)
	{
		m_pOwner = pOwner;
		m_wstrName = wstrName;
		m_iMaxHP = iMaxHp;
		m_fPersent = (_float)m_iMaxHP * 0.1f;

	}

	//업데이트에 사용
	void SetCurHp(_int iHp) { m_iCurHp = iHp; }

	//죽기 전에 사용
	void KillHpbar() { m_bDead = true; }

	//디폴트 0.435f
	void SetNamePos(_float fPos) { m_fNamePosx = fPos; m_bNameSet = true; }

	void SetRender(_bool bRend) { m_bRend = bRend; }
private:
	array<CRcBossHPTex*, 10> m_pHeart{};

	_int m_iProgress = 0;
	_int m_iMaxHP = 0;

	_int m_iCurHp = 0;
	_float m_fPersent = 0;
	_float m_fNamePosx = 0.435f;
	_bool m_bDead = false;
	CGameObject* m_pOwner = nullptr;
	wstring m_wstrName = L"";
	wstring m_strLine = L"X ";
	wstring m_strNum = L"";

	_bool m_bNameSet = false;
	_bool m_bRend = true;
};

