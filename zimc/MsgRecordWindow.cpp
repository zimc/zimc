#include "stdafx.h"
#include "MsgRecordWindow.h"
#include<iostream>
#include<fstream>
#include<vector>


CMsgRecordWindow::CMsgRecordWindow(CChatDialog * pChatDialog, int suffix_num)
	: m_pChatDialog(pChatDialog)
	, m_nSuffixNum(suffix_num)
	, m_nSuffixNum_now(suffix_num)
	, m_nPage(0)
{}

CMsgRecordWindow::~CMsgRecordWindow()
{}


LPCTSTR CMsgRecordWindow::GetWindowClassName() const
{
	return _T("MsgRecordWnd");
}

tstring CMsgRecordWindow::GetSkinFile()
{
	return _T("msgrecord.xml");
}

void    CMsgRecordWindow::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("windowinit")) {
		while (m_nSuffixNum > 0 && loadMsgRecord() < 0) {
			m_nSuffixNum --;
		}
		int page_count = (m_vecMsgRecord.size() + PAGE_COUNT/2) / PAGE_COUNT;
		m_nPage = page_count - 1;
		m_nPage = max(m_nPage, 0);
		showMsgRecord();
	}
	else if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),   OnExit);
		DuiClickButtonMap(_T("UpBtn"),  OnUp);
		DuiClickButtonMap(_T("NextBtn"),  OnNext);
	}
}

void    CMsgRecordWindow::OnFinalMessage(HWND hWnd)
{
	m_pChatDialog->m_pMsgRecordWindow = 0 ;
	CDuiWindowBase::OnFinalMessage(hWnd);
	delete this;
}

int   CMsgRecordWindow::OnExit(TNotifyUI & msg)
{
	Close();
	return 0;
}
//上一页
int   CMsgRecordWindow::OnUp(TNotifyUI & msg)
{

	if (m_nPage <= 0) {
		int last = m_nSuffixNum;
		m_nSuffixNum --;
		while (m_nSuffixNum > 0 && loadMsgRecord() < 0 ) {
			m_nSuffixNum --;
		}
		if (m_nSuffixNum > 0) {
			m_nPage = (m_vecMsgRecord.size() + PAGE_COUNT/2)/ PAGE_COUNT;
			if (m_nPage > 0) m_nPage --;
		}
		else {
			m_nSuffixNum = last;
		}
	} else {
		m_nPage --;
	}
	showMsgRecord();
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(_T("ViewRichEdit")));
	Assert(pRichEdit);
	pRichEdit->EndDown();
	//pRichEdit->PostMessage(WM_VSCROLL, SB_TOP, 0);
	return 0 ;
}
//下一页
int CMsgRecordWindow::OnNext(TNotifyUI & msg) {
	int count = (m_vecMsgRecord.size() + PAGE_COUNT/2) / PAGE_COUNT;
	count = max(count ,1);
	if (m_nPage >= count - 1) {
		m_nSuffixNum ++;
		while (m_nSuffixNum <= m_nSuffixNum_now && loadMsgRecord() < 0) {
			m_nSuffixNum ++;
		}
		if (m_nSuffixNum > m_nSuffixNum_now) {
			m_nSuffixNum = m_nSuffixNum_now;
		}
		else {
			m_nPage = 0;
		}
	}
	else {
		m_nPage ++;
	}
	showMsgRecord();
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(_T("ViewRichEdit")));
	Assert(pRichEdit);
	pRichEdit->HomeUp();
    return 0 ;
}


int CMsgRecordWindow::loadMsgRecord() {
	char filename[256] = {0};
	sprintf(filename, ".data\\%d\\%s%d.%d", 
		m_pChatDialog->GetSelfInfo()->nId, 
		(m_pChatDialog->GetFriendInfo()->chType == Type_ImcGroup ? "group" : "friend"),
		m_pChatDialog->GetFriendInfo()->nId, m_nSuffixNum);
	FILE *fp = fopen(filename, "rb");
	if (fp) {
		int succ = 0;
		m_vecMsgRecord.clear();
		do {
			size_t index = m_vecMsgRecord.size();
			char *start_token = "textbegin@";
			char *end_token = "@textend\r\n";
			char token[64] = {0};
			if (fread(token, strlen(start_token)*sizeof(char), 1, fp) != 1) {
				succ = 1;
				break;
			}
			if (strcmp(start_token, token) != 0) {
				succ = 2;
				break;
			}
			int len;
			if (fread(&len, sizeof(int), 1, fp) != 1) {
				succ = 3;
				break;
			}
			
			char *pBuf = new char[len+1];
			for(int i = 0; i < len; i++) {
				if (fread(pBuf+i, sizeof(char), 1, fp) != 1) {
					succ = 4;
					break;
				}
			}
			if (succ != 0) {
				delete []pBuf;
				break;
			}
			if (fread(token, strlen(end_token)*sizeof(char), 1, fp) != 1) {
				delete []pBuf;
				succ = 5;
				break;
			}
			if (strcmp(end_token, token) != 0) {
				delete []pBuf;
				succ = 6;
				break;
			}
			m_vecMsgRecord.push_back(string());
			m_vecMsgRecord[index] = string(pBuf, len);
		} while(1);
		fclose(fp);
		return m_vecMsgRecord.size();
	}
	return -1;
}

