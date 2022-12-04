#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CFinn final : public CGameObject
{
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

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void	Current_Player(_double TimeDelta);
	void	Key_Input(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);

private:
	_bool		m_OnMove = false;			// Key_Input() : 키를 누르면 treu 됨
		
	_double		m_dNotfollow_TimeAcc = 0;	// Check_Follow() : Player 를 따라가지 못 하는 시간

public:
	static CFinn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END