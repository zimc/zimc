#include "stdafx.h"

#include "conn.h"
#pragma comment (lib, "Ws2_32.lib")

typedef int (*callback) (msg_t *msg);

conn_t::conn_t(size_t rsz, size_t wsz, string &adr, string &p){
    fd = -1;
    address = adr;
    port = p;
    invalid = 1;
    rsize = rsz;
    wsize = wsz;
    rpos = wpos = 0;
    rbuf = new char[rsize + 1];
    wbuf = new char[wsize + 1];
    clearFdSet();
}
conn_t::~conn_t() {
    if (fd > 0) {
        closesocket(fd);
    }
    delete []rbuf;
    delete []wbuf;
}

bool conn_t::reConnectToServer() {
    cout<<"reconnect ... "<<endl;
    if (fd > 0) {
        closesocket(fd);
        fd = -1;
    }
    return connectToServer();
}

bool conn_t::connectToServer() {
    if (isValid()) {
        return true;
    }
    clearFdSet();
    fd = socket(PF_INET, SOCK_STREAM, 0); 
    if (fd < 0) {
        return false;
    }
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int nRet = getaddrinfo(address.c_str(), port.c_str(), &hints, &res);
	if(nRet) {
		closesocket(fd);
		fd = -1;
        cerr <<"Server Address Error!"<<endl;;
		return false;
	}
    int TimeOut = 6000; //6 sec
    if(::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR) {
        closesocket(fd);
        fd = -1;
        return false;
    }
    if (connect(fd, res->ai_addr, res->ai_addrlen)<0) {
        closesocket(fd);
        cerr <<"Connect to server failed!"<<endl;;
        return false; 
    }
    u_long iMode = 1;
    if (ioctlsocket(fd, FIONBIO, &iMode) == SOCKET_ERROR) {
        closesocket(fd);
        fd = -1;
        return false;
    } 
    invalid = 0;
    FD_SET(fd, &master);
    return true;
}

int conn_t::sendToServer(msg_t *msg) {
    if (!isValid()) {
        return -1;
    }
    return send_msg(msg);
}

/*
* return : 0: 成功，1：需要成功发送登录验证，-1：与服务器断开连接，重连失败
*/
int conn_t::dispatch(callback func, int timeout_sec) {
    int ret = 0;
    if (!isValid()) {
        ret = 1;
        if (!reConnectToServer()) {
            cerr<<"reconnect to server failed"<<endl;
            ret = -1;
        }
    }
    if (ret < 0) {
        return ret;
    }
    FD_ZERO(&writeset);
    struct timeval timeout;
    int fdmax = fd;
    readset = master;
    if (wpos > 0) {
        writeset = master;
    }
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;
    int count = select(fdmax+1, &readset, &writeset, NULL, &timeout);
    if (count < 0) {
        cerr<<"select failed ..."<<endl;
    }
    if (count > 0) {
        for (int i = 0; i<= fdmax; i++) {
            if (FD_ISSET(i, &readset)) {
                fill_buffer();
                proc_data(func);
                cout<<"read ... "<<endl;
            }
            if (FD_ISSET(i, &writeset)) {
                send_buffer();
                cout<<"write ... " <<endl;
            }
        }
    }
    else {
        cerr<<"timeout ..."<<endl;
    }
    return ret;
}

int conn_t::fill_buffer() {
    int readsz = 0;
    while (1) {
        int bufsize = rsize - rpos;
        if (bufsize <= 0) return -1;
        int n = recv(fd, rbuf + rpos, bufsize, 0);
        int err = WSAGetLastError();
        if (n < 0 && err != WSAEWOULDBLOCK) {
            //TODO err
            cerr<<"read from socket error, fd:"<<fd<<" errno:"<<err<<endl;
            invalid = 1;
            return -1;
        }
        if (n < 0) break;
        if (n == 0) {
            invalid = 1;
            break;
        }
        readsz += n;
        rpos += n;
    }
    return readsz;
}

int conn_t::read_data(void *buf, int len) {               
    if (rpos < len)  return -1;
    int size = len;
    memcpy(buf, rbuf, size);
    memmove(rbuf, rbuf + size, rpos - size);
    rpos -= size;
    return size;
}

int conn_t::send_buffer() {
    int writesz = 0;
    while (1) {
        int n = send(fd, wbuf + writesz, wpos - writesz, 0);
        int err = WSAGetLastError();
        if (n < 0 && err != WSAEWOULDBLOCK) {
            cerr<<"write to socket error, fd:"<<fd<<" errno:"<<err<<endl;
            invalid = 1;  
            return -1; 
        }   
        if (n <= 0) break;
        writesz += n;
    }       
    if (writesz) {
        memmove(wbuf, wbuf + writesz, wpos - writesz); 
        wpos -= writesz;
    }                           
    return writesz;             
}  
int conn_t::send_data(const void *buf, int len) {   
    if (wsize - wpos < (size_t)len) {
        cerr<<"send buffer not enough, fd:"<<fd<<" wpos:"<<wpos<<" len:"<<len<<endl;
        return -1; 
    }       
    memcpy(wbuf + wpos, buf, len);
    wpos += len;
    if (send_buffer() < 0) {
        return -1;
    }   
    return len;
}   
int conn_t::send_msg(msg_t *msg) {
    int bufsize = wsize - wpos;
    int datalen = msg->serialize_size();
    if (bufsize < datalen + 6) {
        cerr<<"send buffer not enough, buf:"<<bufsize<<" datalen:"<<datalen<<endl; 
        return -1; 
    }   
    char *buf = wbuf + wpos;
    sprintf(buf, "%05X@", datalen);
    msg->serialize(buf+6);
    wpos += datalen + 6;
    return datalen;
}

void conn_t::proc_data(callback func) {
    static char sbuf[8192];
    for (;;) {
        if (rpos <= 6) {
            break;
        }   
        char *buf = sbuf;
        unsigned int ulen = 0;
        sscanf(rbuf, "%05X", &ulen);
        if ((unsigned int)rpos < (6 + ulen)) {
            break;
        }   
        if (ulen + 6 > sizeof(sbuf)) {
            buf = (char *)malloc(sizeof(char)*(ulen+6));
        }   
        if (buf == NULL) {
            cerr<<"proc_data: allocate memory faield"<<endl;
            break;
        }   
        if (read_data(buf, ulen+6) <= 0) {
            cerr<<"proc_data: read_data failed"<<endl;
            break;
        }
        buf[ulen+6-1] = '\0';

        msg_t *msg = new msg_t();
        msg->unserialize(buf+6);

        func(msg);

        delete msg;

        if (buf != sbuf) {
            free(buf);
            buf = NULL;
        }   
    }   
}
