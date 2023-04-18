#pragma once
#include "Scene.h"
#include "SkeletalCube.h"

class CEditBox;
class CUI;

enum LogoState
{
	INPUT_NAME,
	MOVETO_PLAYER,
	SELECT_PLAYER,
	LOGO_STATE_END
};


enum CharacterSelect
{
	CHAR_STEVE,
	CHAR_ESHE,
	CHAR_COPPER,
	CHAR_PRIDE,
	CHAR_END
};

class CLogo : public CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;

public:
	static CLogo*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);

private:
	LogoState eState = INPUT_NAME;

	CUI* m_pLogo = nullptr;
	CEditBox* m_pEdit = nullptr;
	CUI* m_pButton = nullptr;
	CUI* m_pSelectButton = nullptr;

	_float m_fS = 0.f;
	_vec3 m_fLogoTo{};
	_vec3 m_fEditTo{};
	_vec3 m_fButtonTo{};

	CSkeletalCube* m_pCamMove = nullptr;
	_vec3 m_vCamSelectPos;


	_uint m_iCharNum = 0;
	CharacterSelect m_eChar = CHAR_STEVE;

	_float m_fDestYaw = 0.f;
	_float m_fStartYaw;
	_bool m_bMove = false;

	CubeAnimFrame m_Idle;
	CubeAnimFrame m_Jump;
	array<CSkeletalCube*, CHAR_END> m_arrChar{};
	_bool m_bSelected = false;
	wstring m_wstrCharName;
};

