#pragma once
#include "Component.h"

BEGIN(Engine)

// map �����̳ʿ� ���� ������Ʈ���� �����ϰ�
// ���� clone���� �����ϵ��� �� ���̴�.
class ENGINE_DLL CProtoMgr : public CBase
{
	DECLARE_SINGLETON(CProtoMgr)

private:
	explicit CProtoMgr();
	virtual ~CProtoMgr();

public:
	
	HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent); // ������Ʈ ���� ����
	CComponent*		Clone_Proto(const _tchar* pProtoTag); // ������Ʈ �纻 ����

private:
	CComponent*		Find_Proto(const _tchar* pProtoTag);

private:
	map<const _tchar*, CComponent*>		m_mapProto;

public:
	virtual void	Free(void);
};

END
