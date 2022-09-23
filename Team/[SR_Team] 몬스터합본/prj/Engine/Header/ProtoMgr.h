#pragma once
#include "Component.h"

BEGIN(Engine)

// map 컨테이너에 원본 컴포넌트들을 보관하고
// 사용시 clone으로 복사하도록 할 것이다.
class ENGINE_DLL CProtoMgr : public CBase
{
	DECLARE_SINGLETON(CProtoMgr)

private:
	explicit CProtoMgr();
	virtual ~CProtoMgr();

public:
	
	HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent); // 컴포넌트 원본 저장
	CComponent*		Clone_Proto(const _tchar* pProtoTag); // 컴포넌트 사본 생성

private:
	CComponent*		Find_Proto(const _tchar* pProtoTag);

private:
	map<const _tchar*, CComponent*>		m_mapProto;

public:
	virtual void	Free(void);
};

END
