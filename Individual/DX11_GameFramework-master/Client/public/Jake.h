#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Obj_Manager.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
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
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Parts();	// �� ���� ������ ��ü��

private:
	void	Sword_Tick(const _double & TimeDelta);
	void	Sword_LateTick(const _double & TimeDelta);

private:
	void	Player_Tick(_double TimeDelta);
	void	Player_Skill_Tick(_double TimeDelta);
	void	Current_Player(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);
	void	Key_Input(_double TimeDelta);

	void	Space_Attack_Tick(_double TimeDelta);
	void	Attack_Paint_Tick(_double TimeDelta);

	void	Control_Tick(_double TimeDelta);
	void	Roolling_Tick(_double TimeDelta);
	void	Hit_Tick(_double TimeDelta);
	void	Stun_Tick();

	void	Swim_Tick(_double TimeDelta);
	void	Change_Tick();
	void	Cheering_Tick();

	HRESULT	Magic_Tick(_double TimeDelta);

	void	Anim_Change(_double TimeDelta);

private:
	vector<CGameObject*>		m_PlayerParts;
	CObj_Manager::PLAYERINFO	m_tPlayerInfo;

	_bool		m_OnMove = false;			// Key_Input() : Ű�� ������ treu ��

	_double		m_dNotfollow_TimeAcc = 0;	// Check_Follow() : Player �� ������ �� �ϴ� �ð�

	//////////////////////////// �÷��̾� �� ��
	_bool		m_bStru = false;			// ����
	_uint		m_iStun_Count = 0;			// ���� �ִϸ��̼� �� ��

	_bool		m_bDiving = false;			// ����
	_bool		m_bIsSwim = false;

	_bool		m_bSkill_Clone = false;			// ��ų�� �� ���� �����ϱ� ���ؼ�
	_double		m_bSkillClone_TimeAcc = 0;

	// Skill
	_bool		m_bSkill = false;
	_double		m_dSkill_TimeAcc = 0;

	_bool		m_bPaint = false;

	//////////////////////////// �÷��̾ �ƴ� ��

	int m_i = 0;
public:
	static CJake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END