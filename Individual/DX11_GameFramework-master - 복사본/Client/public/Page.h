#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Skill_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CPage final : public CGameObject
{
public:
	typedef struct tagPageInfo
	{
		CSkill_Manager::PLAYERSKILL::SKILL		ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END;

		_float3		fPos = _float3(0.f, 0.f, 0.f);
		_bool		bJemp = false;

	}PAGEINFO;

private:
	CPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPage(const CPage& rhs);
	virtual ~CPage() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void		Random_Jump(const _double & TimeDelta);

private:
	PAGEINFO	m_tinPageInfo;
	PAGEINFO	m_tPageInfo;

	// RandomJump()
	_bool		m_bBigJump = false;		// 큰 점프 후 작은 점프 3번
	_float		m_fSmallJump = 0.0f;	// 작은 점프를 저장할 변수
	_bool		m_bRotation = false;	// 점프 가 끝나면 회전할 것 인지
	_bool		m_bOneDir = false;		// 점프 중 방향 한 번 저장
	
public:
	static	CPage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END