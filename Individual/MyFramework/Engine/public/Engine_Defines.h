#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <d3dcompiler.h>
#include <TypeInfo.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
using namespace std;


#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

////#ifdef _DEBUG
////#define USE_IMGUI
////#endif
//
////#if !defined(USE_IMGUI)
//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#ifndef DBG_NEW
//#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DBG_NEW
//#endif
//#endif // _DEBUG
////#endif

// �޸� ������ üũ �Ϸ��� #define USE_IMGUI ������ �߼�ó�� �ϱ�
//#ifdef _DEBUG
//#define USE_IMGUI
//#endif

// USE_IMGUI �� define �Ǿ� ���� ���� ������ �޸� ���� üũ
//#if !defined(USE_IMGUI)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif // _DEBUG
#endif // DBG_NEW
//#endif // !definde(USE_IMGUI)

using namespace Engine;
