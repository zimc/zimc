#ifndef _GAME_SERIALIZE_H
#define _GAME_SERIALIZE_H

#include "stdafx.h"
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned char byte;

inline int savebyte(char *&dest, byte data) {
    int sz = sizeof(data);
    *(byte*)dest = data;
    dest += sz;
    return sz;
}
inline int savebytes(char *&dest, const char *data, int len) {
    memcpy(dest, data, len);
    dest += len;
    return len;
}
inline int saveuint(char *&dest, unsigned int data) {
    int sz = sizeof(data);
    *(unsigned int*)dest = data;
    dest += sz;
    return sz;
}
inline int saveint(char *&dest, int data) {
    int sz = sizeof(data);
    *(int*)dest = data;
    dest += sz;
    return sz;
}
inline int savelonglong(char *&dest, long long data) {
    int sz = sizeof(data);
    *(long long*)dest = data;
    dest += sz;
    return sz;
}
inline int savefloat(char *&dest, float data) {
    int sz = sizeof(data);
    *(float*)dest = data;
    dest += sz;
    return sz;
}
inline int savestring(char *&dest, const string &data) {
    int len = data.length()+1;
    saveint(dest, len);
    savebytes(dest, data.c_str(), len);
    return sizeof(len) + len;
}
inline int savestr(char *&dest, char *data, int len) {
    len += 1;
    saveint(dest, len);
    savebytes(dest, data, len-1);
    savebyte(dest, '\0');
    return sizeof(len) + len;
}
inline int saveboolvector(char *&dest, vector <bool> &vec){
    int count = vec.size();
    saveint(dest,count);
    for(int i = 0;i<count;i++){
        savebyte(dest,(byte)vec[i]);
    }
    return sizeof(count)+count*(sizeof(byte));
}
inline int savebytevector(char *&dest, vector <byte> &vec){
    int count = vec.size();
    saveint(dest,count);
    for(int i = 0;i<count;i++){
        savebyte(dest,vec[i]);
    }
    return sizeof(count)+count*(sizeof(byte));
}
inline int saveintvector(char *&dest, vector <int> &vec) {
    int count = vec.size();
    saveint(dest, count);
    for (int i = 0; i < count; i++) {
        saveint(dest, vec[i]);
    }
    return sizeof(count)+count*(sizeof(int));
}
inline int savelonglongvector(char *&dest, vector <long long> &vec) {
    int count = vec.size();
    saveint(dest, count);
    for (int i = 0; i < count; i++) {
        savelonglong(dest, vec[i]);
    }
    return sizeof(count)+count*(sizeof(long long));
}
inline int saveintlonglongmap(char *&dest, map <int, long long> &data) {
    int count = data.size();
    saveint(dest, count);
    for (map <int, long long>::iterator iter = data.begin(); iter != data.end(); iter++) {
        saveint(dest, iter->first);
        savelonglong(dest, iter->second);
    }
    return sizeof(count)+count*(sizeof(int)+sizeof(long long));
}
inline byte loadbyte(char *&src) {
    byte data = *(byte*)src;
    src += sizeof(data);
    return data;
}
inline char *loadbytes(char *&src, char *dest, int len) {
    memcpy(dest, src, len);
    src += len;
    return dest;
}
inline unsigned int loaduint(char *&src) {
    unsigned int data = *(unsigned int*)src;
    src += sizeof(data);
    return data;
}
inline int loadint(char *&src) {
    int data = *(int*)src;
    src += sizeof(data);
    return data;
}

inline int loadbytevector(char *&src, vector<byte> &vec) {
    vec.clear();
    int count = loadint(src);
    for(int i = 0 ;i<count;i++){
        vec.push_back(loadbyte(src));
    }   
    return count;
}
inline long long loadlonglong(char *&src) {
    long long data = *(long long*)src;
    src += sizeof(data);
    return data;
}
inline float loadfloat(char *&src) {
    float data = *(float*)src;
    src += sizeof(data);
    return data;
}
inline string loadstring(char *&src) {
    int len = loadint(src);
    string data = string(src);
    src += len;
    return data;
}
inline int loadintvector(char *&src, vector<int> &vec) {
    vec.clear();
    int count = loadint(src);
    for (int i=0; i<count; i++) {
        vec.push_back(loadint(src));
    }
    return count;
}
inline void loadlonglongvector(char *&src, vector<long long> &vec) {
    vec.clear();
    int count = loadint(src);
    for (int i=0; i<count; i++) {
        vec.push_back(loadlonglong(src));
    }
}
inline void loadintlonglongmap(char *&src, map<int, long long>& data) {
    data.clear();
    int count = loadint(src);
    for (int i=0; i<count; i++) {
        int puid = loadint(src);
        long long time = loadlonglong(src);
        data.insert(make_pair(puid, time));
    }
}

inline int sizebyte(byte data) {
    return sizeof(data);
}
inline int sizeint(int data) {
    return sizeof(data);
}
inline int sizelonglong(long long data) {
    return sizeof(data);
}
inline int sizefloat(float data) {
    return sizeof(data);
}
inline int sizestring(string &data) {
    return sizeof(int)+data.length()+1;
}
inline int sizeintvector(vector <int> &vec) {
    return sizeof(int)+sizeof(int)*vec.size();
}
inline int sizelonglongvector(vector <long long> &vec) {
    return sizeof(int)+sizeof(long long)*vec.size();
}
inline int sizeintlonglongmap(map <int, long long> &data) {
    return sizeof(int)+data.size()*(sizeof(int)+sizeof(long long));
}

#endif

