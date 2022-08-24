#pragma once
#include "Obj.h"

class CPlayer :	public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	virtual void	OnCollision(CObj* pOther) override;

public:
	void			OnDamaged(CObj* pOther);

private:
	void	Key_Input(void);

	void	CollisionUpdate(void);

public:
	virtual HRESULT Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render() override;
	virtual void Release(void) override;

private:
	void		Move_Route();

};

// �޸� ����ȭ : ������ �޸𸮷� ���� �޸𸮰� ����ǰų� ����� �޸𸮰� �������� ����� �� ���� ����

//- ���� ����ȭ : ���� �ʿ��� �޸𸮺��� �ʰ��Ǵ� ũ��� �Ҵ�Ǿ� �޸𸮸� ������� ���ϴ� ��Ȳ(EX) ����ü
//- �ܺ� ����ȭ : ����� �޸� �Ҵ�, ���� �� �޸𸮰� �߰��߰� ��� �޸𸮸� ������� ���ϴ� ��Ȳ(EX) ��ũ ���� ����

// �޸� Ǯ��