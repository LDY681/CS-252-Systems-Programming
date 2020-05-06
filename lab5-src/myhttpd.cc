#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

const char * usage =
"                                                               \n"
"HTTP Server:                                                   \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   myhttpd [-f|-t|-p]  [<port>]		                	    \n"
"                                                               \n"
"	Where 1024 < port < 65536.                                  \n"
"                                                               \n"
"	Flags -f -t -p are optional                                 \n"
"                                                               \n"
"	Will return the document requested							\n";

const char * error =
"                                                               \n"
"Could not find the specified URL.                              \n"
"                                                               \n";

const char * titleAndheader = 
"<html>															\n"
"  <head>														\n"
"    <title>Directory Listing</title>                           \n"
"  </head>														\n"
"																\n"
"  <body>														\n"
"    <h1>Directory Listing</h1>									\n"
"																\n"
"    <ul>														\n";

const char* footer =

"    </ul>                                                      \n"
"																\n"
"    <hr>														\n"
"  </body>														\n"
"</html>														\n";


bool cgi = false;

int QueueLength = 5;

const char *secret_key = "681";



//Helper function for threads
void * loopthread(void * masterSocket);
void * eachthread(void * slaveSocket);


//mutex for poolthread
pthread_mutex_t mutex;

// Processes  request
void processRequest(int socket);

//Directory Listing
void directoryListing(char* filePath, int socket);

struct sockaddr_in clientIPAddress;
int alen;

char *docType;	//what type is this extension

char * getFilePath(int socket);

bool key_check = true;

bool plain = false;
int port;
bool indir = false;
extern "C" void killZombies(int sig) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char ** argv)
{
	struct sigaction sig;
	sig.sa_handler = &killZombies;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	int err = sigaction(SIGCHLD, &sig, NULL);
	if (err) {
		perror("sigaction");
		exit(-1);
	}

	
	char flag = 'i';	// i == iterative, f = new process, t = new thread, p = pool of threads
	// Get port from arguments
	if (argc < 2) {
		fprintf(stderr, "%s", usage);
		exit(-1);
	}
	else if (argc == 2) {
		port = atoi(argv[1]);
	}
	else if (argc == 3) {	//if there is a flag
		port = atoi(argv[2]);
		flag = argv[1][1];
	}
	else {
		fprintf(stderr, "%s", usage);
		exit(-1);
	}

	//printf("Flag is %c\n", flag);

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress;
	memset(&serverIPAddress, 0, sizeof(serverIPAddress));
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short)port);

	// Allocate a socket
	int masterSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (masterSocket < 0) {
		perror("socket");
		exit(-1);
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1;
	err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR,
		(char *)&optval, sizeof(int));

	// Bind the socket to the IP address and port
	int error1 = bind(masterSocket,
		(struct sockaddr *)&serverIPAddress,
		sizeof(serverIPAddress));
	if (error1) {
		perror("bind");
		exit(-1);
	}

	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error1 = listen(masterSocket, QueueLength);
	if (error1) {
		perror("listen");
		exit(-1);
	}

	// i == iterative, f = new process, t = new thread, p = pool of threads
	switch (flag) {

		case 'i':
			while (1) {
				struct sockaddr_in clientIPAddress;
				//int alen = sizeof(clientIPAddress);
				int slaveSocket = accept(masterSocket,
					(struct sockaddr *)&clientIPAddress,
					(socklen_t*)&alen);

				//Server doesn't exit when error occur
				if (slaveSocket < 0) {
					perror("accept");
					continue;	//continue the while loop, dont execute below
				}

				// Process request.
				processRequest(slaveSocket);

				// Close socket
				close(slaveSocket);
			}
		break;

		case 'f':
			while (1) {	
				struct sockaddr_in clientIPAddress;
				int slaveSocket = accept(masterSocket,
					(struct sockaddr *)&clientIPAddress,
					(socklen_t*)&alen);

				//Server doesn't exit when error occur
				if (slaveSocket < 0) {
					perror("accept");
					continue;	//continue the while loop, dont execute below
				}

				int ret = fork();

				if (ret == 0) {	//in child process
					processRequest(slaveSocket);

					// Close socket
					//close(slaveSocket);
					exit(0);
				}
				close(slaveSocket);	//close again in case request is still coming	

			}
		break;

		case 't':
			while (1) {	
				struct sockaddr_in clientIPAddress;
				//int alen = sizeof(clientIPAddress);
				int slaveSocket = accept(masterSocket,
					(struct sockaddr *)&clientIPAddress,
					(socklen_t*)&alen);

				//Server doesn't exit when error occur
				if (slaveSocket < 0) {
					perror("accept");
					continue;	//continue the while loop, dont execute below
				}

				pthread_t newthread;
				//pass only one thread, so &eachthread
				if (pthread_create(&newthread, NULL, &eachthread, (void *)slaveSocket)) {
					perror("newthread");
					exit(0);
				}

				//close(slaveSocket);
			}
		break;

		case 'p':
			pthread_t poolthread[5];
			pthread_mutex_init(&mutex, NULL);
			for (int i = 0; i < 4; i++) {

				if (pthread_create(&poolthread[i], NULL, loopthread, (void *)masterSocket)) {
					perror("poolthread");
					exit(0);
				}	
			}
			loopthread((void *)masterSocket);	//join the last one
		break;
	}
}

