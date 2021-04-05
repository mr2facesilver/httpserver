// Program Instructions 
// Server main:
//      1) Create socket s
//      2) Set the socket options
//      3) Bind the socket s to a port (given on the command line)
//      4) Call the listen() function to have the OS queue connection requests
//      5)  Call accept() to accept a new connection
//           /* This will block. When a client connects, accept() will unblock 
//              and return a data socket. */
//      6) Create a thread that calls the Connection_hander function, passing 
//         it the data socket
//         /* use pthread_create(), ANSI C thrd_create(), or ANSI C++ thread 
//            objects */
//      7) go to step 5
     
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <fstream>
#include <regex>
#include <string>

#include <cstring>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <stdarg.h>
#include <resolv.h>
#include <fcntl.h>

#include <sstream>

#include <stdexcept>

#include <signal.h>
#include <sys/stat.h>

/* 
 extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
} 

*/

#include <pthread.h>

using namespace std;


void * ForConnections(void * intsock)
{

	int clisock = (long) intsock;
	printf("%ld \r\n" , (long) clisock);
	FILE *fp = fdopen(clisock, "r+");
	// now fp points to a stream and you can use the nice stream functions, such as fgets()

	char buff[1024];
	buff[0] = 0;
	fgets(buff, 1024, fp); //rec()
	string cmdsent=buff;
	cout<<cmdsent<<endl;

	//parse buf for file path 
	int hcount=0;
	bool done = false,keep_alivebool =false;
	string keep_alive="HTTP/1.1";
	string cmd= cmdsent.substr(0,4);
	cout<<cmd<<endl;


	int hold = 0;	
	char space =' ';
	for(int i = 4;i<1016;i++)
	{
		if(cmdsent.at(i)==space)
		{
			hold=i;
			break;
		}
	}
	string path= cmdsent.substr(4,hold-4);
	cout<<path<<endl;
	string HTMLVALUE = cmdsent.substr(hold+1);
	if(HTMLVALUE==keep_alive)
	{
		keep_alivebool=true;
	}
	cout<<HTMLVALUE<<endl;
	
	char header[1025];
	char colon =':';
	int numberofheads=2000;
	string allheaderslist[numberofheads];
	while(!done)
	{
		fgets(header, 1024, fp);
		string Curheader=header;
		cout<<Curheader<<endl;
		allheaderslist[hcount]=header;
		hcount++;
		string value;
		string key;
		int headerDel=3;
		int headerDel2=3;
		if(header[0]==13) 
		{
			done = true;
		}
		else
		{
			for(int i = 0;i<1016;i++)
			{
				if(Curheader.at(i)==colon)
				{
					headerDel=i;
					break;
				}
			}
			value=Curheader.substr(headerDel+2);
			key=Curheader.substr(0,headerDel);
			if(key=="Connection" && value=="keep-alive")
			{
				keep_alivebool=true;cout<< "i got to keep alive"<<endl;
			}
		
		}
	}
	cout<<"-------------"<<path<<"----------------------"<<endl;
	string sidepath =path;
	stringstream ss(sidepath);
	char val [1024];
	ss.getline(val,1024,'?');
	string mylua=val;
	int position=0;
	position=mylua.find(".lua");
if(position>0)
{
	int ret, result, i;
    double sum;
	cout<<"do stuff"<<endl;
	ss.getline(val,1024,'%');
	string name =val;
	ss.getline(val,1024,'%');
	string age =val;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	ret = luaL_loadfile(L, "showdata.lua");
	lua_newtable(L);  
    lua_pushstring(L, "name");
    lua_pushstring(L, name);
	lua_pushstring(L, "age");
    lua_pushstring(L, age);
	lua_rawset(L, -5);
	lua_setglobal(L, "REQUEST"); 
	int *ud = (int *) lua_newuserdata(L, sizeof(int));
    *ud = 1; 
	lua_pushcclosure(L, c_print, 1);
	lua_pcall(L, 0, 1, 0);
	lua_pop(L, 1);  /* Take the returned value out of the stack */
    lua_close(L);   /* Adios, Lua */


}
else
{
	char cwd[500];   // limit directory path to 500 chars in length
	getcwd(cwd, 500);
	string full_path = cwd;
	full_path.append(path);
	fstream inf(full_path.c_str(), ios::in | ios::binary);
	int beg = inf.tellg();
	inf.seekg(0, ios::end);
	int len = inf.tellg();
	inf.seekg(beg, ios::beg);
	char bufk[] =  "HTTP/1.1 200 OK\r\n";
	fwrite(bufk, strlen(bufk), 1, fp);
	char bufh[4096];
	sprintf(bufh, "%s: %d\r\n", "Content-Length", len);
	fwrite(bufh, strlen(bufh), 1, fp);
	fwrite("\r\n", 2, 1, fp);
	inf.read(bufh, 4096);
	int countnow = inf.gcount();
	while (countnow > 0) 
	{
		fwrite(bufh, countnow, 1, fp);
		inf.read(bufh, 4096);
		countnow = inf.gcount();
	}
	inf.close();
}
	
 fclose(fp);  
 return 0;
}





int main(int argc, char const * argv[])
{
	int numberofthreads =10;
	pthread_t thread;
	int clientarray;


	int currsocket, newestsocket; 
	unsigned int lenofclient;
	int port = atoi(argv[1]);

	//creation of the socket 
	struct sockaddr_in serverAddres;
	struct sockaddr_in clientAddres;
	if((currsocket = socket(AF_INET,SOCK_STREAM, 0))< 0)
	{
		exit(1);
	}
	
	//int setsockopt(int sockets, int level, int optname, const void *optval, socklen_t optler);
	//set the socket options


		memset((void *) &serverAddres, 0, sizeof(serverAddres));
		serverAddres.sin_family = AF_INET;
		serverAddres.sin_addr.s_addr=htonl(INADDR_ANY);
		serverAddres.sin_port = htons(port);

	int val = 1;
  	setsockopt(currsocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val);




	//bind the socket s to a port
	//int bind(int sockets, const struct scockaddr *aadr, socklen_t addrlen);
	int b = bind(currsocket, (struct sockaddr *) & serverAddres, sizeof(serverAddres));
	if(b < 0)
	{
		exit(1);
	}
	
//create listen function
	listen(currsocket, 10);

	while(1) 
	{

		lenofclient= sizeof(clientAddres);
		newestsocket = accept(currsocket, (struct sockaddr *) &clientAddres, &lenofclient);
		if(newestsocket < 0) 
		{
			exit(1);
		}
		//ForConnections(newestsocket);
		//Pthreads Creation
		//printf("%ld" , (long) newestsocket);
		void * myvoid = (void *) (long)newestsocket;
		if (pthread_create(&thread, NULL, ForConnections, myvoid ))
		{
			printf("Error:unable to create thread,\n");
			exit(-1);

		}

	}


	pthread_join(thread,NULL);
	return 1;
}





