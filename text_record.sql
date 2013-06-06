create table `text_record`(
    `id` int(10) NOT NULL AUTO_INCREMENT COMMENT 'id',
    `send_uid` varchar(256) NOT NULL COMMENT 'sender',
    `recv_uid` varchar(256) NOT NULL COMMENT 'reciever',
    `type` int(10) NOT NULL COMMENT '0:users, 1:group,talks',
    `recv_id` int(10) NOT NULL comment 'reciever id',
    `ts` TIMESTAMP comment 'text timestamp',
    `text` varchar(2048) NOT NULL COMMENT 'text, encode by base64',
    PRIMARY KEY(`id`),
    index(`send_uid`),
    index(`recv_uid`),
    index(`ts`)
)ENGINE=MyISAM DEFAULT CHARSET=gbk;
