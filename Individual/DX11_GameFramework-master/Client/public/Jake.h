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
	void	Player_Tick(_double TimeDelta);
	void	Current_Player(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);
	void	Key_Input(_double TimeDelta);

	void	Space_Attack_Tick(_double TimeDelta);
	void	Roolling_Tick(_double TimeDelta);
	void	Hit_Tick();
	void	Stun_Tick();
	void	Change_Tick();
	void	Cheering_Tick();

	void	Anim_Change(_double TimeDelta);

private:
	CObj_Manager::PLAYERINFO	m_tPlayerInfo;

	_bool		m_OnMove = false;			// Key_Input() : 키를 누르면 treu 됨

	_double		m_dNotfollow_TimeAcc = 0;	// Check_Follow() : Player 를 따라가지 못 하는 시간

	//////////////////////////// 플레이어 일 때
	_bool		m_bStru = false;			// 스턴
	_uint		m_iStun_Count = 0;			// 스턴 애니메이션 두 번

	//////////////////////////// 플레이어가 아닐 때

public:
	static CJake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END