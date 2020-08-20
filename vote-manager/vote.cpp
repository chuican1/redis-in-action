
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <hiredis/hiredis.h>
#include <ctime>
#include <string>

//#include "gtest/gtest.h"

using namespace std;


const unsigned long ONE_WEEK_IN_SECONDS = 7 * 86400;
const unsigned int VOTE_SCORE = 432;


int post_articlt(redisContext* conn,string user,string title,string link)
{
	//获取当前文章id
	redisReply* r = (redisReply* )redisCommand(conn,"incr article:");
	if( NULL == r)
	{
		printf("get article seq failure\n");
		redisFree(conn);
		return -1;
	}

	//当前文章已投票用户
	int article_id = r->integer;
	string voted("voted:");
	voted = voted + to_string(article_id);
	string command("sadd "); // sadd voted:id user
	command = command + voted + " " + user;
	r = (redisReply* )redisCommand(conn,(const char*)command.c_str());
	if( NULL == r)
	{
		printf("add user into Voted Set failure\n");
		redisFree(conn);
		return -1;
	}

	//expire 
	command = "expire " + voted + " " + to_string(ONE_WEEK_IN_SECONDS);
	r = (redisReply* )redisCommand(conn,(const char*)command.c_str());
	if( NULL == r)
	{
		printf("add user into Voted Set failure\n");
		redisFree(conn);
		return -1;
	}

	time_t now = time(0);
	string article = "article:" + article_id;
	command = "hmset " +  article;
	command = command + "title:" + title;
	command = command + "link:" + link;
	command = command + "poster:" + user;
	command = command + "time:" + to_string(now);
	command = command + "votes:" + "1";
	r = (redisReply* )redisCommand(conn,(const char*)command.c_str());
	if( NULL == r)
	{
		printf("hash for each article\n");
		redisFree(conn);
		return -1;
	}

	return article_id;
}
void doTest()
{
	//redis默认监听端口为6387 可以再配置文件中修改
	redisContext* c = redisConnect("127.0.0.1", 6379);
	if ( c->err)
	{
		redisFree(c);
		printf("Connect to redisServer faile\n");
		return ;
	}
	printf("Connect to redisServer Success\n");
	
	const char* command1 = "set stest1 value1";
	redisReply* r = (redisReply*)redisCommand(c, command1);
	
	if( NULL == r)
	{
		printf("Execut command1 failure\n");
		redisFree(c);
		return;
	}
	if( !(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK")==0))
	{
		printf("Failed to execute command[%s]\n",command1);
		freeReplyObject(r);
		redisFree(c);
		return;
	}	
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command1);
	
	const char* command2 = "strlen stest1";
	r = (redisReply*)redisCommand(c, command2);
	if ( r->type != REDIS_REPLY_INTEGER)
	{
		printf("Failed to execute command[%s]\n",command2);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	int length =  r->integer;
	freeReplyObject(r);
	printf("The length of 'stest1' is %d.\n", length);
	printf("Succeed to execute command[%s]\n", command2);
	
	
	const char* command3 = "get stest1";
	r = (redisReply*)redisCommand(c, command3);
	if ( r->type != REDIS_REPLY_STRING)
	{
		printf("Failed to execute command[%s]\n",command3);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	printf("The value of 'stest1' is %s\n", r->str);
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command3);
	
	const char* command4 = "get stest2";
	r = (redisReply*)redisCommand(c, command4);
	if ( r->type != REDIS_REPLY_NIL)
	{
		printf("Failed to execute command[%s]\n",command4);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command4);	
	
	
	redisFree(c);
	
}
 
int main()
{
	
	redisContext* c = redisConnect("127.0.0.1", 6379);
	if ( c->err)
	{
		redisFree(c);
		printf("Connect to redisServer faile\n");
		return 0;
	}
	printf("Connect to redisServer Success\n");

	int article_id = post_articlt(c,"username","a title","http://www.example.com");
		printf("We posted a new article with id: %d\n",article_id);
	return 0;
}

/*TEST(REDIS_VOTE,VOTE)
{
	redisContext* c = redisConnect("127.0.0.1", 6379);
	if ( c->err)
	{
		redisFree(c);
		printf("Connect to redisServer faile\n");
		return ;

		int article_id = post_articlt(c,"username","a title","http://www.example.com");
		printf("We posted a new article with id: %d\n",article_id);
	}
}*/