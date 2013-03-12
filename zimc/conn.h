#pragma once
#include "stdafx.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include<iostream>
#include <string>
#include <stdio.h>

#include "msg.h"

using namespace std;


typedef int (*callback) (msg_t *msg);

class conn_t {
public:
    conn_t(size_t rsz, size_t wsz, string &adr, string &p);
    ~conn_t();

    bool connectToServer();
    int sendToServer(msg_t *msg);
    int dispatch(callback func, int timeout_sec);
    bool isValid() {
        return invalid == 0;
    }

private:
    int fd;
    int invalid;

    char *rbuf;
    char *wbuf;
    size_t wsize;
    size_t rsize;
    int rpos,wpos;
    string address;
    string port;
    fd_set readset;
    fd_set writeset;
    fd_set master;

    bool reConnectToServer();
    int fill_buffer();
    int read_data(void *buf, int len);
    int send_buffer();
    int send_data(const void *buf, int len) ;
    int send_msg(msg_t *msg);
    void proc_data(callback func) ;

    void clearFdSet() {
        FD_ZERO(&master);
        FD_ZERO(&writeset);
        FD_ZERO(&readset);
    }
};
