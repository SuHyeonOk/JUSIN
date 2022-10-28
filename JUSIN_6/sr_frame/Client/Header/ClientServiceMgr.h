#pragma once
#include "Base.h"
#include "Session.h"
#include "Protocol.pb.h"

extern _bool g_bOnline;

class CClientServiceMgr : public CBase
{
	DECLARE_SINGLETON(CClientServiceMgr)
private:
	explicit CClientServiceMgr();
	virtual ~CClientServiceMgr();

public:
	HRESULT ReadyClientService();
	void Broadcast(const SendBufferRef& sendBuffer) const;
	
public:
	virtual void Free() override;

	static void Mat2Pkt(const _matrix& matWorld, Protocol::Matrix& matPkt);
	static void Pkt2mat(const Protocol::Matrix& matPkt, _matrix& matWorld);

// {
// 	matPkt.mutable_vright()->set_x(matWorld._11);
// 	matPkt.mutable_vright()->set_y(matWorld._12);
// 	matPkt.mutable_vright()->set_z(matWorld._13);
// 	matPkt.mutable_vright()->set_w(matWorld._14);
//
// 	matPkt.mutable_vup()->set_x(matWorld._21);
// 	matPkt.mutable_vup()->set_y(matWorld._22);
// 	matPkt.mutable_vup()->set_z(matWorld._23);
// 	matPkt.mutable_vup()->set_w(matWorld._24);
//
// 	matPkt.mutable_vlook()->set_x(matWorld._31);
// 	matPkt.mutable_vlook()->set_y(matWorld._32);
// 	matPkt.mutable_vlook()->set_z(matWorld._33);
// 	matPkt.mutable_vlook()->set_w(matWorld._34);
//
// 	matPkt.mutable_vpos()->set_x(matWorld._41);
// 	matPkt.mutable_vpos()->set_y(matWorld._42);
// 	matPkt.mutable_vpos()->set_z(matWorld._43);
// 	matPkt.mutable_vpos()->set_w(matWorld._44);
// }

	uint32 m_iPlayerID;


private:
	ClientServiceRef _service;
	bool m_bStop = false;

};


/*-----------------
 *  ServerSession
 -----------------*/
class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};