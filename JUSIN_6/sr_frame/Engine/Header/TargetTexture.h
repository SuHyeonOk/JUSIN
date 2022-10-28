#pragma once
#include "Component.h"
#include "GameObject.h"
#include "RcTex.h"
#include "Texture.h"

BEGIN(Engine)
class ENGINE_DLL CTargetTexture : public CComponent
{
private:
	explicit CTargetTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTargetTexture(const CTargetTexture& rhs);
	virtual ~CTargetTexture() override;

public:
	HRESULT		Ready_Texture();
	void		Set_Texture(const _uint& iIndex = 0);		// 텍스처 그리기 관련 함수
	void		Set_Texture(class CShader* pShaderCom, D3DXHANDLE hHandle, const _uint& iIndex = 0);		// 텍스처 그리기 관련 함수

	void AddRender(CGameObject* pObj) { m_RenderList.push_back(pObj);}
	void RenderOnSurface();
private:
	IDirect3DTexture9*		m_pTexture;
	ID3DXRenderToSurface* m_pRender2Surface;
	list<CGameObject*> m_RenderList;

	CTexture* m_pMvpTexture = nullptr;
	CRcTex* m_pBuff = nullptr;

public:
	static	CTargetTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone() override;
	virtual void Free() override;
};

END

