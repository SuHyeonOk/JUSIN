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

class CFinn final : public CGameObject
{
private:
	CFinn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFinn(const CFinn& rhs);
	virtual ~CFinn() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Parts();	// 나 에서 생성할 객체들

	void	Shader_Time(_double TimeDelta);

private:
	void	Player_Info();
	void	Sword_Tick(const _double & TimeDelta);
	void	Sword_LateTick(const _double & TimeDelta);

private:
	void	Player_Tick(_double TimeDelta);
	void	Current_Player(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);
	void	Key_Input(_double TimeDelta);

	void	Space_Attack_Tick(_double TimeDelta);
	void	Roolling_Tick(_double TimeDelta);
	void	Hit_Tick(_double TimeDelta);
	void	Stun_Tick();
	void	Change_Tick();
	void	Cheering_Tick();

	void	TreeWitch_Tick();

	void	Anim_Change(_double TimeDelta);

private:
	vector<CGameObject*>		m_PlayerParts;
	CObj_Manager::PLAYERINFO	m_tPlayerInfo;

	_bool		m_OnMove = false;			// Key_Input() : 키를 누르면 treu 됨

	// 셰이더
	_bool		m_bHit = false;
	_double		m_bHit_TimeAcc = 0;			// 일정 시간 후 Hit 가 꺼짐

	//////////////////////////// 플레이어 일 때
	_double		m_dNotfollow_TimeAcc = 0;	// Check_Follow() : Player 를 따라가지 못 하는 시간

	_double		m_dHit_TimeAcc = 0;

	_bool		m_bStru = false;			// 스턴
	_uint		m_iStun_Count = 0;			// 스턴 애니메이션 두 번


	_bool		m_bSpace_Attack_2 = false;
	_bool		m_bSpace_Attack_3 = false;
	_uint		m_iSpace_Attack_Count = 0;

	// Anim Test
	_int		m_AnimiNum = 0;
	_int		m_i = 0;
	//////////////////////////// 플레이어가 아닐 때

public:
	static CFinn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END