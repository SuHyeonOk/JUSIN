#include "stdafx.h"
#include "AstarMgr.h"
#include "ObjMgr.h"
#include "Terrain.h"

IMPLEMENT_SINGLETON(CAstarMgr)

CAstarMgr::CAstarMgr()
	: m_iStartIdx(0)
{
}


CAstarMgr::~CAstarMgr()
{
	Release();
}
bool CAstarMgr::Make_Route(int iStartIdx, int iGoalIdx)
{
	CObj*		pTerrain = CObjMgr::Get_Instance()->Get_Terrain();
	if (nullptr == pTerrain)
		return false;

	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->Get_VecTile();
	if (vecTile.empty())
		return false;

	vector<list<TILE*>>& vecAdj = dynamic_cast<CTerrain*>(pTerrain)->Get_VecAdj();
	if (vecAdj.empty())
		return false;

	//너비 우선 탐색의 특성상 가장 앞에 있는 원소부터 지운다.
	if (!m_OpenList.empty())
		m_OpenList.pop_front();

	m_CloseList.push_back(iStartIdx);

	for (auto& pTile : vecAdj[iStartIdx])
	{
		if (iGoalIdx == pTile->iIndex)
		{
			pTile->iParentIndex = iStartIdx;

			return true;
		}

		if(false == Check_Close(pTile->iIndex) &&
			false == Check_Open(pTile->iIndex))
		{
			pTile->iParentIndex = iStartIdx;
			m_OpenList.push_back(pTile->iIndex);
		}
	}

	if (m_OpenList.empty())
		return false;

	int	iOriginIdx = m_iStartIdx;

	m_OpenList.sort([&vecTile, &iGoalIdx, &iOriginIdx](int iPre, int iCur)->bool
	{
		D3DXVECTOR3	vPCost1 = vecTile[iOriginIdx]->vPos - vecTile[iPre]->vPos;
		D3DXVECTOR3	vPCost2 = vecTile[iOriginIdx]->vPos - vecTile[iCur]->vPos;

		D3DXVECTOR3	vGCost1 = vecTile[iGoalIdx]->vPos - vecTile[iPre]->vPos;
		D3DXVECTOR3	vGCost2 = vecTile[iGoalIdx]->vPos - vecTile[iCur]->vPos;

		float	fCost1 = D3DXVec3Length(&vPCost1) + D3DXVec3Length(&vGCost1);
		float	fCost2 = D3DXVec3Length(&vPCost2) + D3DXVec3Length(&vGCost2);

		return fCost1 < fCost2;
	});
	
	return Make_Route(m_OpenList.front(), iGoalIdx);
}

void CAstarMgr::Make_BestList(int iStartIdx, int iGoalIdx)
{
	CObj*		pTerrain = CObjMgr::Get_Instance()->Get_Terrain();
	if (nullptr == pTerrain)
		return ;

	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->Get_VecTile();
	if (vecTile.empty())
		return ;

	m_BestList.push_front(vecTile[iGoalIdx]);

	int	iRouteIdx = vecTile[iGoalIdx]->iParentIndex;

	while (true)
	{
		if (iRouteIdx == iStartIdx)
			break;

		m_BestList.push_front(vecTile[iRouteIdx]);
		iRouteIdx = vecTile[iRouteIdx]->iParentIndex;
	}


}
void CAstarMgr::Astar_Start(const D3DXVECTOR3 & vStart, const D3DXVECTOR3 & vGoal)
{
	Release();

	CObj*		pTerrain = CObjMgr::Get_Instance()->Get_Terrain();
	if (nullptr == pTerrain)
		return;

	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->Get_VecTile();
	if (vecTile.empty())
		return;

	m_iStartIdx = GetTileIndex(vStart);

	int	iGoalIdx = GetTileIndex(vGoal);

	// 타일 인덱스 범위 안에 없다면
	if (0 > m_iStartIdx || 0 > iGoalIdx || TILEX * TILEY <= m_iStartIdx || TILEX * TILEY <= iGoalIdx)
		return;

	// 출발 지점과 목표 지점이 일치하는 경우
	if (m_iStartIdx == iGoalIdx)
		return;

	// 목표 지점으로 클릭한 곳이 장애물 타일인 경우
	if (0 != vecTile[iGoalIdx]->byOption)
		return;

	if (true == Make_Route(m_iStartIdx, iGoalIdx))
		Make_BestList(m_iStartIdx, iGoalIdx);
}

void CAstarMgr::Release()
{
	m_CloseList.clear();
	m_OpenList.clear();
	m_BestList.clear();
}



bool CAstarMgr::Picking(const D3DXVECTOR3 & vPos, const int & iIndex)
{
	CObj*		pTerrain = CObjMgr::Get_Instance()->Get_Terrain();
	if (nullptr == pTerrain)
		return false;

	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->Get_VecTile();
	if (vecTile.empty())
		return false;


	D3DXVECTOR3	vPoint[4]{

		{ vecTile[iIndex]->vPos.x, vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f },
		{ vecTile[iIndex]->vPos.x + (TILECX / 2.f), vecTile[iIndex]->vPos.y, 0.f },
		{ vecTile[iIndex]->vPos.x, vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ vecTile[iIndex]->vPos.x - (TILECX / 2.f), vecTile[iIndex]->vPos.y, 0.f }

	};

	D3DXVECTOR3	vDir[4]{

		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]
	};

	D3DXVECTOR3	vNormal[4]{

		D3DXVECTOR3(-vDir[0].y , vDir[0].x, 0.f),
		D3DXVECTOR3(-vDir[1].y , vDir[1].x, 0.f),
		D3DXVECTOR3(-vDir[2].y , vDir[2].x, 0.f),
		D3DXVECTOR3(-vDir[3].y , vDir[3].x, 0.f)
	};

	D3DXVECTOR3	vMouseDir[4]{

		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]
	};

	for (int i = 0; i < 4; ++i)
	{
		D3DXVec3Normalize(&vNormal[i], &vNormal[i]);
		D3DXVec3Normalize(&vMouseDir[i], &vMouseDir[i]);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (0.f < D3DXVec3Dot(&vNormal[i], &vMouseDir[i]))
			return false;
	}

	return true;
}

int CAstarMgr::GetTileIndex(const D3DXVECTOR3 & vPos)
{
	CObj*		pTerrain = CObjMgr::Get_Instance()->Get_Terrain();
	if (nullptr == pTerrain)
		return -1;

	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->Get_VecTile();
	if (vecTile.empty())
		return -1;

	for (size_t index = 0; index < vecTile.size(); ++index)
	{
		if (Picking(vPos, index))
		{
			return index;
		}
	}

	return -1;
}

bool CAstarMgr::Check_Close(int _iIndex)
{
	for (int& iCloseIdx : m_CloseList)
	{
		if (_iIndex == iCloseIdx)
			return true;
	}

	return false;
}

bool CAstarMgr::Check_Open(int _iIndex)
{
	for (int& iOpenIdx : m_OpenList)
	{
		if (_iIndex == iOpenIdx)
			return true;
	}

	return false;
}
