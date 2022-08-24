#pragma once

#include "Obj.h"

class CCollisionMgr
{
public:
	CCollisionMgr();
	~CCollisionMgr();

public:
	static void	Collision_Rect(list<CObj*> _Dest, list<CObj*> _Sour);
	static void	Collision_Sphere(list<CObj*> _Dest, list<CObj*> _Sour);

	static void	Collision_RectEx(list<CObj*> _Dest, list<CObj*> _Sour);
	static bool Check_Rect(CObj* pDest, CObj* pSour, float* pX, float* pY);
	static bool Check_Rect(CObj* pDest, CObj* pSour);

	static bool Check_Sphere(CObj* pDest, CObj* pSour);

};

