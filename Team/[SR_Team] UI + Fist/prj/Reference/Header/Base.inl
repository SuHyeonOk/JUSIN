CBase::CBase(void) : m_dwRefCnt(0)
{

}
CBase:: ~CBase(void)
{

}


unsigned long		CBase::AddRef(void)
{
	// ���۷��� ī��Ʈ�� ���� ���� �� �������� �����Ǿ��� �� �����ϱ� ������ 1���� �����ϴ°� �´�.
	// ������ �츮�� 0���� �Ѵ�
	return ++m_dwRefCnt;
}

unsigned long		CBase::Release(void)
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;// Release �Լ��� ȣ���� ��ü�� ����

		return 0;
	}
	
	return m_dwRefCnt--;
}