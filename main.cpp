#include "db.hpp"
#include "httplib.h"
using namespace httplib;
void test()
{

    MYSQL *mysql=blog_system::MysqlInit();
    blog_system::TableBlog table_blog(mysql);
    Json::Value blog;
    blog["id"]=3;
    // blog["title"]="JAVA是语言";
    //  blog["tag_id"]=2;
    //  blog["content"]="java语言是最牛逼的语言";
    //table_blog.Insert(blog);
    //table_blog.Delete(2);
    //table_blog.Update(blog);
      
    table_blog.GetOne(&blog);
   Json::StyledWriter writer;
   std::cout<<writer.write(blog)<<std::endl;
   



    blog_system::TableTag table_tag(mysql);

    //Json::Value tag;
    //tag["name"]="php";
   // tag["id"]=2;
    //table_tag.Insert(tag);
   // table_tag.Delete(3);
   //table_tag.Update(tag);
//     table_tag.GetOne(&tag);
//    Json::StyledWriter writer;
//    std::cout<<writer.write(tag)<<std::endl;
}

blog_system::TableBlog *table_blog;
blog_system::TableTag *table_tag;
void InsertBlog(const Request &req,Response &rsp)
{
    //从请求中取出正文--正文就是提交的博客信息，以json格式字符串组织的
    req.body;
    //将json格式的字符串进行反序列化，得到各个博客的信息
    Json::Reader reader;
    Json::Value blog;
    Json::FastWriter writer;
    Json::Value errmsg;
    bool ret=reader.parse(req.body,blog);
   if(ret==false)
    {
        printf("InsertBlog parse blog json failed!\n");
        rsp.status=400;
        errmsg["ok"]=false;
        errmsg["reason"]="parse blog json failed";
        rsp.set_content(writer.write(errmsg),"application/json");//添加正文rsp.body
        return;
    } 
    //将得到的博客信息插入到数据库中
    ret=table_blog->Insert(blog);
    if(ret==false)
    {
        printf("InsertBlog insert into database failed!\n");
        rsp.status=500;
        return;
    } 
    rsp.status=200;
    return;
}
void DeleteBlog(const Request &req,Response &rsp)
{
    // /blog/123  /blog(\d+)  req.matches[0]=/blog/123   req.matches[1]=123;
    int blog_id=std::stoi(req.matches[1]);
    bool ret=table_blog->Delete(blog_id);
    if(ret==false)
    {
        printf("DeleteBlog delete from database failed!\n");
        rsp.status=500;
        return;
    } 
    rsp.status=200;
    return;
}
void UpdateBlog(const Request &req,Response &rsp)
{
    int blog_id=std::stoi(req.matches[1]);
    Json::Value blog;
    Json::Reader reader;
    bool ret=reader.parse(req.body,blog);
    if(ret=false)
    {
        printf("UpdateBlog parse json failed!\n");
        rsp.status=400;
        return;
    }
    blog["id"]=blog_id;
    ret=table_blog->Update(blog);
    if(ret==false)
    {
        printf("UpdateBlog update database failed!\n");
        rsp.status=500;
        return;
    } 
    rsp.status=200;


    return;
}
void GetAllBlog(const Request &req,Response &rsp)
{
    //从数据库中取出博客列表数据
    Json::Value blogs;
    bool ret=table_blog->GetAll(&blogs);
    if(ret==false)
    {
        printf("GetAllBlog select from database failed!\n");
        rsp.status=500;
        return;
    }

    //将数据进行json序列化，添加到rsp正文中
    Json::FastWriter writer;
    rsp.set_content(writer.write(blogs),"application/json");
    rsp.status=200;
    
    return;

}
void GetOneBlog(const Request &req,Response &rsp)
{
    Json::Value blog;
    int blog_id=std::stoi(req.matches[1]);
    blog["id"]=blog_id;
    bool ret=table_blog->GetOne(&blog);
    if(ret==false)
    {
        printf("GetOneBlog select from database failed!\n");
        rsp.status=500;
        return;
    }

    //将数据进行json序列化，添加到rsp正文中
    Json::FastWriter writer;
    rsp.set_content(writer.write(blog),"application/json");
    rsp.status=200;
    
    return;
}

