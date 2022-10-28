#include "stdafx.h"
#include "ClientServiceMgr.h"
#include "ThreadManager.h"
#include "ServerPacketHandler.h"
#include "Service.h"

IMPLEMENT_SINGLETON(CClientServiceMgr)

_bool g_bOnline = false;

CClientServiceMgr::CClientServiceMgr()
{
}


CClientServiceMgr::~CClientServiceMgr()
{
	Free();
}

HRESULT CClientServiceMgr::ReadyClientService()
{
	g_bOnline = true;
	ServerPacketHandler::Init();
	this_thread::sleep_for(1s);

	_service = MakeShared<ClientService>(
		NetAddress(L"192.168.0.27", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager 등
		1);
	ASSERT_CRASH(_service->Start());

	for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([this]()
			{
				while (m_bStop == false)
				{
					_service->GetIocpCore()->Dispatch(3000);
				}
			});
	}

	

	return S_OK;
}

void CClientServiceMgr::Broadcast(const SendBufferRef& sendBuffer) const
{
	_service->Broadcast(sendBuffer);
}


void CClientServiceMgr::Free()
{
	m_bStop = true;
	g_bOnline = false;
	GThreadManager->Join();
}

void CClientServiceMgr::Mat2Pkt(const _matrix& matWorld, Protocol::Matrix& matPkt)
{
	matPkt.mutable_vright()->set_x(matWorld._11);
	matPkt.mutable_vright()->set_y(matWorld._12);
	matPkt.mutable_vright()->set_z(matWorld._13);
	matPkt.mutable_vright()->set_w(matWorld._14);

	matPkt.mutable_vup()->set_x(matWorld._21);
	matPkt.mutable_vup()->set_y(matWorld._22);
	matPkt.mutable_vup()->set_z(matWorld._23);
	matPkt.mutable_vup()->set_w(matWorld._24);

	matPkt.mutable_vlook()->set_x(matWorld._31);
	matPkt.mutable_vlook()->set_y(matWorld._32);
	matPkt.mutable_vlook()->set_z(matWorld._33);
	matPkt.mutable_vlook()->set_w(matWorld._34);

	matPkt.mutable_vpos()->set_x(matWorld._41);
	matPkt.mutable_vpos()->set_y(matWorld._42);
	matPkt.mutable_vpos()->set_z(matWorld._43);
	matPkt.mutable_vpos()->set_w(matWorld._44);
}

void CClientServiceMgr::Pkt2mat(const Protocol::Matrix& matPkt, _matrix& matWorld)
{
	matWorld._11 = matPkt.vright().x();
	matWorld._12 = matPkt.vright().y();
	matWorld._13 = matPkt.vright().z();
	matWorld._14 = matPkt.vright().w();

	matWorld._21 = matPkt.vup().x();
	matWorld._22 = matPkt.vup().y();
	matWorld._23 = matPkt.vup().z();
	matWorld._24 = matPkt.vup().w();

	matWorld._31 = matPkt.vlook().x();
	matWorld._32 = matPkt.vlook().y();
	matWorld._33 = matPkt.vlook().z();
	matWorld._34 = matPkt.vlook().w();

	matWorld._41 = matPkt.vpos().x();
	matWorld._42 = matPkt.vpos().y();
	matWorld._43 = matPkt.vpos().z();
	matWorld._44 = matPkt.vpos().w();
}

/*-----------------
 *  ServerSession
 -----------------*/
void ServerSession::OnConnected()
{
	Protocol::C_LOGIN pkt;
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}
