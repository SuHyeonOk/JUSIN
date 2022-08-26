#pragma once

#include "Include.h"

class CDevice
{
	DECLARE_SINGLETON(CDevice)

private:
	CDevice();
	~CDevice();

	// 장치(렌더링 담당 객체) 초기화 과정
	
	// 1. 렌더링 장치를 생성하고 조사할 객체 생성
	// 2. 장치 조사(지원 수준을 파악)
	// 3. 렌더링 장치 제어 객체 생성

public:
	LPDIRECT3DDEVICE9		Get_Device(void) { return m_pDevice; }
	LPD3DXSPRITE			Get_Sprite(void) { return m_pSprite; }
	LPD3DXFONT				Get_Font(void)   { return m_pFont; }

public:
	HRESULT				Initialize(void);
	void				Release(void);

	void				Render_Begin(void);
	void				Render_End(HWND hWnd = nullptr);
	
private:
	void				Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp);


private:
	LPDIRECT3D9			m_pSDK;		// 사용하고자 하는 장치를 조사하고 해당 장치를 제어하는 객체를 생성하는 상위 객체
	LPDIRECT3DDEVICE9	m_pDevice;	// 그래픽 장치를 통한 렌더링을 제어하는 객체

	LPD3DXSPRITE		m_pSprite;	// dx에서 2d 이미지 출력을 제어하는 객체
	LPD3DXFONT			m_pFont;
};

