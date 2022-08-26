#pragma once

#include "Include.h"

class CTool131View;
class CMyTerrain
{
public:
	CMyTerrain();
	~CMyTerrain();

public:
	vector<TILE*>&	Get_Tile(void) { return m_vecTile; }

	bool			Picking(const D3DXVECTOR3& vPos, const int& iIndex);
	bool			Picking_Dot(const D3DXVECTOR3& vPos, const int& iIndex);
	int				Get_TileIndex(const D3DXVECTOR3& vPos);

	void		Tile_Change(const D3DXVECTOR3& vPos, const BYTE& _byDrawID);
	void		Set_MainView(CTool131View* pMainView) { m_pMainView = pMainView; }

public:
	void		Initialize(void);
	void		Update(void);
	void		Mini_Render(void);
	void		Render(void);
	void		Release(void);

private:
	void		Set_Ratio(D3DXMATRIX* pOut, float fRatioX, float fRatioY);

private:
	CTool131View*	m_pMainView = nullptr;
	vector<TILE*>	m_vecTile;

};

