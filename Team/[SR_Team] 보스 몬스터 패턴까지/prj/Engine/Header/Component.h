#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

// ������Ʈ ���� : "��ɵ��� ��ǰȭ" �Ͽ� �ʿ��� �� "������ ���ڴ�"�� ������ �����̴�.
// ������ ������Ʈȭ ��ų ���ΰ��� ���� ����� �ʿ��ϴ�.
// manangement > Scene > Layer > Object > Component

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	virtual _int			Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void			LateUpdate_Component(void) {}

protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_bool					m_bClone;

public:
	virtual CComponent*	Clone(void)PURE;	// ������Ÿ�� ����
											// ���� �ϳ��� ����� �����ϰ�, �纻�� ��������� �ʿ��� �� ������ ����.
											// ex) return new CRcCol(*this); => ������Ʈȭ �ߴ�.
	virtual void		Free(void);
};

END