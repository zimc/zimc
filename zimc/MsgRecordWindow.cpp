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
			m_vecMsgRecord.push_back(string());
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
			m_vecMsgRecord[index].resize(len+1);
			char *pBuf = new char[len+1];
			for(int i = 0; i < len; i++) {
				if (fread(pBuf+i, 1, 1, fp) != 1) {
					succ = 4;
					break;
				}
			}
			if (fread(token, strlen(end_token)*sizeof(char), 1, fp) != 1) {
				succ = 5;
				break;
			}
			if (strcmp(end_token, token) != 0) {
				succ = 6;
				break;
			}
		} while(0);
		fclose(fp);
	}
}