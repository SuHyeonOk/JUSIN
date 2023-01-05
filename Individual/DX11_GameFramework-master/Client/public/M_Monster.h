#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CM_Monster abstract : public CGameObject
{
public:
	typedef struct tagMonsterDesc : public CGameObject::GAMEOBJECTDESC
	{
		enum MONSTERKIND { 
			W_BEE, W_WORKE, 
			S_COWBOY, S_SR, 
			G_BLUE, G_RED, G_YELLOW,
			TREE_WITCH, MAGIC_MAN, MIMIC,
			GHOST_1, GHOST_2, GHOST_3,
			SKELETON_SHIELD_1, SKELETON_SHIELD_2,
			SKELETON_ARCHER_1, SKELETON_ARCHER_2,
			MONSTERKIND_END };

		MONSTERKIND		eMonsterKind = MONSTERKIND_END;
		_float3			f3Pos;

	}MONSTERDESC;

	typedef struct tagMonsterInfo
	{
		enum STATE { IDLE, MOVE, FIND, ATTACK, HIT, DIE, DANCE, ADD_1, STATE_END };

		STATE			eState			= STATE_END;

		_float	fHP			= 0.0f;
		_float	fMaxHP		= 0.0f;
		_int	iAttack		= 0;
		_int	iExp		= 0;

	}MONSTERINFO;

protected:
	CM_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Monster(const CM_Monster& rhs);
	virtual ~CM_Monster() = default;

public:
	_int		Get_Monster_Attack() { return m_tMonsterInfo.iAttack; }

public:		// �� �ʼ�
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(const _double& TimeDelta);
	virtual void		Late_Tick(const _double& TimeDelta);
	virtual HRESULT		Render();

	virtual void		On_Collision(CGameObject* pOther) override;

protected:	// �ڽĿ��� ���� �ʼ����� �Լ�
	virtual HRESULT		SetUp_Components()		{ return S_OK; }
	virtual HRESULT		SetUp_ShaderResources() { return S_OK; }

protected:	// �ڽ��� ���� ����� �Լ�
	virtual void			Monster_Tick(const _double& TimeDelta) {};
	virtual void			Idle_Tick(const _double& TimeDelta) {};
	virtual void			Move_Tick(const _double& TimeDelta) {};
	virtual void			Find_Tick() {};
	virtual void			Attack_Tick(const _double& TimeDelta) {};
	virtual void			Hit_Tick() {};
	virtual	void			Die_Tick(const _double& TimeDelta) {};

protected:	// �ڽĿ��� ������ ���� �Լ�
	_bool				Random_Move(CTransform* pTransform, _float4 f4CenterPos, _double TimeDelta, _double dMoveTime, _float fRange = 2.f);
	_bool				RandomMove(CTransform* pTransform, _float4 f4FirstPos, _float fRange, _double TimeDelta, _float fStart = 0.7f, _float fEnd = 3.f);	// ���� ��ġ�� �̵��ϴ� ���.

	void				Die(const _double& TimeDelta, _float fPlusY, _uint iBronzeCount = 0, _uint iSilverCount = 0, _uint iGoldCount = 0);
	void				Dance_Time();

private:	// �ڽ��� Tick ���� �� �Լ�
	void				HPGauge(const _double& TimeDelta);

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom[COLLTYPE_END] = { nullptr };

protected:	// ��� ������ ���̴� ��
	MONSTERINFO			m_tMonsterInfo;
	MONSTERDESC			m_tMonsterDesc;

protected:	// Shader
	_float				m_fAlpha = 1.f;
	_double				m_dAlpha_TimeAcc = 0;

protected:	
	// On_Collision();
	_bool				m_bPlayer_Attack = false; 
	_double				m_dPlayer_Attack_TimeAcc = 0;

	// Monster_Die();
	_bool				m_OneCoin = false;	
	_int				m_iDieEffect_Count = 0;

private:	
	// RandomMove();
	_float4				m_f4RandomPos;
	
	// Random_Move();
	_float				m_fRandomAxis = 0.f;
	_bool				m_bRandomPos = false;
	_double				m_bRandomMove_TimeAcc = 0;

public:		
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free() override;
};

END