#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTexture;
class CCubeTex;
class CTransform;

END

class CCubePlayer : public Engine::CGameObject
{
private:
	explicit CCubePlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubePlayer();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	HRESULT				Add_Component();
	void				Key_Input(const _float& fTimeDelta);

	//template<typename T>
	//CGameObject*		Create_Bullet(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vDir);

private:
	CCubeTex*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

private:
	_vec3				m_vDirection;

	float				m_fSpeed;


public:
	static CCubePlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();
};

