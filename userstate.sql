create table `user_state`
(
    `user_id` int(10) NOT NULL COMMENT "user id",
    `state` int(10) NOT NULL DEFAULT "0" comment "user login state",
    `invited` int(10) NOT NULL DEFAULT "0" COMMENT "invited set",
    primary key (`user_id`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;