void CMsgRecordWindow::showMsgRecord() {
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(_T("ViewRichEdit")));
	Assert(pRichEdit);
	pRichEdit->SetText(_T(""));
	int page_count = (m_vecMsgRecord.size() + PAGE_COUNT/2) / PAGE_COUNT;
	page_count = max(page_count, 1);
	size_t start_pos = m_nPage * PAGE_COUNT;
	size_t end_pos = (m_nPage + 1) * PAGE_COUNT;
	if (m_nPage == page_count - 1) {
		end_pos = m_vecMsgRecord.size();
	}
	end_pos = min(end_pos, m_vecMsgRecord.size());
	for (size_t i = start_pos; i < end_pos; i++) {
		//int s = sizeof(int) + strlen(pChatData->szTime)*sizeof(char) + sizeof(int) + strlen(pChatData->szSenderName)*sizeof(char) + sizeof(int)+ pChatData->nDataLen*sizeof(char);
		int len;
		const char *pBuf = m_vecMsgRecord[i].c_str();
		string strTime, strSenderName, strData;
		memcpy(&len,pBuf, sizeof(int));
		strTime = ": ";
		strTime.append(pBuf+sizeof(int), len);
		strTime.append("\n");
		//strTime = string(pBuf+sizeof(int), len);
		pBuf += sizeof(int) + len;

		memcpy(&len, pBuf, sizeof(int));
		strSenderName = string(pBuf + sizeof(int), len);
		pBuf += sizeof(int) + len;

		memcpy(&len, pBuf, sizeof(int));
		strData = string(pBuf + sizeof(int), len);
		ChatFont_t cht;
		memcpy(&cht, strData.c_str(), sizeof(ChatFont_t));
		
		CA2T tsSenderName(strSenderName.c_str());
		CA2T tsTime(strTime.c_str());
		CA2T tsText(strData.c_str() + sizeof(ChatFont_t));
		//tstring tstrTime = _T(": ") + tsTime + _T("\n");

		LPCTSTR tszSenderName = tsSenderName;
		LPCTSTR tszTime       = tsTime;
		LPCTSTR tszText       = tsText;

		long lSelBegin = 0, lSelEnd = 0;
		CHARFORMAT2 cf;
		ZeroMemory(&cf, sizeof(CHARFORMAT2));

		// ------------------------------------------------------
		// name + time
		cf.cbSize         = sizeof(cf);
		cf.dwReserved     = 0;
		cf.dwMask         = CFM_COLOR | CFM_LINK | CFM_UNDERLINE | CFM_UNDERLINETYPE;
		cf.dwEffects      = CFE_LINK;
		cf.bUnderlineType = CFU_UNDERLINE;
		cf.crTextColor    = RGB(220, 0, 0);
		lSelEnd =  lSelBegin = pRichEdit->GetTextLength();
		pRichEdit->SetSel(lSelEnd, lSelEnd);
		pRichEdit->ReplaceSel(tszSenderName, false);
		lSelEnd =  pRichEdit->GetTextLength();
		pRichEdit->SetSel(lSelBegin, lSelEnd);
		pRichEdit->SetSelectionCharFormat(cf);

		lSelEnd =  lSelBegin = pRichEdit->GetTextLength();
		pRichEdit->SetSel(lSelEnd, lSelEnd);
		pRichEdit->ReplaceSel(tszTime, false);
		cf.dwMask         = CFM_COLOR;
		cf.crTextColor    = RGB(220, 0, 0);
		cf.dwEffects      = 0;
		lSelEnd =  pRichEdit->GetTextLength();
		pRichEdit->SetSel(lSelBegin, lSelEnd);
		pRichEdit->SetSelectionCharFormat(cf);

		// ------------------------------------------------------
		// text.
		PARAFORMAT2 pf;
		ZeroMemory(&pf, sizeof(PARAFORMAT2));
		pf.cbSize         = sizeof(pf);
		pf.dwMask         = PFM_STARTINDENT;
		pf.dxStartIndent  = 0;
		pRichEdit->SetParaFormat(pf);

		lSelEnd = lSelBegin = pRichEdit->GetTextLength();
		pRichEdit->SetSel(-1, -1);
		pRichEdit->ReplaceSel(tszText, false);
		pRichEdit->SetSel(-1, -1);
		pRichEdit->ReplaceSel(_T("\n"), false);

		//设置聊天内容字体样式
		//LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic
		//TODO 大小 未解决 ??? 
		cf.dwMask = CFM_COLOR| CFM_FACE | CFM_SIZE;
		cf.yHeight = cht.dwFontSize;
		cf.crTextColor = cht.dwTextColor;
		cf.yHeight = cht.dwFontSize*cht.dwFontSize;
		_tcscpy_s(cf.szFaceName, cht.tszFontName);
		if (cht.bBold) {
			cf.dwMask |= CFM_BOLD;
			cf.dwEffects |= CFE_BOLD;
		}
		if (cht.bUnderline) {
			cf.dwMask |= CFM_UNDERLINE;
			cf.dwEffects |= CFE_UNDERLINE;
		}
		if (cht.bItalic) {
			cf.dwMask |= CFM_ITALIC;
			cf.dwEffects |= CFE_ITALIC;
		}
		lSelEnd = pRichEdit->GetTextLength();
		pRichEdit->SetSel(lSelBegin, lSelEnd);
		pRichEdit->SetSelectionCharFormat(cf);

		ZeroMemory(&pf, sizeof(PARAFORMAT2));
		pf.cbSize         = sizeof(pf);
		pf.dwMask         = PFM_STARTINDENT;
		pf.dxStartIndent  = 220;
		pRichEdit->SetParaFormat(pf);

		pRichEdit->EndDown();
	}
	//pRichEdit->SetSel(0, 0);
}