#pragma once
#include "Component.h"
#include "Engine_Include.h"

BEGIN(Engine)

// 텍스처의 종류(용도가 다르다)
// 1. 화면 출력용 2. 큐브 출력용
// 텍스쳐는 하나마다 텍스쳐컴객체 하나가 필요하다.
// CTexture 컴포넌트는 텍스처 컴객체를 생성해서 보관하는 역할이다.
class ENGINE_DLL CTexture : public CComponent
{
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	HRESULT		Ready_Texture(const _tchar* pPath, TEXTUREID eType, const _uint& iCnt);
	void		Set_Texture(const _uint& iIndex = 0);

	_uint       Get_FrameEnd() { return m_iFrameEnd; }

private:
	vector<IDirect3DBaseTexture9*>	m_vecTexture;
		// IDirect3DBaseTexture9 : 화면 출력/ 큐브출력용 텍스쳐의 부모 자료형. 즉, 둘다 저장 가능
public:
	static	CTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath, TEXTUREID eType, const _uint& iCnt = 1);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

private:
	_uint					m_iFrameEnd;

};

END
