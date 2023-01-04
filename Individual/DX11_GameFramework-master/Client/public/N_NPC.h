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

class CN_NPC abstract : public CGameObject
{
public:
	typedef struct tagNpcInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum NPC { BUBBLEGUM, KEYMAN, DOCTOR, NPC_END };

		NPC		eNpcType = NPC_END;

	}NPCDESC;

protected:
	CN_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CN_NPC(const CN_NPC& rhs);
	virtual ~CN_NPC() = default;

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

protected:
	virtual void		Help_UI() = 0;
	virtual HRESULT		UI_Dead() = 0;

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

protected:
	_bool			m_bInteraction = false;		// ��ȣ �ۿ��� �ϰ� �ִ���
	_bool			m_bHelp_UI = false;			// ����ǥ�� �� ���� �����Ǳ� ����
	_int			m_Script_Count = 0;			// 'G' �� ���� �� ���� �����Ͽ� ���� �ؽ�Ʈ�� �Ѿ�� ����

public:		
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free() override;
};

END