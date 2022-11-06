#pragma once

#include "Base.h"
#include "Client_Defines.h"

#if defined(USE_IMGUI)

#include "../Default/ImGui/imgui.h"
#include "../Default/ImGui/imgui_impl_dx11.h"
#include "../Default/ImGui/imgui_impl_win32.h" // 32�ᵵ �Ǵ°ǰ�?

#include "GameInstance.h"

BEGIN(Client)

class CImGuiManager final : public CBase
{
	DECLARE_SINGLETON(CImGuiManager)
public:
	CImGuiManager();
	~CImGuiManager();
};