void * eachthread(void * slaveSocket) {
	int socket = (intptr_t)slaveSocket;
	processRequest(socket);

	// Close socket
	close(socket);
}

void * loopthread(void * masterSocket) {
	int socket = (intptr_t)masterSocket;	//pointer to int = intptr_t
	
	while (1) {
		struct sockaddr_in clientIPAddress;
		int alen = sizeof(clientIPAddress);	

		pthread_mutex_lock(&mutex);
		int slaveSocket = accept(socket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
		pthread_mutex_unlock(&mutex);

		processRequest(slaveSocket);

		close(slaveSocket);
	
	}
}

char * getFilePath(int socket) {
	cgi = false;
	//printf("getFilePath: Enter getFilePath Function\n");
	//if you visit http://data.cs.purdue.edu:<port>/681/file.c4d
	//It will send request as GET /681/file.c4d HTTP/1.1

	const int MaxLength = 1024;
	unsigned char newChar = 0;
	char *keycheck = (char*)malloc(4);
	//memset set fill a block of memory with a particular value 
	memset(keycheck, '\0', 4);

	int n = 0;

	//Read "GET "
	while (n = read(socket, &newChar, sizeof(newChar)) && newChar != ' ') {}
	if (n < 0) {
		perror("Read failure");
		exit(0);
	}

	//Read /681
	n = read(socket, &newChar, sizeof(newChar));
	int i;
	for (i = 0; i < 3; i++) {
		n = read(socket, &newChar, sizeof(newChar));
		keycheck[i] = newChar;
	}
	if (n < 0) {
		perror("read failure");
		exit(0);
	}

	//printf("input key is %s\n", keycheck);
	if (strcmp(keycheck, "681") != 0) {
		key_check = false;
		return NULL;
	}
	else {
		key_check = true;
	}

	//Get filepath and also file extension
	char *filePath = (char*)malloc(MaxLength + 1);
	char *fileExt = (char*)malloc(MaxLength + 1);
	memset(fileExt, '\0', MaxLength + 1);
	memset(filePath, '\0', MaxLength + 1);

	i = 0;
	int j = 0;	//store extension (
	bool ext = false;
	//cgi = false;
	while (n = read(socket, &newChar, sizeof(newChar)) && newChar != ' ') {

		if (newChar == '.') {
			ext = true;
		}
		if (ext == true) {
			fileExt[j] = newChar;
			j++;
		}
		filePath[i] = newChar;
		i++;
	
	}


	//Case CGI-BIN
	if (strstr(filePath, "/cgi-bin")) {
		cgi = true;
	}

	if (n < 0) {
		perror("read failure");
		exit(0);
	}


	//printf("getFilePath: file extension is %s\n", fileExt);
	if (!strcmp(fileExt, ".html")) {	//html
		docType = strdup("text/html");
	}
	else if (!strcmp(fileExt, ".gif")){	//gif
		docType = strdup("image/gif");
	}
	else if (!strcmp(fileExt, ".jpg")) {	//jpg
		docType = strdup("image/jpeg");
	}
	else if (!strcmp(fileExt, ".png")) {	//png
		docType = strdup("image/png");
	}
	else if (!strcmp(fileExt, ".svg")) {	//svg
		docType = strdup("image/svg+xml");
	}
	else {
		docType = strdup("text/html");	
		plain = true;
	}


	//Now we read the whole filepath
	//Read until find \13\10\13\10 so no request is left
	unsigned char lastChar = 0;
	while (n = read(socket, &newChar, sizeof(newChar))) {
		if (lastChar == '\015' && newChar == '\012') {

			n = read(socket, &newChar, sizeof(newChar));
			lastChar = '\012';

			if (newChar == '\015') {
				lastChar = newChar;
				n = read(socket, &newChar, sizeof(newChar));

				if (newChar == '\012') {
					break;	//Done!
				}
			}
		}
		lastChar = newChar;
	}

	//printf("Filepath quickfix %s\n", filePath);
	return filePath;
}

void processRequest(int socket)
{
	//Reset environmental Var
	setenv("QUERY_STRING", "", 1);
	//printf("processRequest: Enter processRequest function\n");

	char *cwd = (char*)malloc(1024);
	if (getcwd(cwd, 1024) == NULL) {
		perror("getcwd() error");
	}
     //printf("current working directory is: %s\n", cwd);
  	

	//Get requested path
	char *requestedPath = getFilePath(socket);

	printf("File after getfilepath() is %s\n", requestedPath);
	//Case CGI-BIN (TODO: Case, sort of directory listing )
	if (cgi == true) {
		pid_t pid = fork();

		if (pid == 0) {	//In child process

			//char *cgipath = (char*)malloc(200);
			//memset(cgipath, '\0', 200);

			//int z = 0;
			//while (n = read(socket, &newChar, sizeof(newChar)) && newChar != ' ') {
			//	cgipath[z] = newChar;
			//	z++;
			//}
			//printf("cgipath is %s\n", cgipath);

			char * filePath = (char*)malloc(1024);

			//char *trunc[2];
			//trunc[0] = strtok(requestedPath, "/");
			//trunc[1] = strtok(NULL, "\0");
			//printf("trunc[1] = %s\n", trunc[1]);

			char *exec[2];
			exec[0] = strtok(requestedPath, "?");	//Command before ?
			exec[1] = strtok(NULL, "\0");	//Condition after ?

			//Excluding directory listing case
			if (exec[1] != NULL) {
				setenv("REQUEST_METHOD", "GET", 1);
				setenv("QUERY_STRING", exec[1], 1);
			}
			//else {
			//	setenv("QUERY_STRING", envar, 1);
			//}

			printf("QUERY_STRING = %s\n", getenv("QUERY_STRING"));

			//if (strstr(requestedPath, "finger") != NULL) {
			//	exec[1] = NULL;
			//}

			//set cgi file path
			strcpy(filePath, cwd);
			strcat(filePath, "/http-root-dir");
			strcat(filePath, exec[0]);
			exec[0] = filePath;

			printf("exec[0] = %s: exec[1] = %s\n", exec[0], exec[1]);
			printf("CGI: final filepath is = %s\n", filePath);
			//Redirect output
			dup2(socket, STDOUT_FILENO);
			dup2(socket, STDERR_FILENO);
			//close(socket);

			//header
			write(socket, "HTTP/1.1 ", strlen("HTTP/1.1 "));
			write(socket, "200 ", strlen("200 "));
			write(socket, "Document ", strlen("Document "));
			write(socket, "follows", strlen("follows"));
			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "Server: ", strlen("Server: "));
			write(socket, "CS 252 Lab5", strlen("CS 252 Lab5"));
			write(socket, "\015\012", strlen("\015\012"));

			//execvp
			execvp(exec[0],exec);


			write(socket, "\015\012", strlen("\015\012"));

			cgi = false;
		}
		else {	//Parent Process
			waitpid(pid, NULL, 0);
		}
	}	//CGI == TRUE. 
	else {		//Case Normal, NO CGI
		if (key_check == false) {	//check for key
			close(socket);
			return;
		}
		//printf("processRequest: document type is %s\n", docType);
		printf("processRequest: Requested Path is %s\n", requestedPath);

		//Get file path
		char * filePath = (char*)malloc(1024);

		if (strcmp(requestedPath, "/") == 0) {
			printf("Case Home directory\n");
			strcpy(filePath, cwd);
			strcat(filePath, "/http-root-dir/htdocs/index.html");
		}	//Home directory case
		else if (strstr(requestedPath, "icons") || strstr(requestedPath, "htdocs")) {
			printf("Case File is in icons or htdocs directory\n");
			strcpy(filePath, cwd);
			strcat(filePath, "/http-root-dir");
			strcat(filePath, requestedPath);
		}	//File is in icons or htdocs directory
		else if (requestedPath[strlen(requestedPath) - 1] != '/' && plain == true) {	//&& strstr(requestedPath, "!subdir1/")
			printf("Case dir with no '/'\n");

			printf("Case in dir\n");
			strcpy(filePath, cwd);
			strcat(filePath, "/http-root-dir/htdocs");
			strcat(filePath, requestedPath);
			strcat(filePath, "/");
			plain = false;
			indir = true;


		}// Case dir with no '/'
		else {
			printf("Case files in htdocs\n");
			strcpy(filePath, cwd);
			strcat(filePath, "/http-root-dir/htdocs");
			strcat(filePath, requestedPath);
		}	//Case files in htdocs

		//else if ( strstr(requestedPath, "htdocs") == NULL && strstr(requestedPath, "icons") == NULL){
		//	strcpy(filePath, cwd);
		//	strcat(filePath, "/http-root-dir/htdocs");
		//	strcat(filePath, requestedPath);
		//}
		//else{
		//	strcpy(filePath, cwd);
		//	strcat(filePath, requestedPath);
		//}


		printf("Normal Case: Final file path is %s\n", filePath);

		FILE * file;
		if (!strcmp(docType, "text/html") || !strcmp(docType, "text/plain")) {
			file = fopen(filePath, "r");
		}
		else {
			file = fopen(filePath, "rb");
		}

		if (file == NULL) {	//open(filePath, O_RDONLY) <= 0
			perror("fopen");
			write(socket, "HTTP/1.1 ", strlen("HTTP/1.1 "));

			//Case 404
			write(socket, "404 File Not Found", strlen("404 File Not Found"));

			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "Server: ", strlen("Server: "));
			write(socket, "CS 252 Lab5", strlen("CS 252 Lab5"));
			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "Content-type: ", strlen("Content-type: "));
			write(socket, "text/html", strlen("text/html"));
			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "\015\012", strlen("\015\012"));

			//Case 404 : error = couldn't find specific URL
			write(socket, error, strlen(error));
		}
		else {
			write(socket, "HTTP/1.1 ", strlen("HTTP/1.1 "));

			//Case 200
			write(socket, "200 ", strlen("200 "));
			write(socket, "Document ", strlen("Document "));
			write(socket, "follows", strlen("follows"));

			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "Server: ", strlen("Server: "));
			write(socket, "CS 252 Lab5", strlen("CS 252 Lab5"));
			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "Content-type: ", strlen("Content-type: "));
			write(socket, docType, strlen(docType));
			write(socket, "\015\012", strlen("\015\012"));
			write(socket, "\015\012", strlen("\015\012"));

			//Case Directory Listing 
			if (opendir(filePath)) {	//opendir return NULL if not directory
				directoryListing(filePath, socket);
			}
			else {	//Case normal
				int n;
				unsigned char newChar = 0;
				while (read((int)fileno(file), &newChar, sizeof(newChar))) {
					n = write(socket, &newChar, sizeof(newChar));
					if (n < 0) {
						perror("write");
						break;
					}
				}
				fclose(file);

			}
		}	
	}
	plain = false;
}

