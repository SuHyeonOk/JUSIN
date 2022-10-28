#pragma once
#include "Base.h"
// max 32 chan
enum CHANNELID 
{
	MAX_FREE_CHANNEL = 29,
	SOUND_ARROW = MAX_FREE_CHANNEL,
	SOUND_BGM = MAX_FREE_CHANNEL + 1,
	SOUND_UI = MAX_FREE_CHANNEL + 2,
	MAXCHANNEL = 32
};


class CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr() override;

public:
	void PlaySound(const wstring& pSoundKey, const _vec3& vSoundPos, float fVolume = 1.f);
	void PlaySoundRandom(const vector<wstring>& vecSoundKey, const _vec3& vSoundPos, float fVolume = 1.f);
	void PlaySoundChannel(const wstring& pSoundKey, const _vec3& vSoundPos, CHANNELID eID, float fVolume = 1.f);
	void PlaySoundRandomChannel(const vector<wstring>& vecSoundKey, const _vec3& vSoundPos, CHANNELID eID, float fVolume = 1.f);
	// void PlaySoundRandom(const wstring& pSoundKey, const _vec3& vSoundPos, CHANNELID eID, float fVolume = 1.f);

	void PlayBGM(const wstring& pSoundKey, float fVolume);
	
	void StopSound(CHANNELID eID);
	void StopAll();

	void SetChannelVolume(CHANNELID eID, float fVolume);
	void SetChannelPause(CHANNELID eID, bool bPause);

	void Update_Listener(LAYERID eID, const wstring& wstrListernTag);

private:
	void Ready_SoundMgr(); 
	void LoadSoundFile(); 
	virtual void Free() override;

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound; 
	
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

	_vec3 m_vListenerPos = CGameUtilMgr::s_vZero;
	_float m_fMaxListenDist;
	_float m_fMasterVolume;

};

