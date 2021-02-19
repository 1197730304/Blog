#include<iostream>
#include<json/json.h>
#include<mysql/mysql.h>
#include<mutex>
#include<string>
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD "114514"
#define MYSQL_DB "db_blog1"
namespace blog_system
{
    static std::mutex g_mutex;
    //向外提供接口返回初始化的mysql句柄（连接服务器，选择数据库，设置字符集）
    MYSQL *MysqlInit()
    {
        MYSQL *mysql;
        //1.初始化数据库
        mysql=mysql_init(NULL);
        if(mysql==NULL)
        {
            printf("mysql init error\n");
            return NULL;
        }
        //2.连接服务器
        if(mysql_real_connect(mysql,MYSQL_HOST,MYSQL_USER,MYSQL_PSWD,NULL,0,NULL,0)==NULL)
        {
            printf("connect mysql server error:%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }

        //3.创建客户端字符集
        if(mysql_set_character_set(mysql,"utf8")!=0)
        {
            printf("set client character error:%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }

        //4.选择数据库
        if(mysql_select_db(mysql,MYSQL_DB)!=0)
        {
            printf("select db error:%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }
        return mysql;//返回数据库操作句柄
    }

    //销毁句柄
    void MysqlRelease(MYSQL *mysql)
    {
        if(mysql)
        {
            mysql_close(mysql);
        }
        return;
    }
    //执行语句的共有接口
    bool MysqlQuery(MYSQL *mysql,const char *sql)
    {
        int ret=mysql_query(mysql,sql);
        if(ret!=0)
        {
            printf("mysql query:[%s] error:%s\n",sql,mysql_error(mysql));
            return false;
        }
        return true;
    }
    class TableBlog
    {
        public:
            TableBlog(MYSQL *mysql)
             :_mysql(mysql)
              {
              }

           //从blog中取出博客信息，组织sql语句，将数据插入数据库
            bool Insert(Json::Value &blog)
            {
#define INSERT_BLOG "insert tb_blog values(null,'%d','%s','%s',now());"
                //tmp的长度必须动态分配，根据正文的长度来进行开空间
                int len=blog["content"].asString().size()+4096;
                char *tmp=(char*)malloc(len);
                sprintf(tmp,INSERT_BLOG,blog["tag_id"].asInt()
                ,blog["title"].asCString()
                ,blog["content"].asCString()
                );
                bool ret= MysqlQuery(_mysql,tmp);
                free(tmp);
                return ret;
            }
            //根据博客id删除博客
            bool Delete(int blog_id)
            {
#define DELETE_BLOG "delete from tb_blog where id=%d;"
                char tmp[1024]={0};
                sprintf(tmp,DELETE_BLOG,blog_id);
                bool ret= MysqlQuery(_mysql,tmp);
                return ret;
                
            }
            //从blog中取出博客信息，组织sql语句，更新数据库的数据
            //id  tag_id   title   content ctime
            bool Update(Json::Value &blog)
            {
#define UPDATE_BLOG "update tb_blog set tag_id=%d,title='%s',content='%s'where id=%d;"
                int len=blog["content"].asString().size()+4096;
                char *tmp=(char*)malloc(len);
                sprintf(tmp,UPDATE_BLOG,blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString(),blog["id"].asInt());
                bool ret= MysqlQuery(_mysql,tmp);
                free(tmp);
                return ret;
            
            }

            //通过blog返回所有的博客信息（因为通常是列表展示，因此不包含正文）
            bool GetAll(Json::Value *blogs)
            {
#define GETALL_BLOG "select id,tag_id,title,ctime from tb_blog;"
                char tmp[1024]={0};
                sprintf(tmp,GETALL_BLOG);
                //执行查询语句
                g_mutex.lock();
                bool ret=MysqlQuery(_mysql,tmp);
                if(ret==false)
                {
                    g_mutex.unlock();
                    return false;
                }

                //保存结果集
                MYSQL_RES *res=mysql_store_result(_mysql);
                g_mutex.unlock();
                if(res==NULL)
                {
                    printf("store all blog result failed:%s\n",mysql_error(_mysql));
                    return false;
                }
                //遍历结果集
                int row_num=mysql_num_rows(res);
                for(int i=0;i<row_num;i++)
                {
                    MYSQL_ROW row=mysql_fetch_row(res);//自动获取下一行的数据
                    Json::Value blog;
                    blog["id"]=std::stoi(row[0]);
                    blog["tag_id"]=std::stoi(row[1]);
                    blog["title"]=row[2];
                    blog["ctime"]=row[3];
                    blogs->append(blog);//向博客集合中添加新的数据

                }
                mysql_free_result(res);
                return ret;
            }

            //返回单个博客信息
            bool GetOne(Json::Value *blog)
            {
#define GETONE_BLOG "select tag_id,title,content,ctime from tb_blog where id=%d;"
                char tmp[1024]={0};
                sprintf(tmp,GETONE_BLOG,(*blog)["id"].asInt());
                g_mutex.lock();
                bool ret=MysqlQuery(_mysql,tmp);
                if(ret==false)
                {
                    g_mutex.unlock();
                    return false;
                }
              //保存结果集
                MYSQL_RES *res=mysql_store_result(_mysql);
                g_mutex.unlock();
                if(res==NULL)
                {
                    printf("store one blog result failed:%s\n",mysql_error(_mysql));
                    return false;
                }
                //遍历结果集
                int row_num=mysql_num_rows(res);
                if(row_num!=1)
                {
                    printf("get one blog result error:%s\n",mysql_error(_mysql));
                    mysql_free_result(res);
                    return false;
                }
                MYSQL_ROW row=mysql_fetch_row(res);
                (*blog)["tag_id"]=std::stoi(row[0]);
                (*blog)["title"]=row[1];
                (*blog)["content"]=row[2];
                (*blog)["ctime"]=row[3];
                mysql_free_result(res);
                return true;

            }
            ~TableBlog()
            {
                
            }
        private:
            MYSQL *_mysql;
    };

    class TableTag
    {
        public:

            TableTag(MYSQL *mysql)
            :_mysql(mysql)
            {
            }

            bool Insert(Json::Value &tag)
            {
#define INSERT_TAG "insert tb_tag values(null,'%s');"
                char tmp[1024]={0};
                sprintf(tmp,INSERT_TAG,tag["name"].asCString());
                return MysqlQuery(_mysql,tmp);
            }
            
            bool Delete(int tag_id)
            {
#define DELETE_TAG "delete from tb_tag where id=%d;"
            char tmp[1024]={0};
            sprintf(tmp,DELETE_TAG,tag_id);
            return MysqlQuery(_mysql,tmp);
            }

            bool Update(Json::Value &tag)
            {
#define UPDATE_TAG "update tb_tag set name='%s' where id=%d;"
                char tmp[1024]={0};
                sprintf(tmp,UPDATE_TAG,tag["name"].asCString(),tag["id"].asInt());
                return MysqlQuery(_mysql,tmp);

            }
            bool GetAll(Json::Value *tags)
            {
#define GETALL_TAG "select id,name from tb_tag;"

            //执行语句
            g_mutex.lock();//加锁防止多线程中访问同一句柄
            bool ret=  MysqlQuery(_mysql,GETALL_TAG);
            if(ret==false)
            {
                g_mutex.unlock();
                return false;
            }
            //获取结果集
            MYSQL_RES *res=mysql_store_result(_mysql);
            g_mutex.unlock();
            if(res==NULL)
            {
                printf("store all tag result error:%s\n",mysql_error(_mysql));
                return false;
            }
            //遍历结果集
            int num_row=mysql_num_rows(res);
            for(int i=0;i<num_row;i++)
            {
                MYSQL_ROW row=mysql_fetch_row(res);//每次获取新的一行
                Json::Value tag;
                tag["id"]=std::stoi(row[0]);
                tag["name"]=row[1];
                tags->append(tag);
            }
            mysql_free_result(res);
            return true;

            }
            bool GetOne(Json::Value *tag)
            {
#define GETONE_TAG "select name from tb_tag where id=%d;"
                char tmp[1024]={0};
                sprintf(tmp,GETONE_TAG,(*tag)["id"].asInt());
                g_mutex.lock();
                bool ret=MysqlQuery(_mysql,tmp);
                if(ret==false)
                {
                    g_mutex.unlock();
                    return false;
                }
                //获取结果集
                MYSQL_RES *res=mysql_store_result(_mysql);
                g_mutex.unlock();
                if(res==NULL)
                {
                    printf("store one tag result error:%s\n",mysql_error(_mysql));
                    return false;
                }
                //遍历结果集
                int row_num=mysql_num_rows(res);
                if(row_num!=1)
                {
                    printf("get one tag result error:%s\n",mysql_error(_mysql));
                    return false;
                }
                MYSQL_ROW row=mysql_fetch_row(res);
                (*tag)["name"]=row[0];
                mysql_free_result(res);
                return true;
            }
            ~TableTag()
            {
                    
            }
        private:
            MYSQL *_mysql;
    };
}
