#pragma once
#include "Include.h"

class CAstarMgr
{
	DECLARE_SINGLETON(CAstarMgr)

private:
	CAstarMgr();
	~CAstarMgr();

public:
	list<TILE*>&		Get_BestList(void) { return m_BestList; }

public:
	void	Astar_Start(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vGoal);
	void	Release();

private:
	bool	Make_Route(int iStartIdx, int iGoalIdx);
	void	Make_BestList(int iStartIdx, int iGoalIdx);

	bool	Picking(const D3DXVECTOR3& vPos, const int& iIndex);
	int		GetTileIndex(const D3DXVECTOR3& vPos);

private:
	bool	Check_Close(int _iIndex);
	bool	Check_Open(int _iIndex);

private:
	list<int>			m_CloseList;
	list<int>			m_OpenList;
	list<TILE*>			m_BestList;

	int					m_iStartIdx;
};