void InsertTag(const Request &req,Response &rsp)
{
    //从请求中取出正文--正文就是提交的博客信息，以json格式字符串组织的
    req.body;
    //将json格式的字符串进行反序列化，得到各个博客的信息
    Json::Reader reader;
    Json::Value tag;
    bool ret=reader.parse(req.body,tag);
   if(ret==false)
    {
        printf("InsertTag parse blog json failed!\n");
        rsp.status=400;
        return;
    } 
    //将得到的博客信息插入到数据库中
    ret=table_tag->Insert(tag);
    if(ret==false)
    {
        printf("InsertTag insert into database failed!\n");
        rsp.status=500;
        return;
    } 
    rsp.status=200;
    return;
}
void DeleteTag(const Request &req,Response &rsp)
{
    // /blog/123  /blog(\d+)  req.matches[0]=/blog/123   req.matches[1]=123;
    int tag_id=std::stoi(req.matches[1]);
    bool ret=table_tag->Delete(tag_id);
    if(ret==false)
    {
        printf("DeleteTag delete from database failed!\n");
        rsp.status=500;
        return;
    } 
    rsp.status=200;
    return;
}

void UpdateTag(const Request &req,Response &rsp)
{
    int tag_id=std::stoi(req.matches[1]);
    Json::Value tag;
    Json::Reader reader;
    bool ret=reader.parse(req.body,tag);
    if(ret=false)
    {
        printf("UpdateTag parse json failed!\n");
        rsp.status=400;
        return;
    }
    tag["id"]=tag_id;
    ret=table_tag->Update(tag);
    if(ret==false)
    {
        printf("UpdateTag update database failed!\n");
        rsp.status=500;
        return;
    } 
    rsp.status=200;


    return;
}

void GetAllTag(const Request &req,Response &rsp)
{
    //从数据库中取出博客列表数据
    Json::Value tags;
    bool ret=table_tag->GetAll(&tags);
    if(ret==false)
    {
        printf("GetAllTag select from database failed!\n");
        rsp.status=500;
        return;
    }

    //将数据进行json序列化，添加到rsp正文中
    Json::FastWriter writer;
    rsp.set_content(writer.write(tags),"application/json");
    rsp.status=200;
    
    return;

}

void GetOneTag(const Request &req,Response &rsp)
{
    Json::Value tag;
    int tag_id=std::stoi(req.matches[1]);
    tag["id"]=tag_id;
    bool ret=table_tag->GetOne(&tag);
    if(ret==false)
    {
        printf("GetOneTag select from database failed!\n");
        rsp.status=500;
        return;
    }

    //将数据进行json序列化，添加到rsp正文中
    Json::FastWriter writer;
    rsp.set_content(writer.write(tag),"application/json");
    rsp.status=200;
    
    return;
}
int main()
{
    MYSQL *mysql=blog_system::MysqlInit();
    table_blog=new blog_system::TableBlog(mysql);
    table_tag=new blog_system::TableTag(mysql);
    //业务处理模块 
   Server server;
    //设置相对根目录的目的：当客户端请求静态文件资源时，httplib会直接根据路径读取数据并直接响应
    server.set_base_dir("./www");//设置url中资源的相对根目录，并在请求/的时候自动加上index.html
    //网络通信--搭建http服务器
    //博客信息的增删改查
    server.Post("/blog",InsertBlog);
    //正则表达式： \d--匹配数字字符  +---表示匹配前面的字符一次或多次  () 表示临时保存的数据
    server.Delete(R"(/blog/(\d+))",DeleteBlog);//R"()"去除所有的特殊含义
    server.Put(R"(/blog/(\d+))",UpdateBlog);
    server.Get("/blog",GetAllBlog);
    server.Get(R"(/blog/(\d+))",GetOneBlog);
    //标签信息的增删改查

    server.Post("/tag",InsertTag);
    server.Delete(R"(/tag/(\d+))",DeleteTag);//R"()"去除所有的特殊含义
    server.Put(R"(/tag/(\d+))",UpdateTag);
    server.Get("/tag",GetAllTag);
    server.Get(R"(/tag/(\d+))",GetOneTag);
    
    server.listen("0.0.0.0",2580);
    
    
    
    return 0;
}
