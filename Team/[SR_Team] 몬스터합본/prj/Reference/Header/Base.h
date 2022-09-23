#ifndef Base_h__
#define Base_h__

class _declspec(dllexport) CBase
	//_declspec()	: �ش� Ŭ������ ���� ���
	// dllexport	: �ش� �ڵ带 ���̳ʸ�ȭ�� dll�� ���� ������. => ���� ������Ʈ���� ������ �ڵ�. 
{
protected:
	inline		explicit CBase(void);
	inline		virtual ~CBase(void);

public:
	inline		unsigned long		AddRef(void);		// ���۷��� ī��Ʈ ����
	inline		unsigned long		Release(void);		// ���۷��� ī��Ʈ ���� �� ����

private:
	unsigned long				m_dwRefCnt;

public:
	inline virtual void	Free(void) = 0;	// �� �ڽ� Ŭ�������� ������ ����
};

#include "Base.inl"	// �̷��� �ζ��� �Լ��� ����� ���ÿ� ����. �������� �����Լ� ���� ����̴�.

#endif // Base_h__
