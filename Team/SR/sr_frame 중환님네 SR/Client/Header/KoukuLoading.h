#pragma once

#include "Engine_Include.h"
#include "Scene.h"

class CUI;
class CKoukuLoading : public Engine::CScene
{
private:
	explicit CKoukuLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKoukuLoading();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;


public:
	static CKoukuLoading*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
	CUI* m_pImage = nullptr;

};

