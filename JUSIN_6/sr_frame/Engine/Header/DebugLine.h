#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CDebugLine : public CGameObject
{
private:
	explicit CDebugLine(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDebugLine(const CDebugLine& rhs);
	virtual ~CDebugLine() override;

public:
	HRESULT Ready_Object(const _vec3& vOrigin, const _vec3& vDir, const _float fLength);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CDebugLine* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vOrigin, const _vec3& vDir, const _float fLength, _float fTime = 1.f);

private:
	ID3DXLine* m_pLine = nullptr;
	_float m_fTime = 0.f;
	_vec3 m_vVertexList[2];
};
END
