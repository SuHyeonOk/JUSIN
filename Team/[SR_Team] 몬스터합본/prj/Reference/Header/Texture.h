#pragma once
#include "Component.h"
#include "Engine_Include.h"

BEGIN(Engine)

// �ؽ�ó�� ����(�뵵�� �ٸ���)
// 1. ȭ�� ��¿� 2. ť�� ��¿�
// �ؽ��Ĵ� �ϳ����� �ؽ����İ�ü �ϳ��� �ʿ��ϴ�.
// CTexture ������Ʈ�� �ؽ�ó �İ�ü�� �����ؼ� �����ϴ� �����̴�.
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
		// IDirect3DBaseTexture9 : ȭ�� ���/ ť����¿� �ؽ����� �θ� �ڷ���. ��, �Ѵ� ���� ����
public:
	static	CTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath, TEXTUREID eType, const _uint& iCnt = 1);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

private:
	_uint					m_iFrameEnd;

};

END
