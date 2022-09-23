#pragma once
#include "Component.h"
#include "Texture.h"

#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CAnimator : public CComponent
{
private:
	explicit CAnimator(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAnimator(const CAnimator& rhs);
	virtual ~CAnimator();

	// 애니메이션 만들기, 실행 시키기, 바꾸기

public:
	HRESULT		Ready_Animator(const _tchar* pPath, TEXTUREID eType, const _uint& iCnt);

	void		Play_Animation(const _float& fTimeDelta);


private:
	vector<IDirect3DBaseTexture9*>		m_vecTexture;

public:
	static	CAnimator*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath, TEXTUREID eType, const _uint& iCnt = 1);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

private:
	CTexture*				m_pTextureCom = nullptr;

private:
	_float					m_fFrame;
};

END