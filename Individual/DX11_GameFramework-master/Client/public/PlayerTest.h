#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;

class CVIBuffer_Terrain;
END

BEGIN(Client)

class CPlayerTest final : public CGameObject
{
private:
	CPlayerTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerTest(const CPlayerTest& rhs);
	virtual ~CPlayerTest() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void	Key_Input(_double TimeDelta);

private:
	_double		m_dTimeAcc = 0;

public:
	static CPlayerTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END