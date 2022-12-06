#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CB_Star final : public CGameObject
{
public:
	typedef struct tagBulletDesc
	{
		_float		fAngle;
		_float3		f3Pos;
	}BULLETDESC;

private:
	CB_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CB_Star(const CB_Star& rhs);
	virtual ~CB_Star() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	_float4					m_f4Pos;

public:
	static	CB_Star*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END