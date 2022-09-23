#pragma once
#include "Engine_Include.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;

END

class CBackGround : public CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBackGround();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Obejct(void) override;

private:
	HRESULT		Add_Component(void);

private:
	CRcTex*			m_pBufferCom = nullptr;		// �ؽ��ĸ� ����ϱ� ���� ����
	CTexture*		m_pTextureCom = nullptr;	// ����� �ؽ���

public:
	static CBackGround*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);

};

