#ifndef Template_h__
#define Template_h__

template <typename T>			// 클래스 포인터들을 해제
DWORD Safe_Release(T& pointer) // 반환타입 DWORD
{
	DWORD	dwRefCnt = 0;

	if (NULL != pointer) // 포인터가 null이 아닌경우
	{
		dwRefCnt = pointer->Release(); // Release() 호출

		if (dwRefCnt == 0)
			pointer = NULL; 
		// delete가 없다? 주소값만 NULL로 채워주고
		// delete구조는 따로 만든다
	}
	return dwRefCnt; // dwRefCnt(레퍼런스 카운트)를 반환한다
}

template <typename T>
void Safe_Single_Destory(T& pointer)
{
	if (NULL != pointer)
	{
		pointer->DestroyInstance();
		pointer = NULL;
	}
}

template <typename T>			// 원시 자료형, 구조체 포인터 해제
void Safe_Delete(T& pointer)
{
	if (NULL != pointer)
	{
		delete pointer;
		pointer = NULL;
	}
}

template <typename T>			// 동적 배열을 삭제하는 용도
void Safe_Delete_Array(T& pointer)
{
	if (NULL != pointer)
	{
		delete[] pointer;
		pointer = NULL;
	}
}
//////////////////////////////////////////////////////////////////
/////////////////////////////////Functor 함수객체//////////////////////////

class CTag_Finder
{
public:
	explicit CTag_Finder(const _tchar* pTag)
		: m_pTargetTag(pTag)
	{
	}
	~CTag_Finder() {		}
public:
	template<typename T>
	bool operator()(const T& pair)
	{
		if (0 == lstrcmpW(m_pTargetTag, pair.first))
		{
			return true;
		}

		return false;
	}

private:
	const _tchar*		m_pTargetTag = nullptr;
};

class CDeleteObj
{
public:
	explicit CDeleteObj(void) {}
	~CDeleteObj(void) {}
public: // operator
	template <typename T>
	void operator () (T& pInstance)
	{
		_ulong dwRefCnt = 0;

		dwRefCnt = pInstance->Release();

		if (0 == dwRefCnt)
			pInstance = nullptr;
	}
};

// 연관컨테이너 삭제용
class CDeleteMap
{
public:
	explicit CDeleteMap(void) {}
	~CDeleteMap(void) {}
public: // operator	
	template <typename T>
	void operator () (T& Pair)
	{
		_ulong dwRefCnt = 0;

		dwRefCnt = Pair.second->Release();

		if (0 == dwRefCnt)
			Pair.second = NULL;
	}
};




#endif // Template_h__
