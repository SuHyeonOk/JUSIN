#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CCoin final : public CGameObject
{
public:
	typedef struct tagCoinInfo
	{
		enum COINKIND { COIN_BRONZE, COIN_SILVER, COIN_GOLD, COIN_END };

		_float3		fPos = _float3(0.f, 0.f, 0.f);
		COINKIND	eCoinKind;
		
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

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void		RandomJump(_double TimeDelta);

private:
	COININFO	m_tinCoinInfo;
	COININFO	m_tCoinInfo;

private:
	// 큰 점프 후 작은 점프 3번
	_bool		m_bBigJump = false;
	_float		m_fSmallJump = 0.0f;

	// 점프 후 회전
	_bool		m_bRotation = false;

	// 점프 중 이동 거리 한 번 저장
	_bool		m_bOneDit = false;


public:
	static	CCoin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END