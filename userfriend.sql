create table `user_friends` (
`user_id` int(10) NOT NULL comment "user id",
`friend_id` int(10) NOT NULL comment "friend id",
`type` varchar(256) NOT NULL comment "online",
index(`user_id`),
index(`friend_id`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;
