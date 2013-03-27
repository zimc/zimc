#ifndef __MSG_INTERFACE_H__
#define __MSG_INTERFACE_H__
#include "dbstruct.h"
#include "json_util.h"

void buildDBUserJson(Value &json, DBUser &dbuser);
Value buildFriendsJson(vector <DBFriend> &dbfriends);
Value buildGroupJson(DBGroup &dbgroup);
Value buildGroupsJson(vector <DBGroup> &dbgroups);
Value buildTalksJson(vector <DBTalks> &dbtalks);
void buildDBInterfaceJson(Value &json, DBInterface &dbinterface);

void parse_user(Value &json, DBUser &dbuser);
int parse_dbinterface(Value &json, DBInterface &dbinterface);
int parse_group(Value &json, DBGroup &dbgroup);



#endif //

