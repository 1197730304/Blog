create database if not exists db_blog1;
use db_blog1;
drop table if exists tb_tag;
create table if not exists tb_tag( id int primary key auto_increment comment '标签ID',name varchar(32) comment '标签名称');

drop table if exists tb_blog;
create table if not exists tb_blog(id int primary key auto_increment comment'博客ID',tag_id int comment'所属标签ID',
title varchar(255) comment '博客标题',content text comment'博客内容',ctime datetime comment '博客的创建时间');

insert tb_tag values(null,'C++'),(null,'JAVA'),(null,'Linux');
insert tb_blog values(null,1,'这是一个C++博客','##C++是面向对象的高级语言',now()),
(null,1,'这是一个JAVA博客','##Java是最好的语言',now()),
(null,1,'这是一个Linux博客','##Linux是最好的开发语言',now());

