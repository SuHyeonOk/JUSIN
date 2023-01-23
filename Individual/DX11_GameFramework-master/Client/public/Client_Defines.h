#pragma once

#include <Process.h>
#include <random>

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_SKELETON, LEVEL_SKELETON_BOSS, LEVEL_END };

	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };

	enum ITEMINDEX { ITEM_ONE, ITEM_TWO, ITEM_THREE, ITEM_FOUR, ITEMINDEX_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;

using namespace Client;

#define F2_SKELETON