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
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };

private:
	CFinn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFinn(const CFinn& rhs);
	virtual ~CFinn() = default;

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
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	void	Shader_Time(_double TimeDelta);

private:
	void	Player_Info();

private:
	void	Current_Player(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);
	void	Key_Input(_double TimeDelta);

	void	Space_Attack(_double TimeDelta);
	void	Anim_Change(_double TimeDelta);

private:
	CObj_Manager::PLAYERINFO	m_tPlayerInfo;

	_bool		m_OnMove = false;			// Key_Input() : Ű�� ������ treu ��

	_double		m_dNotfollow_TimeAcc = 0;	// Check_Follow() : Player �� ������ �� �ϴ� �ð�

	_bool		m_bHit = false;
	_double		m_bHit_TimeAcc = 0;			// ���� �ð� �� Hit �� ����

	_bool		m_bSpace_Attack = false;
	_uint		m_iSpace_AttackCount = 0;	// Space_Attack() : ���� 3��
	_double		m_dSpace_AttackTimeAcc = 0;	// �����ð����� space�� ������ ������ ���� ���� �ʱ�ȭ
	_double		m_dSpace_InputTimeAcc = 0;	

	// Anim Test
	_int	m_AnimiNum = 0;

public:
	static CFinn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END