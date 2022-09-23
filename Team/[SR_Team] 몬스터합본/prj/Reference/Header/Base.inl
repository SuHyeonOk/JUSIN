CBase::CBase(void) : m_dwRefCnt(0)
{

}
CBase:: ~CBase(void)
{

}


unsigned long		CBase::AddRef(void)
{
	// 레퍼런스 카운트는 원본 생성 후 복제본이 생성되었을 때 증가하기 때문에 1부터 시작하는게 맞다.
	// 하지만 우리는 0부터 한다
	return ++m_dwRefCnt;
}

unsigned long		CBase::Release(void)
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;// Release 함수를 호출한 객체를 해제

		return 0;
	}
	
	return m_dwRefCnt--;
}