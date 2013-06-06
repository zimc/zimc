create table `user_messages`
(
    `id` int(10) NOT NULL AUTO_INCREMENT comment 'id',
    `user_id` int(10) NOT NULL comment 'user id',
    `messages` blob NOT NULL comment 'binary data',
    primary key(`id`),
    index(`user_id`)
)ENGINE=MyISAM DEFAULT CHARSET=gbk;
