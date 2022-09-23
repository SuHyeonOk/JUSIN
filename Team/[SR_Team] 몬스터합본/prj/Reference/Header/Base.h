#ifndef Base_h__
#define Base_h__

class _declspec(dllexport) CBase
	//_declspec()	: 해당 클래스의 목적 명시
	// dllexport	: 해당 코드를 바이너리화해 dll로 추출 목적임. => 엔진 프로젝트에게 내려줄 코드. 
{
protected:
	inline		explicit CBase(void);
	inline		virtual ~CBase(void);

public:
	inline		unsigned long		AddRef(void);		// 레퍼런스 카운트 증가
	inline		unsigned long		Release(void);		// 레퍼런스 카운트 감소 및 삭제

private:
	unsigned long				m_dwRefCnt;

public:
	inline virtual void	Free(void) = 0;	// 각 자식 클래스마다 구현될 내용
};

#include "Base.inl"	// 이렇게 인라인 함수를 선언과 동시에 구현. 고전적인 전역함수 구현 방식이다.

#endif // Base_h__
