#include "stdafx.h"
#include "MsgRecordWindow.h"
#include<iostream>
#include<fstream>
#include<vector>


CMsgRecordWindow::CMsgRecordWindow(CChatDialog * pChatDialog)
	: m_pChatDialog(pChatDialog)
	  ,MsgCount(0)
	  ,page(1)
	  ,PageCount(0)

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
	if(msg.sType == _T("click"))
	{
		DuiClickButtonMap(_T("CloseBtn"),   OnExit);
		//DuiClickButtonMap(_T("UpBtn"),  OnUp);
		//DuiClickButtonMap(_T("NextBtn"),  OnNext);
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
int   CMsgRecordWindow::OnUp(char *filename,int page)
{
	// 需要关闭吗. ?
	//OnExit(msg);
    //数组指针
    //vector <string> arrMsgRecord ;
    //arrMsgRecord = ReadMsgRecord(filename) ;
    //
/*    if( (arrMsgRecord.size()-page*10)>0){
        if( this->page < this->PageCount ){
            this->page=this->page-1 ;
        }
    }*/
	return 0 ;
}
//下一页
int CMsgRecordWindow::OnNext(char *filename,int page)
{	
	//数组指针
    /*
     if( this->page < this->PageCount ){
            this->page=this->page+1 ;
     }
     */
    return 0 ;
}

//把文件存入数组
char * CMsgRecordWindow::ReadMsgRecord(char *filename/*,vector<string> &vRecord,int nNumPerPage*/){
    //char buffer[600] ;
    /*
    vector <string> arrMsgRecord ;
    fstream out ;
    out.open(filename,ios::in) ;
    while(!out.eof())    {
        out.getline(buffer,600,'\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
        arrMsgRecord.push_back(buffer) ;
    }  
    this->PageCount = arrMsgRecord.size/10+(arrMsgRecord.size%10 == 0 ? 0 : 1) ;
    vRecord = arrMsgRecord[(this->PageCount-nNumPerPage)*10] ;
    this->PageCount = arrMsgRecord.size/10+(arrMsgRecord.size%10 == 0 ? 0 : 1) ;
    return vRecord ;*/
    return NULL;
}
void CMsgRecordWindow::loadMsgRecord() {
	char filename[256] = {0};
	sprintf(filename, ".data\\%d\\%s%d.txt", 
		m_pChatDialog->GetSelfInfo()->nId, 
		(m_pChatDialog->GetFriendInfo()->chType == Type_ImcGroup ? "group" : "friend"),
		m_pChatDialog->GetFriendInfo()->nId);
	FILE *fp = fopen(filename, "rb");
	m_vecMsgRecord.clear();
	if (fp) {
		int succ = 0;
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
	}
	showMsgRecord();
}

void CMsgRecordWindow::showMsgRecord() {
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pmUi.FindControl(_T("ViewRichEdit")));
	Assert(pRichEdit);
	for (size_t i = 0; i < m_vecMsgRecord.size(); i++) {
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

		::OutputDebugStringA("-------------------------------------1\n");
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

		::OutputDebugStringA("-------------------------------------8\n");
		

	}
}