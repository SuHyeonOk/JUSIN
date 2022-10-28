#pragma once
#include "Rune.h"
class CLaserShotRune : public CRune
{
private:
	explicit CLaserShotRune(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLaserShotRune() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;
	virtual void Collision() override;
	virtual void Use() override;

public:
	static CLaserShotRune* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void KillLaser() 
	{
		for (auto itr : Get_Layer(LAYER_EFFECT)->Get_MapObject())
		{
			if(CLazer* pLaser = dynamic_cast<CLazer*>(itr.second))
				pLaser->KillLaser();
		}
	}
private:
	_float m_fCurLaserTime = 0.f;
	_float m_fLaserTime = 0.2f;
};

