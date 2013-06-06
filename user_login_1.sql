DELIMITER //
create procedure `user_login_1`(IN userName varchar(20))
BEGIN
declare vuid int(20);
declare vname varchar(20);
declare vpwd varchar(20);
declare vpwd_key varchar(20);
select `user_id`,`user_name`,`user_pwd`,`user_pwd_key` into vuid,vname,vpwd,vpwd_key from `are_sys_user` where `user_name`=userName limit 0,1;
select vuid,vname,vpwd,vpwd_key;
End
//
