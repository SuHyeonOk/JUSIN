// #pragma once
//
// #include "Scene.h"
//
// class CDynamicCamera;
// class CTerrainCubeMap;
//
//
// class CUItool : public Engine::CScene
// {
// private:
// 	explicit CUItool(LPDIRECT3DDEVICE9 pGraphicDev);
// 	virtual ~CUItool();
//
// public:
// 	virtual HRESULT	Ready_Scene(void) override;
// 	virtual _int	Update_Scene(const _float& fTimeDelta) override;
// 	virtual void	LateUpdate_Scene(void) override;
// 	virtual void	Render_Scene(void) override;
//
// private:
// 	HRESULT		Ready_Layer_Environment();
// 	HRESULT		Ready_Proto(void);
//
// public:
// 	static CUItool*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
//
// private:
// 	CLayer*		m_pLayer = nullptr;
// 	// UiTool		m_tUITool;
// 	CTerrainCubeMap*		m_pCubeMap = nullptr;
//
// 	CDynamicCamera*			m_pDCamera = nullptr;
//
// private:
// 	virtual void	Free(void);
// };
//
