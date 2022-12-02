#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CM_PigWarrior_BEE final : public CGameObject
{
public:
	typedef struct tagMonsterInfo
	{
		_float3		fPos = _float3(0.f, 0.f, 0.f);
		
	}MONSTERINFO;

private:
	CM_PigWarrior_BEE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_PigWarrior_BEE(const CM_PigWarrior_BEE& rhs);
	virtual ~CM_PigWarrior_BEE() = default;

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
	MONSTERINFO		m_tinMonsterInfo;
	MONSTERINFO		m_tMonsterInfo;

public:
	static	CM_PigWarrior_BEE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END