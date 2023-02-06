#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;
	
public:
	HRESULT Set_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

	void	Set_ChangeLight(_bool bChange) {
		m_bChangeLight = bChange;
	}

	void	Set_ChangeLight_index(_int iIndex) {
		m_iChangeLight_index = iIndex;
	}

	HRESULT Add_ChangeLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc, const _int iIndex);
	HRESULT Set_ChangeLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc, const _int iIndex);

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	void	Render_Light(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	void	Clear();

private:
	vector<class CLight*>					m_Lights;
	typedef vector<class CLight*>			LIGHTS;

private:
	vector<class CLight*>					m_ChangeLights[5];
	typedef vector<class CLight*>			CHANGELIGHTS;

	_bool			m_bChangeLight = { false };
	_int			m_iChangeLight_index = 0;

public:
	virtual void Free() override;
};

END