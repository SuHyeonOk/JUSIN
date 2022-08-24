#pragma once
#include "Scene.h"
class CLogo :
	public CScene
{
public:
	CLogo();
	virtual ~CLogo();

public:
	CRITICAL_SECTION		Get_Crt(void) { return m_Crt; }

public:
	virtual HRESULT Ready_Scene() override;
	virtual void Update_Scene() override;
	virtual void Late_Update_Scene() override;
	virtual void Render_Scene() override;
	virtual void Release_Scene() override;

public:
	static size_t __stdcall ImgLoadThreadFunc(void* pVoid); // ﻿로고 클래스가 할 기능을 실행해줄 함수
	// static을 붙여주었다 쓰레드 함수는 조건자와 마찬가지로 static또는 전역함수여야 한다
	// 특정 객체에 소속된 것이 아닌 독립적인 함수로써 동작하기 때문에 멤버함수로 만들 수 없다
	// 반드시 함수호출 규약이 std_call이여야만 한다 그래야 시스템 메시지에 의해서 쓰레드를 언제 호출할지가 결정된다

	// size_t는 unsigned int이다 
	// 함수가 종료될 때 return되는 값에 의해서 쓰레드가 잘 구동 되는지 안 되는지에 대한 에러 코드처리가 가능하기에 unsigned int를 사용한다

	// 매개변수 (void* pVoid) 
	// 이 쓰레드 함수를 통해서 외부에 있는 어떤 데이터를 가공하고 싶을 때 가공할 대상이 어떤 자료형인지 모른다
	// ex_ TextRPG에서 Player의 Hp를 쓰레드로 가공하고 싶다면 int데이터로 받을 것 이다
	// 그런데 성적표의 경우에는 평균값을 쓰레드를 통해 가져오고 싶다면 float데이터로 받을 것 이다
	// 이렇게 어떤 자료형이 필요한 지를 알 수 없기 때문에 범용성을 위해서 자료형을 void*로 만들어 준 것이다
	// 실제 코드 구현시는 명시적으로 형변환을 반드시 해주어야 한다


public:
	static CLogo*		Create(void);

private:
	CRITICAL_SECTION			m_Crt;
	HANDLE						m_hThread;

};

