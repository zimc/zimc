#ifndef __UTILS_2_H__
#define __UTILS_2_H__

#include <stddef.h>
#include <time.h>
#include <string>

using namespace std;

/**
* @brief string to long int
*
* @param str pointer to the string
* @param is_succ pointer to a int to mark whether the convert is succussful. if success, *is_succ will be set to 1, \n
* if errors occurred, *is_succ will be set to 0. you can pass NULL if you don't care whether it's successful.
* @return the number converted from this string
*/
long hl_atol(const char *str, int *is_succ);

/**
* @brief long int to string
*
* @param num the number to be converted
* @return the string converted from this number, "NaN" if failed (means "Not a Number")
*/
string hl_ltostring(long num);

/**
* @brief string to double
*
* @param str pointer to the string
* @param is_succ pointer to a int to mark whether the convert is succussful. if success, *is_succ will be set to 1, \n
* if errors occurred, *is_succ will be set to 0. you can pass NULL if you don't care whether it's successful.
* @return the number converted from this string
*/
double hl_atod(const char *str, int *is_succ);

/**
* @brief double to string
*
* @param num the number to be converted
* @return the string converted from this number, "NaN" if failed (means "Not a Number")
*/
string hl_dtostring(double num);

/**
* @brief get a timestamp of current time (microseconds)
*
* @return the timestamp
*/
long long hl_timestamp(void);

/**
* @brief fill a struct time_val from a timestamp (microseconds)
*
* @param tv pointer to struct time_val
* @param timestamp timestamp
* @return tv will returned
*/
struct timeval *hl_ts2timeval(struct timeval *tv, long long timestamp);

/** @} */

//base64
//use : in, inlen, out, outlen,  outlen>=inlen * 4 + 1
void base64_encode(const char *in, const int in_len, char *out, int out_len);
std::string base64_decode(std::string const& encoded_string);


int getFileSuffixNum(time_t);

void httpCompelte(string &strurl, string &filename);

void check_file_path(string &filepath);

void createDirectory(const char *filepath);

#endif //__UTILS_2_H__

