#pragma once

#include "Base.h"
#include "Client_Defines.h"

#if defined(USE_IMGUI)

#include "../Default/ImGui/imgui.h"
#include "../Default/ImGui/imgui_impl_dx11.h"
#include "../Default/ImGui/imgui_impl_win32.h"

#include "GameInstance.h"

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public: /* MainApp 에서 돌아가야 할 기본적인 함수들 */
	void	Ready_Imgui();
	void	Tick_Imgui();
	void	Render_Imgui();

public:
	void	Monster_Tool();

private:
	class CMonster_Tool*	m_pMonsterTool = nullptr;

private:
	CGameInstance*			m_pGameInstance = nullptr;

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

public:
	virtual void Free() override;
};

#endif