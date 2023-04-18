#pragma once
#include "SkeletalCube.h"
class CCamAnimation : public CSkeletalCube
{
protected:
	explicit CCamAnimation(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCamAnimation(const CCamAnimation& rhs);
	virtual ~CCamAnimation();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void AnimationEvent(const string& strEvent) override;
	virtual void Render_Object() override;
	static CCamAnimation* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrAnimPath);
	virtual void Free() override;

	void GetCamWorld(_matrix& matWorld);
	_bool IsFinish() const { return m_bFinish; }

private:
	CubeAnimFrame m_AnimFrame;

	_bool m_bFinish = false;
	_bool m_bRender = false;
};

