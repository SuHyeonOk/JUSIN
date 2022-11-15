#pragma once
#include "SkeletalCube.h"

class CCat_Idle : public CSkeletalCube
{
private:
	explicit CCat_Idle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCat_Idle(const CCat_Idle& rhs);
	virtual ~CCat_Idle() override;

public:
	virtual HRESULT	Ready_Object(const wstring& wstrPath);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object() override;
	virtual void	Render_Object()override;
	virtual void	AnimationEvent(const string& strEnvet) override;
	static CCat_Idle* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);
	


private:
	CubeAnimFrame m_iIdle;
	// CubeAnimFrame m_iMove;
	DWORD m_dwAnim;

	_bool	m_bCanPlayAnim = true;
	_bool	m_bMove = false;

};

