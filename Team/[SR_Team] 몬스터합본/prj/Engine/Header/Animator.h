#pragma once
#include "Component.h"
#include "Texture.h"
#include "GameObject.h"

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
	HRESULT		Ready_Animator();

	HRESULT		Add_Com(const _tchar * pTectureTag, COMPONENTID eID);
	void		Play_Animation(const _float& fTimeDelta);
	void		Change_Animation(const _tchar * pTectureTag, COMPONENTID eID);
	void		Set_Text();

	_float		Get_Frame() { return m_fFrame; }
	 
private: // Key : 오브젝트 이름, Velaue : 어떤 이미지
	map<const _tchar*, CTexture*>		m_mapAnimation;

public:
	static	CAnimator*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

private:
	CTexture*				m_pTextureCom = nullptr;

private:
	_float					m_fFrame;
};

END