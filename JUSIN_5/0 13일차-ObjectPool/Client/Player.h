#pragma once
#include "Obj.h"
class CPlayer :	public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	virtual HRESULT Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(void) override;
	virtual void Release(void) override;

	// Player.h
private:
	void		Move_Route();
	void		Fire_Bullet(void);

private:
	float		m_fFireItv = 0.f;		// �ð����� ����(Interval)�� �� ����
	float		m_fFireAngle = 0.f;		// 360���� �������� ����� ����

	int			m_iCount = 0;			// �Ѿ��� �� �� �������� ���� ����

};

// �޸� ����ȭ : ������ �޸𸮷� ���� �޸𸮰� ����ǰų� ����� �޸𸮰� �������� ����� �� ���� ����

//- ���� ����ȭ : ���� �ʿ��� �޸𸮺��� �ʰ��Ǵ� ũ��� �Ҵ�Ǿ� �޸𸮸� ������� ���ϴ� ��Ȳ(EX) ����ü
//- �ܺ� ����ȭ : ����� �޸� �Ҵ�, ���� �� �޸𸮰� �߰��߰� ��� �޸𸮸� ������� ���ϴ� ��Ȳ(EX) ��ũ ���� ����

// �޸� Ǯ��