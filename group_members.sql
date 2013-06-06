#create table `user_group_members` (
#`group_id` int(10) NOT NULL comment "group id",
#`user_id` int(10) NOT NULL comment "group's member id",
#index(`group_id`),
#index(`user_id`)
#)ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `user_group_members`;
CREATE TABLE `user_group_members` (
  `group_id` int(10) NOT NULL COMMENT 'group id',
  `user_id` int(10) NOT NULL COMMENT 'group''s member id',
  `type` int(11) DEFAULT '0',
  UNIQUE index `group_id_2` (`group_id`,`user_id`),
  index `group_id` (`group_id`),
  index `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

create table `user_talks_info` (
`talks_id` int(10) NOT NULL AUTO_INCREMENT COMMENT "talks id",
`name` varchar(128) NOT NULL comment "talks name",
`notice` varchar(256) NOT NULL comment "talks notice",
`headurl` varchar(256) NOT NULL comment "talks headurl,pic ",
primary key(`talks_id`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

create table `user_talks_members` (
`talks_id` int(10) NOT NULL comment "talks id",
`user_id` int(10) NOT NULL comment "talks's member id",
index(`talks_id`),
index(`user_id`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;
