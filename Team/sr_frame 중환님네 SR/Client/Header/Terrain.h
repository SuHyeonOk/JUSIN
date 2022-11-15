#pragma once
#include "GameObject.h"


class CTerrain : public CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT				Add_Component(void);

private:
	CTerrainTex*		m_pBufferCom = nullptr;

	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

public:
	static CTerrain*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);
};

class CTerrainWater : public CGameObject
{
private:
	explicit CTerrainWater(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrainWater();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;


private:
	// 버퍼 포인터 바꾸기
	CTerrainShader*		m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;





public:
	static CTerrainWater*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);

};
