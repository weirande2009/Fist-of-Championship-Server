drop table if exists Users;
create table Users(
    userid char(8) not null primary key,
    username char(32) not null,
    passwd char(128) not null
);

create table Friends(
    user1_id char(8) not null,
    user2_id char(8) not null
);

create table WaitFriends(
    user1_id char(8) not null,
    user2_id char(8) not null
);


insert into Users value ('00000001', '户英豪nb', '123456');
insert into Users value ('00000002', '黄政nb', '123456');
insert into Friends value ('00000001', '000000002');
