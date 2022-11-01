#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVoxelTex : public CVIBuffer
{
private:
	explicit CVoxelTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVoxelTex(const CVoxelTex& rhs);
	virtual ~CVoxelTex() override;

public:
	HRESULT Ready_Buffer(const char* strTexPath, _float fCellSize);
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual void Render_Buffer() override;

	static CVoxelTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const char* strTexPath, _float fCellSize);
	virtual CComponent* Clone() override;


};

END
