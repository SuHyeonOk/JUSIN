#pragma once
#include "GameObject.h"
#include "SkeletalCube.h"


class CBox : public CSkeletalCube
{
private:

	explicit CBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBox();

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Free() override;

	static CBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void	BoxOpen();

private:
	CubeAnimFrame m_AnimClosen;
	CubeAnimFrame m_AnimPlay;
	CubeAnimFrame m_AnimOpened;

	virtual void AnimationEvent(const string& strEvent);

	_bool m_bOpened = false;

private:
	//template<class T>
	void InitFunction();
	std::function<void(const _vec3)> m_fCreateItem[4];
};
