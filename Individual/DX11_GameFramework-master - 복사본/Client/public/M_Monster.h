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
			SKELETON_ARCHER_1, SKELETON_ARCHER_2, SKELETON_ARCHER_3,
			PENNY,
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
		_float	fAttack		= 0.0f;
		_float	fExp		= 0.0f;

	}MONSTERINFO;

protected:
	CM_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Monster(const CM_Monster& rhs);
	virtual ~CM_Monster() = default;

public:
	_float		Get_Monster_Attack() { return m_tMonsterInfo.fAttack; }

public:		// 걍 필수
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(const _double& TimeDelta);
	virtual void		Late_Tick(const _double& TimeDelta);
	virtual HRESULT		Render();

	virtual void		On_Collision(CGameObject* pOther) override;

protected:	// 자식에게 거의 필수적인 함수
	virtual HRESULT		SetUp_Components()		{ return S_OK; }
	virtual HRESULT		SetUp_ShaderResources() { return S_OK; }

protected:	// 자식이 자주 사용할 함수
	virtual void			Monster_Tick(const _double& TimeDelta) {};
	virtual void			Idle_Tick(const _double& TimeDelta) {};
	virtual void			Move_Tick(const _double& TimeDelta) {};
	virtual void			Find_Tick() {};
	virtual void			Attack_Tick(const _double& TimeDelta) {};
	virtual void			Hit_Tick(const _double& TimeDelta) {};
	virtual	void			Die_Tick(const _double& TimeDelta) {};

protected:	// 자식에서 가져다 쓰는 함수
	_bool				Random_Move(CTransform* pTransform, _float4 f4CenterPos, _double TimeDelta, _float fRange = 2.f);

	void				Die(const _double& TimeDelta, _float fPlusY, _uint iBronzeCount = 0, _uint iSilverCount = 0, _uint iGoldCount = 0);
	void				Dance_Time();

	void				Effect_Hit(_float3 fPulsPos = _float3(0.0f, 1.0f, -0.7f));

private:
	void				Hit_Process(const _double & TimeDelta);		// 자식 Tick 에서 도는 함수

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom[COLLTYPE_END] = { nullptr };

protected:	// 멤버 변수로 쓰이는 것
	MONSTERINFO			m_tMonsterInfo;
	MONSTERDESC			m_tMonsterDesc;

protected:	// Shader
	_float				m_fAlpha = 1.0f;
	_bool				m_bShader_Hit = false;

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
	_bool				m_bRandomTime = false;
	_double				m_bRandom_TimeAcc = 0;
	
	// Random_Move();
	_bool				m_bRandomPos = false;
	_double				m_bRandomMove_TimeAcc = 0;

public:		
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free() override;
};

END