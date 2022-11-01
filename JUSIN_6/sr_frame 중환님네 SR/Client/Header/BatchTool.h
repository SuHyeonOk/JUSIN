#pragma once
#include "Scene.h"

class CBatchTool : public CScene
{
private:
	explicit CBatchTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBatchTool() override;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;

	virtual void Free() override;
	static CBatchTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	static void Load(const wstring& wstrPath);
	void Save(const wstring& wstrPath);

	static std::vector<std::wstring> SplitWString(std::wstring str, _tchar splitter);


private:
	class CDynamicCamera* m_pCam = nullptr;
	CTransform* m_pTransform = nullptr;
	class CTerrainCubeMap* m_pMap = nullptr;

	_bool m_bPick = false;
};
