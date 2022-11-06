#include "stdafx.h"
#include "CollisionMgr.h"

CCollisionMgr::CCollisionMgr()
{
}


CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::Collision_UI(list<CObj*> _Dest)
{
	for (auto& Dest : _Dest)
	{
		if (Check_Rect_Mouse(Dest))
		{
			Dest->OnCollision(nullptr);
		}
	}
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

bool CCollisionMgr::Check_Rect_Mouse(CObj* pDest)
{
	D3DXVECTOR3* vUI_Pos = pDest->Get_Point();
	D3DXVECTOR3 vMouse_Pos = ::Get_Mouse();

	bool		bCheck[4]{ false };

	if (vUI_Pos[0].x < vMouse_Pos.x && vUI_Pos[0].y < vMouse_Pos.y)
		bCheck[0] = true;

	if (vUI_Pos[1].x > vMouse_Pos.x && vUI_Pos[1].y < vMouse_Pos.y)
		bCheck[1] = true;

	if (vUI_Pos[2].x > vMouse_Pos.x && vUI_Pos[2].y > vMouse_Pos.y)
		bCheck[2] = true;

	if (vUI_Pos[3].x < vMouse_Pos.x && vUI_Pos[3].y > vMouse_Pos.y)
		bCheck[3] = true;

	return bCheck[0] && bCheck[1] && bCheck[2] && bCheck[3];
} 