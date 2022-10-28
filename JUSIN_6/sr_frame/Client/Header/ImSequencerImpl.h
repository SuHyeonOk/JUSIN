#pragma once
#include "ImCurveEdit.h"
#include "ImSequencer.h"
#include "GraphEditor.h"
#include "SkeletalCube.h"

class CImSequencerImpl : public ImSequencer::SequenceInterface
{
public:
	CImSequencerImpl() : m_iFrameMin(0), m_iFrameMax(0)
	{
		m_vecPartName.push_back("__EVENT__");
	}

public:
	virtual int GetFrameMin() const
	{
		return m_iFrameMin;
	}

	virtual int GetFrameMax() const
	{
		return m_iFrameMax;
	}

	virtual int GetItemCount() const { return (int)m_vecPartName.size(); }

	virtual int GetItemTypeCount() const { return 1; }
	virtual const char* GetItemTypeName(int typeIndex) const { return "T"; }

	virtual const char* GetItemLabel(int index) const
	{
		static char tmps[128];
		_snprintf_s(tmps, 128, "%s", m_vecPartName[index].c_str());
		return tmps;
	}

	virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
	{
		if (color)
			*color = 0xFFAA8080; // same color for everyone, return color based on type
		if (start)
			*start = &m_iFrameMin;
		if (end)
			*end = &m_iFrameMax;
		// if (type)
		// 	*type = item.mType;
	}
	void AddPart(const char* szPartName)
	{
		string strPartname(szPartName);
		m_CubeAnim.mapFrame.insert({strPartname, {}});
		m_vecPartName.push_back(strPartname);
	}

	// virtual void Add(int type) { myItems.push_back(MySequenceItem{type, 0, 10, false}); };
	virtual void Del(int index)
	{
		string strPartName = m_vecPartName[index];
		if (strPartName == "__EVENT__") return;
		m_CubeAnim.mapFrame.erase(strPartName);
		m_vecPartName.erase(m_vecPartName.begin() + index);
	}
	// virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }
	//
	// virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

	void AddEvent(int iCurFrame, const char* szEventName)
	{
		m_CubeAnim.vecEvent.push_back({Frame2Sec(iCurFrame), szEventName});
	}

	void MoveFrame(int& iCurFrame, int iFrameAmount)
	{
		bool bFound = false;
		for (auto& vecFrame : m_CubeAnim.mapFrame)
		{
			for (auto& frame : vecFrame.second)
			{
				if (Sec2Frame(frame.fTime) == iCurFrame)
				{
					frame.fTime += Frame2Sec(iFrameAmount);
					bFound = true;
				}
			}
		}

		if (bFound)
			iCurFrame += iFrameAmount;
	}


	virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
	{
		string& strPartName = m_vecPartName[index];

		if (strPartName == "__EVENT__")
		{
			draw_list->PushClipRect(rc.Min, rc.Max, true);

			for (auto itr = m_CubeAnim.vecEvent.begin(); itr != m_CubeAnim.vecEvent.end();)
			{
				float fCurFrameCnt =  (float)Sec2Frame(itr->first); // sec to frame
				float r = (fCurFrameCnt - m_iFrameMin) / float(m_iFrameMax - m_iFrameMin);
				float x = ImLerp(rc.Min.x, rc.Max.x, r);
				draw_list->AddCircleFilled(ImVec2(x, rc.Min.y + 10), 6.f, 0xAAA00000);
				ImVec2 pta(x - 3, rc.Min.y + 10 - 3);
				ImVec2 ptb(x + 3, rc.Min.y + 10 + 3);
				if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()))
				{
					if (ImGui::IsMouseClicked(1))
					{
						itr = m_CubeAnim.vecEvent.erase(itr);
						continue;
					}
					draw_list->AddText(ImVec2(x + 7, rc.Min.y + 2), 0xAA000000, itr->second.c_str());
				}

				++itr;
			}
			draw_list->PopClipRect();
		}
		else
		{
			vector<TransFrame>& vecFrame = m_CubeAnim.mapFrame.find(strPartName)->second;

			draw_list->PushClipRect(rc.Min, rc.Max, true);

			for (auto itr = vecFrame.begin(); itr != vecFrame.end();)
			{
				float fCurFrameCnt = itr->fTime * 60.f; // sec to frame
				float r = (fCurFrameCnt - m_iFrameMin) / float(m_iFrameMax - m_iFrameMin);
				float x = ImLerp(rc.Min.x, rc.Max.x, r);
				draw_list->AddCircleFilled(ImVec2(x, rc.Min.y + 10), 6.f, 0xAA000000);
				ImVec2 pta(x - 3, rc.Min.y + 10 - 3);
				ImVec2 ptb(x + 3, rc.Min.y + 10 + 3);
				if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(1))
				{
					itr = vecFrame.erase(itr);
				}
				else ++itr;
			}
			draw_list->PopClipRect();
		}


	}

	void LoadAnimFrame(const wstring& wstrPath);

	void AddTransFrame(const int iCurrentFrame, const SkeletalPart* pPart);
	void AddTransFrameRecur(const int iCurrentFrame, const SkeletalPart* pPart);

	static _float Frame2Sec(const int iFrame) { return _float(iFrame) / 60.f; }
	static int Sec2Frame(const _float fSec) { return int(fSec * 60.f); }

public:
	int m_iFrameMin, m_iFrameMax;
	CubeAnimFrame m_CubeAnim;
	vector<string> m_vecPartName; // CubeAnimFrame의 맵 컨테이너에 임시로 index를 주기 위함
};
