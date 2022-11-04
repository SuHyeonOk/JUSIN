#pragma once

#pragma warning (disable : 4005)

#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
using namespace DirectX;


#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


using namespace Engine;
