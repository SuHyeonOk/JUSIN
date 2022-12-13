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

class CCoin final : public CGameObject
{
public:
	typedef struct tagCoinInfo
	{
		enum COINKIND { COIN_BRONZE, COIN_SILVER, COIN_GOLD, COIN_END };
		enum STATE { IDLE, COMEOUT, STATE_END };

		_float3		fPos = _float3(0.f, 0.f, 0.f);
		COINKIND	eCoinKind;
		STATE		eState;
		
	}COININFO;

private:
	CCoin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin(const CCoin& rhs);
	virtual ~CCoin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void On_Collision(CGameObject* pOther) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();

private:
	void		CurrentState(_double TimeDelta);

private:
	COININFO	m_tinCoinInfo;
	COININFO	m_tCoinInfo;

private:
	_bool		m_bPlayer_Collider = false;
	_double		m_dDead_TimeAcc = 0;	// 코인은 바로 사라 지지 않고, 자연스럽게 플레이어를 따라가다가 사라짐

public:
	static	CCoin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END