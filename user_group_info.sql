create table `user_group_info` (
`group_id` int(10) NOT NULL AUTO_INCREMENT COMMENT "group id",
`name` varchar(128) NOT NULL comment "group name",
`notice` varchar(256) NOT NULL comment "group notice",
`headurl` varchar(256) NOT NULL comment "group headurl,pic ",
`user_id` int(10) NOT NULL comment "admin id",
`create_time` int(10) NOT NULL COMMENT 'create time, sec',
`invalid` int(10) NOT NULL DEFAULT '0',
primary key(`group_id`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;
