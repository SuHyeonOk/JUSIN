#include "stdafx.h"
#include "CollisionMgr.h"


CCollisionMgr::CCollisionMgr()
{
}


CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::Collision_Rect(list<CObj*> _Dest, list<CObj*> _Sour)
{
	for (auto& Dest : _Dest) // 플레이어
	{
		for (auto& Sour : _Sour)
		{
			if (Check_Rect(Dest, Sour))
			{
				Dest->OnCollision(Sour);
				Sour->OnCollision(Dest);
			}
		}
	}
}

void CCollisionMgr::Collision_Sphere(list<CObj*> _Dest, list<CObj*> _Sour)
{

}
									 //   충돌하는 대상			장애물
void CCollisionMgr::Collision_RectEx(list<CObj*> _Dest, list<CObj*> _Sour)
{

}

bool CCollisionMgr::Check_Rect(CObj * pDest, CObj * pSour, float * pX, float * pY)
{


	return false;
}

bool CCollisionMgr::Check_Rect(CObj * pDest, CObj * pSour)
{
	if (fabs(pDest->Get_Info().vPos.x - pSour->Get_Info().vPos.x) >
		pDest->Get_Radius() + pSour->Get_Radius())
		return false;

	if (fabs(pDest->Get_Info().vPos.y - pSour->Get_Info().vPos.y) >
		pDest->Get_Radius() + pSour->Get_Radius())
		return false;

	return true;
}

bool CCollisionMgr::Check_Sphere(CObj* pDest, CObj* pSour)
{
	D3DXVECTOR3 vPos = pDest->Get_Info().vPos - pSour->Get_Info().vPos;

	float		fDiagonal = D3DXVec3Dot(&vPos, &vPos);

	float		fRadius = pDest->Get_Radius() + pSour->Get_Radius();

	return fDiagonal <= fRadius * fRadius;

	return 0;
}
