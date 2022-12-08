#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Obj_Manager.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
END

BEGIN(Client)

class CJake final : public CGameObject
{
private:
	CJake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJake(const CJake& rhs);
	virtual ~CJake() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void	Current_Player(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);
	void	Key_Input(_double TimeDelta);

	void	Space_Attack(_double TimeDelta);
	void	Roolling(_double TimeDelta);
	void	Stun();
	void	Change();

	void	Anim_Change(_double TimeDelta);

private:
	CObj_Manager::PLAYERINFO	m_tPlayerInfo;

	_bool		m_OnMove = false;			// Key_Input() : 키를 누르면 treu 됨

	_double		m_dNotfollow_TimeAcc = 0;	// Check_Follow() : Player 를 따라가지 못 하는 시간

	_bool		m_bHit = false;
	_double		m_bHit_TimeAcc = 0;			// 일정 시간 후 Hit 가 꺼짐

	_bool		m_bRoll = false;			// 앞 구르기 
	_bool		m_bStru = false;			// 스턴
	_uint		m_iStun_Count = 0;			// 스턴 애니메이션 두 번


	_bool		m_bSpace_Attack = false;

public:
	static CJake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END