#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
public:
	typedef struct tagMonsterInfoTexture
	{
		wstring		wstrName = L"";
		_int		iHp = 0;
		_float3		fPos = { 0.0f, 0.0f ,0.0f };

	}MINFOTEX;

private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	FileTest(list<MINFOTEX*> & fPathList);

private:
	CRenderer*				m_pRendererCom = nullptr;

private:
	HRESULT SetUp_Components();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END