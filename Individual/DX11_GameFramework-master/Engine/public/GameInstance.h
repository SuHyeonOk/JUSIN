#pragma once

/* �̱���.  */
/* Ŭ���̾�Ʈ�� ������ ������ ���� ������� Ŭ����. */

#include "Base.h"
#include "Input_Device.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Picking.h"
#include "Collider_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	static _uint Get_StaticLevelIndex() {
		return m_iStaticLevelIndex;
	}

	HWND GetHWND() { return m_hWnd; }

public: /* For.GameInstance */	
	static const _tchar*			m_pPrototypeTransformTag;


public: /* For.GameInstance */
	HRESULT		Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void		Tick_Engine(_double TimeDelta);
	void		Clear_Level(_uint iLevelIndex);

public: /* For.Graphic_Device */
	HRESULT		Clear_Graphic_Device(const _float4* pColor);
	HRESULT		Present();
	HRESULT		Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

public: /* For.Input_Device */
	_bool		Key_Down(_uchar eKeyID);
	_bool		Key_Up(_uchar eKeyID);
	_bool		Key_Pressing(_uchar eKeyID);

	_bool		Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseKeyID);
	_bool		Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseKeyID);
	_bool		Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseKeyID);

	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState);

public: /* For.Level_Manager */
	HRESULT		Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);	
	HRESULT		Render_Level();

public: /* For.Object_Manager */
	class CComponent*	Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex); // ��
	class CGameObject*	Get_GameObjectPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag, _uint iLayerIndex); // ��
	HRESULT				Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT				Clone_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject*		Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	
public: /* For.Component_Manager */
	HRESULT				Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent*	Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* For.PipeLine */
	_matrix			Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState) ;
	_float4x4		Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState) ;
	_matrix			Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState) ;
	void			Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4			Get_CamPosition();

public: /* For.Timer_Manager */ 
	_double			Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT			Ready_Timer(const _tchar* pTimerTag);
	void			Update_Timer(const _tchar* pTimerTag);

public: /* For.Light_Manager */ 
	const LIGHTDESC*	Get_LightDesc(_uint iIndex);
	HRESULT				Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public: // for imgui manager
	void		Render_ImGui();
	void		Render_Update_ImGui();
	void		Add_ImguiTabObject(class CImguiObject* ImguiObject);
	void		Add_ImguiWindowObject(class CImguiObject* ImguiObject);
	void		Clear_ImguiObjects();

public: // For.Picking
	_float4		Get_MousePos();

public: // For. Collider_Manager
	HRESULT Add_ColGroup(CCollider_Manager::COL_TYPE eColType, class CGameObject* pGameObject);
	void Update_Col(CCollider_Manager::COL_TYPE eColType_L, CCollider_Manager::COL_TYPE eColType_R);
	void Reset_Col();

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));

public: /* For.Frustum */
	_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);

private:
	static _uint					m_iStaticLevelIndex;

private:
	class CGraphic_Device*			m_pGraphic_Device = nullptr;
	class CInput_Device*			m_pInput_Device = nullptr;
	class CLevel_Manager*			m_pLevel_Manager = nullptr;
	class CObject_Manager*			m_pObject_Manager = nullptr;
	class CComponent_Manager*		m_pComponent_Manager = nullptr;
	class CPipeLine*				m_pPipeLine = nullptr;
	class CTimer_Manager*			m_pTimer_Manager = nullptr;
	class CLight_Manager*			m_pLight_Manager = nullptr;

	class CImgui_Manager*			m_pImgui_Manager = nullptr;
	class CPicking*					m_pPicking = nullptr;
	class CCollider_Manager*		m_pCollider_Manager = nullptr;
	class CFont_Manager*			m_pFont_Manager = nullptr;
	class CFrustum*					m_pFrustum = nullptr;

public:
	static void Release_Engine();

private:
	HWND m_hWnd;

public:
	virtual void Free() override;
};

END