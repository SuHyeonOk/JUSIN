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

protected:
	virtual void		Help_UI() = 0;
	virtual HRESULT		UI_Dead() = 0;

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

protected:
	_bool			m_bInteraction = false;		// 상호 작용을 하고 있는지
	_bool			m_bHelp_UI = false;			// 물음표가 한 번만 생성되기 위해
	_int			m_Script_Count = 0;			// 'G' 를 누를 때 마다 증가하여 다음 텍스트로 넘어가기 위해

public:		
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free() override;
};

END