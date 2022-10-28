#ifndef Engine_Template_h__
#define Engine_Template_h__

#include "Engine_Typedef.h"
#include <functional>
#include "Base.h"

namespace Engine
{
	template <typename T> // 클래스 포인터들을 해제
	DWORD Safe_Release(T& pointer)
	{
		DWORD dwRefCnt = 0;

		if (NULL != pointer)
		{
			dwRefCnt = pointer->Release();

			if (dwRefCnt == 0)
				pointer = NULL;
		}
		return dwRefCnt;
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

	template <typename T> // 원시 자료형, 구조체 포인터 해제
	void Safe_Delete(T& pointer)
	{
		if (NULL != pointer)
		{
			delete pointer;
			pointer = NULL;
		}
	}

	template <typename T> // 동적 배열을 삭제하는 용도
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

	//class CTag_Finder
	//{
	//public:
	//	explicit CTag_Finder(const _tchar* pTag)
	//		: m_pTargetTag(pTag)
	//	{
	//	}
	//	~CTag_Finder() {		}
	//public:
	//	template<typename T>
	//	bool operator()(const T& pair)
	//	{
	//		if (0 == lstrcmpW(m_pTargetTag, pair.first))
	//		{
	//			return true;
	//		}

	//		return false;
	//	}

	//private:
	//	const _tchar*		m_pTargetTag = nullptr;
	//};

	class CDeleteObj
	{
	public:
		explicit CDeleteObj(void)
		{
		}

		~CDeleteObj(void)
		{
		}

	public: // operator
		template <typename T>
		void operator ()(T& pInstance)
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
		explicit CDeleteMap(void)
		{
		}

		~CDeleteMap(void)
		{
		}

	public: // operator	
		template <typename T>
		void operator ()(T& Pair)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = Pair.second->Release();

			if (0 == dwRefCnt)
				Pair.second = NULL;
		}
	};


	template <typename... Args>
	class BaseDelegater
	{
		using CallBack = std::function<void(Args ...)>;

	public:
		~BaseDelegater()
		{
			for (auto& e : m_vecBinds)
				Safe_Release(e.first);
			m_vecBinds.clear();
			m_vecBinds.shrink_to_fit();
		}

		void broadcast(Args&... args)
		{
			for (auto itr = m_vecBinds.begin(); itr != m_vecBinds.end();)
			{
				if (itr->first->GetRef() == 0) // ref == 0이라는 뜻은 이 델리게이터만 해당 ref를 가지고 있다는 뜻
				{
					Safe_Release(itr->first);
					itr = m_vecBinds.erase(itr);
				}
				else
				{
					itr->second(args...);
					++itr;
				}
			}
		}

		template <typename T>
		void bind(T* obj, void (T::*memFunc)(Args ...))
		{
			NULL_CHECK(dynamic_cast<CBase*>(obj));

			CallBack callback = [obj, memFunc](Args ... args)
			{
				(obj->*memFunc)(args...);
			};

			obj->AddRef();
			m_vecBinds.emplace_back(std::make_pair(obj, callback));
		}

		template <typename T>
		void unbind(T* obj)
		{
			auto removedItr = remove_if(m_vecBinds.begin(), m_vecBinds.end(), [&obj](std::pair<void*, CallBack> e)
			{
				return e.first == obj;
			});
			m_vecBinds.erase(removedItr, m_vecBinds.end());
		}

	private:
		std::vector<std::pair<CBase*, CallBack>> m_vecBinds;
	};
}


#endif // Engine_Template_h__
