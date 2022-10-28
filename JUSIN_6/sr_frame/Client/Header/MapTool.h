#pragma once
class CDynamicCamera;
class CTerrainCubeMap;

class CMapTool : public Engine::CScene
{
private:
	explicit CMapTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMapTool();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;

private:
	HRESULT			Ready_Layer_Environment();
	HRESULT			Ready_Proto(void);
	void			Cube_DebugShow(void);


public:
	_bool			PickingOnCube(_vec3& CubeCenter, int& iToDelIdx);

public:
	static CMapTool*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static const _vec3 s_vFaceVtx[FACE_END][4];

private:
	CLayer*			m_pLayer = nullptr;
	MapTool			m_tMapTool;

	CTerrainCubeMap* m_pCubeMap = nullptr;
	//_float			m_fHeight[VTXCNTX][VTXCNTZ];

	CDynamicCamera* m_pDCamera = nullptr;
	_float			m_fFar = 50.f;

private:
	virtual void	Free(void);
};

