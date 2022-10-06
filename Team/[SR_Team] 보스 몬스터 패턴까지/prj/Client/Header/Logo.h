#pragma once

#include "Engine_Include.h"
#include "Scene.h"

#include "Loading.h"

class CLogo : public CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;

private:
	// Environment, GameObj, UI�� ������ �����Ѵٰ� �߾���.
	// Logo�� ��� Env�� �ʿ��Ͽ� �������� �����ϰ� ó����.
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag) { return S_OK; }

	HRESULT			Ready_Proto(void); // ������Ʈ�� ������ ������ ��

private:
	CLoading*		m_pLoading = nullptr;

public:
	static CLogo*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	
private:
	virtual void Free(void);
};

