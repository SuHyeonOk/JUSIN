#pragma once
#include "GameObject.h"
class CPlayerStartPos : public CGameObject
{
private:
	explicit CPlayerStartPos(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerStartPos(const CPlayerStartPos& rhs);
	virtual ~CPlayerStartPos() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Free() override;
	static CPlayerStartPos* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	_uint GetID() const { return m_iID; }
	void SetID(_uint iID) { m_iID = iID; }
	_matrix GetWorld() const { return m_pTransform->m_matWorld; }

private:
	CTransform* m_pTransform = nullptr;
	_uint m_iID = 0;
};

