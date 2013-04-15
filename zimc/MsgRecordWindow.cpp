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
//��һҳ
int   CMsgRecordWindow::OnUp(char *filename,int page)
{
	// ��Ҫ�ر���. ?
	//OnExit(msg);
    //����ָ��
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
//��һҳ
int CMsgRecordWindow::OnNext(char *filename,int page)
{	
	//����ָ��
    /*
     if( this->page < this->PageCount ){
            this->page=this->page+1 ;
     }
     */
    return 0 ;
}

//���ļ���������
char * CMsgRecordWindow::ReadMsgRecord(char *filename/*,vector<string> &vRecord,int nNumPerPage*/){
    //char buffer[600] ;
    /*
    vector <string> arrMsgRecord ;
    fstream out ;
    out.open(filename,ios::in) ;
    while(!out.eof())    {
        out.getline(buffer,600,'\n');//getline(char *,int,char) ��ʾ�����ַ��ﵽ256�����������оͽ���
        arrMsgRecord.push_back(buffer) ;
    }  
    this->PageCount = arrMsgRecord.size/10+(arrMsgRecord.size%10 == 0 ? 0 : 1) ;
    vRecord = arrMsgRecord[(this->PageCount-nNumPerPage)*10] ;
    this->PageCount = arrMsgRecord.size/10+(arrMsgRecord.size%10 == 0 ? 0 : 1) ;
    return vRecord ;*/
    return NULL;
}