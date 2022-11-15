#pragma once
#include "UI.h"

enum class MapUITexNum : _int
{
	EnterIcon = 32,
	EnterIconHover = 31,
	CloseIcon = 34,
	CloseHover,
	CloseActive,
	koukuEnter,
	kouku,
	OnlineModeIcon = 40,
	OnlineModeIconHover,
	End
};

enum class MapUIWorld : _int
{
	Creeper,
	CreeperHover,
	Kouku,
	KoukuHover,
	KoukuWindow,
	KoukuWindowEnter,
	Close,
	OnlineMode,
	End
};

enum class MapUIRend : _int
{
	CreeperHover,
	KoukuHover,
	CloseHover,
	CloseActive,
	KoukuWindow,
	OnlineHover,
	End
};


class CMapUI :
	public CUI
{
public:
	explicit CMapUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMapUI();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CMapUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);

	void Close() { m_bClose = true; }
	void Open() { m_bClose = false; }
	_bool IsClosen() { return m_bClose; }
	void SetUITexture(_int TexIndex) { m_iTexNum = TexIndex; }

private:
	void MouseEvent(_matrix& matWorld);
	_bool m_bClose = true;
	wstring m_wstrCount = L"";
	_bool m_bWorldSet = false;

	array <_matrix, (size_t)MapUIWorld::End> m_matWorld{};
	array <_bool, (size_t)MapUIRend::End> m_bRend{};
};
