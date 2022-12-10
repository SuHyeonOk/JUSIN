#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CM_Monster abstract : public CGameObject
{
public:
	typedef struct tagMonsterDesc : public CGameObject::GAMEOBJECTDESC
	{
		enum MONSTERKIND { W_BEE, W_WORKER, S_COWBOY, S_SR, MONSTERKIND_END };

		MONSTERKIND		eMonsterKind = MONSTERKIND_END;
		_float3			f3Pos;

	}MONSTERDESC;

	typedef struct tagMonsterInfo
	{
		enum STATE { IDLE, MOVE, FIND, ATTACK, HIT, DIE, STATE_END };

		STATE			eState			= STATE_END;

		_int	iHp			= 0;
		_int	iAttack		= 0;
		_int	iExp		= 0;

	}MONSTERINFO;

protected:
	CM_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Monster(const CM_Monster& rhs);
	virtual ~CM_Monster() = default;

public:
	_int		Get_Monster_Attack() { return m_tMonsterInfo.iAttack; }

public:		// 걍 필수
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(const _double& TimeDelta);
	virtual void		Late_Tick(const _double& TimeDelta);
	virtual HRESULT		Render();

protected:	// 자식에게 거의 필수적인 함수
	virtual HRESULT		SetUp_Components()		{ return S_OK; }
	virtual HRESULT		SetUp_ShaderResources() { return S_OK; }

protected:	// 자식이 자주 사용할 함수
	virtual void		Monster_Die() {};

protected:		
	_bool				RandomMove(CTransform* pTransform, _float4 f4FirstPos, _float fRange, _double TimeDelta, _float fStart = 0.7f, _float fEnd = 3.f);	// 랜덤 위치로 이동하는 기능.

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;

protected:
	MONSTERINFO			m_tMonsterInfo;
	MONSTERDESC			m_tMonsterDesc;

protected:
	_bool				m_OneCoin = false;	// Monster_Die();

private:
	_float4				m_f4RandomPos;
	_bool				m_bRandomPos = false;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free() override;
};

END