void directoryListing(char* filePath, int socket) {
	char *html = (char*)malloc(4096);
	memset(html, '\0', 4096);
	strcpy(html, titleAndheader);
	DIR* dir = opendir(filePath);
	struct dirent *entry = (struct dirent*)malloc(sizeof(struct dirent));

	strcat(html, " <li><A HREF=\"?C=N&O=A\"> Name</A> ");
	strcat(html, " <li><A HREF=\"?C=M&O=A\"> Last Modified</A> ");
	strcat(html, " <li><A HREF=\"?C=S;O=A\"> Size</A> ");

	while (entry = readdir(dir)) {
		strcat(html, "<li><A HREF=\"");

		char *dname = (char*)malloc(200);
		memset(dname, '\0', 200);

	/*	if (indir == true) {
			strcpy(dname, "681/dir1/subdir1/");
			printf("direcotry is being modified\n");
			indir = false;
		}
		else {*/
			strcpy(dname, entry->d_name);
		//

		if (entry->d_name[strlen(entry->d_name) - 1] == '1') {	//add '/' after dir1
			strcat(dname, "/");
		}
	
		//printf("dname is %s\n",dname);

		strcat(html, dname);
		strcat(html, "\"> ");
		strcat(html, entry->d_name); 
		strcat(html, "</A>"); 
	}
	strcat(html, footer);

	write(socket, html, strlen(html));
}
