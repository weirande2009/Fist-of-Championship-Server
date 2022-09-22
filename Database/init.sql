drop table if exists Users;
create table Users(
    userid char(8) not null primary key,
    username char(32) not null,
    passwd char(16) not null
);

drop table if exists Friends;
create table Friends(
    user1_id char(8) not null,
    user2_id char(8) not null
);

drop table if exists WaitFriends;
create table WaitFriends(
    user1_id char(8) not null,
    user2_id char(8) not null
);

insert into Users value ('00000000', 'weiran1', '123456');
insert into Users value ('00000001', 'weiran2', '123456');
insert into Friends value ('00000000', '00000001');
insert into Friends value ('00000000', '00000002');

