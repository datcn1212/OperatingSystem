#include<string.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h> 
#include <signal.h> // Thu vien xu ly tin hieu
#include <dirent.h> // Thu vien cho phep kiem duyet thu muc
#include <tlhelp32.h> 
#include <time.h>
#include<shellapi.h> // dung cho ham shellexecute
#include<tchar.h>
#include<minwindef.h>
#include<winbase.h>
DWORD id; // luu id cua process chay foreground
DWORD process_id[50] ; // Mang luu id cua cac process chay background mode
char process_name[50][50] ; // Mang luu ten cac process chay background mode
int count = 0 ; // Bien luu so luong process chay background mode (du da thuc thi xong hay chua)
const int LSH_BUFFERSIZE = 1024; 
bool status[50];
//******************************************************************************************************
//*************************   NHAP XUAT *****************************************************************

// HAM DOC LENH TU BAN PHIM  

char *lsh_read_line(){
	int bufsize = LSH_BUFFERSIZE;
  	int position = 0;
 	char *buffer = (char*) malloc(sizeof(char) * bufsize);
  	int c;

  	if (!buffer) {
    	fprintf(stderr, "lsh: allocation error\n");  // stderr : thiet bi xuat loi chuan
    	exit(EXIT_FAILURE);
  	}

  	while (1) {
// Doc ky tu
    	c = getchar();

// Neu gap ky tu ket thuc "EOF" hoac ky tu xuong dong "\n" thi gan no bang NULL và tra ve cau lenh
    	if (c == EOF || c == '\n') {
      		buffer[position] = '\0';
      		return buffer;
    	} else {
      		buffer[position] = c;
    	}
    	position++;

// Nhap qua do dai cho phep thi xin them bo nho
    	if (position >= bufsize) {
      		bufsize += LSH_BUFFERSIZE;
      		buffer = (char*) realloc(buffer, bufsize);
      		if (!buffer) {
        		fprintf(stderr, "lsh: allocation error\n");
        		exit(EXIT_FAILURE);
      		}
    	}
  	}
}


// HAM TACH DONG LENH THANH CAC CAU LENH NHO VA LUU VAO MANG STRING  

char **lsh_split_line(char *line)
{
  	int bufsize = LSH_BUFFERSIZE, position = 0;
  	char **tokens = (char**)malloc(bufsize * sizeof(char*));
  	char *token;
// Cac ky tu ngan cach
  	char LSH_TOK_DELIM[]= "\t \n";

  	if (!tokens) {
    	fprintf(stderr, "lsh: allocation error\n");
    	exit(EXIT_FAILURE);
  	}
// Ham strtok cho phep tach cac dong lenh khi gap cac ky tu ngat
  	token = strtok(line, LSH_TOK_DELIM);
  	while (token != NULL) {
    	tokens[position] = token;
    	position++;

    if (position >= bufsize) {
      	bufsize += LSH_BUFFERSIZE;
      	tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
      	if (!tokens) {
        	fprintf(stderr, "lsh: allocation error\n");
        	exit(EXIT_FAILURE);
      	}
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

//*************************************************************************************************************
//*************************************************************************************************************
// ************************************************************************************************************


//*************************************************************************************************************
// ************************************KHOI TAO TIEN TRÌNH ****************************************************

//  HAM XU LY TIN HIEU NGAT
void Signal_Handler(int a)
{
	// Ham OpenProcess tra ve handle cua tien trinh dang chay
	// va tra ve NULL neu khong thuc hien thanh cong
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,id); 
    TerminateProcess(hProcess,1);  // hàm ngat tien trình
    CloseHandle(hProcess) ;  
}

// HÀM XU LÍ TIEN TRANH CHAY FOREGROUND PROCESS
int foreground(char * args)
{
    STARTUPINFO si ; 
    PROCESS_INFORMATION pi ; 
    bool bCreateProcess;

    ZeroMemory(&si,sizeof(si)) ; 
    si.cb = sizeof(si) ; 
    // HAM CREATERPROCESS tra ve true neu thanh cong, false neu that bai
    bCreateProcess = CreateProcess(
		(LPCTSTR)args,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);
	if(!bCreateProcess)	{   
        printf("ERROR\n") ; 
        return 1;
    }
    id = pi.dwProcessId ; // set up id cua foreground
    printf("Your process is running\n"); 
//Nhap Ctrl + C de ngat tien trinh SIGINT = Ctrl + C
//Ham signal cho phep tien trinh nhan mot hay nhieu cach xu ly tin hieu ngat tu HDH
//Signal nhan vao hai tham so	1. Tin hieu ngat	2. Ham xu ly tin hieu ngat
	signal(SIGINT,Signal_Handler); 
    
	WaitForSingleObject(pi.hProcess, INFINITE); 
    TerminateProcess(pi.hProcess,1); 
    CloseHandle(pi.hProcess); 
    CloseHandle(pi.hThread); 
    printf("Your process has been terminated!\n");
    return 0;
}

// HAM XU LY TIEN TRINH CHAY BACKGROUND MODE
int background(char * args)
{
    STARTUPINFO si ; 
    PROCESS_INFORMATION pi ;
    bool bCreateProcess;
    
    ZeroMemory(&si,sizeof(si)) ; 
    si.cb = sizeof(si) ; 
    bCreateProcess = CreateProcess(
		(LPCSTR)args,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE ,
		NULL,
		NULL,
		&si,
		&pi);
	if(!bCreateProcess){   
        printf("ERROR\n") ; 
        return 1;
    }
    printf("Your process is running in background mode\n");
    
    // Them cac thong tin ve process vao mang
	process_id[count] = pi.dwProcessId ; 
    strcpy(process_name[count],args) ;  
	status[count] = false; 
    count ++ ; 
    
	
	CloseHandle(pi.hProcess) ; 
    CloseHandle(pi.hThread) ; 
    return 0 ;
}
// HAM EXECUTE A PROGRAM GIVE BY <CMD>
int cmd(char* args){
	// hàm ShellExecute tra ve 1 HINSTANCE co gia tri <=32 neu khong thanh cong, >32 neu thanh cong
	HINSTANCE shell =  
	ShellExecute
	(
	NULL, 
	"open",   //  operation 
	args ,
	NULL,   
	NULL,
	SW_SHOWNORMAL
	);
    long long ab = (long long)shell;
    if(ab <= 32) {
	//printf ("Bad command or file name \n");
	return 0;
	}
    else printf("The program execute success \n");
    return 1;
}
int Execute(char* oper, char* program, char* fName){
	HINSTANCE shell =  
	ShellExecute
	(
	NULL, 
	oper,   //  operation 
	program,
	fName,   
	NULL,
	SW_SHOWNORMAL
	);
    long long ab = (long long)shell;
    if(ab <= 32) printf ("Bad command or file name \n");
    else printf("The program execute success \n");
    return 0;
}

//*************************************************************************************************************
//*************************************************************************************************************
// ************************************************************************************************************



//*************************************************************************************************************
// **************************THAO TAC VOI TIEN TRINH***********************************************************


// HAM KIEM TRA TIEN TRINH CO DANG CHAY HAY KHONG
BOOL IsProcessRunning(DWORD process_id)
{
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    DWORD ret = WaitForSingleObject(process, 1);
	// Ham WaitForSingleObject tra ve gia tri 0 neu tien trinh thuc thi xong, tra ve WAIT_TIMEOUT neu tien trinh chua xong
	// set thoi gian cho doi bang 1 milis giup ta kiem tra lieu tien trinh con song hay khong   
    CloseHandle(process);
    return ret == WAIT_TIMEOUT;
}


// HAM LIST IN RA CAC TIEN TRINH CHAY BACKGROUND MODE

void listAllRunningProcesses()
{
    int i ;   
    printf ( "   PROCESS ID    |    PROCESS NAME                |    STATUS    \n") ;
    for (i = 0; i<count; i++)
    {
         
        if (IsProcessRunning(process_id[i])) {
		if(status[i])  
	    	printf("%10d       |    %-20s        |    SUSPEND   \n",process_id[i],process_name[i]);
        else  
		    printf("%10d       |    %-20s        |    RUNNING   \n",process_id[i],process_name[i]);
        } else {
		    printf("%10d       |    %-20s        |    TERMINATED  \n",process_id[i],process_name[i]);
		}
    }            
}

// HAM KILL MOT TIEN TRINH CHAY BACKGROUND MODE

int kill(DWORD id_kill){
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,id_kill); 
    TerminateProcess(hProcess,1);
    CloseHandle(hProcess) ;  
    int i,j;
    for(i=0;i < count;i++) 
    {
        if (process_id[i] == id_kill)
        {
            for (j = i;j < count -1;j++){                      // xoa phan tu bang cach dich mang
                process_id[j] = process_id[j+1] ;
				status[j] = status[j-1];
				}
            count-- ; 
            break ; 
        }
    }
    return 0 ; 
}

// HAM KILL TAT CA TIEN TRINH CHAY BACKGROUND MODE 
int killAll(){
	int i ; 
    for (i=0;i < count;i++) 
    {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,process_id[i]); 
        TerminateProcess(hProcess,1);
        CloseHandle(hProcess) ; 
    }
    count = 0 ; 
}
// HAM SUSPEND MOT TIEN TRINH CHAY BACKGROUND MODE
void suspend(DWORD ProcessId){
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	
	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);
	
	Thread32First(hThreadSnapshot, &threadEntry);
	
	do{
		if(threadEntry.th32OwnerProcessID == ProcessId){
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			
			SuspendThread(hThread);
			CloseHandle(hThread);
		}
	}
	while (Thread32Next(hThreadSnapshot, &threadEntry));

	CloseHandle(hThreadSnapshot);
	int i;
	for (i=0;i<count;i++) if(ProcessId == process_id[i]){
		status[i] = true;
		break;
	}	
}


// HAM DEBUD - TAC DUNG TUONG TU SUSPEND

void debug(DWORD ProcessId){
    DebugActiveProcess(ProcessId) ;
	int i;
	for (i=0;i<count;i++) if(ProcessId == process_id[i]){
		status[i] = true;
		break;
	} 
}

// HAM STOP DEBUG MOT PROCESS - TUONG TU NHU HAM RESUME MOT TIEN TRINH SUSPEND

void done(DWORD ProcessId){
	DebugActiveProcessStop(ProcessId);
	int i;
	for (i=0;i<count;i++) if(ProcessId == process_id[i]){
		status[i] = false;
		break;
	}
}
// HAM RESUME MOT TIEN TRINH CHAY BACKGROUND MODE

void resume(DWORD ProcessId){
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	
	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);
	
	Thread32First(hThreadSnapshot, &threadEntry);
	
	do{
		if(threadEntry.th32OwnerProcessID == ProcessId){
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			
			ResumeThread(hThread);
			CloseHandle(hThread);	
		}
	}
	while (Thread32Next(hThreadSnapshot, &threadEntry));

	CloseHandle(hThreadSnapshot);
	int i;
	for (i=0;i<count;i++) if(ProcessId == process_id[i]){
	    status[i] = false;
	 	break;
	}   	
}
//*************************************************************************************************************
//*************************************************************************************************************
// ************************************************************************************************************

//*************************************************************************************************************
//*******************************MOT SO TIEN ICH DI KEM********************************************************

//  HÀM IN MENU HELP

void printHelp(){
	int i = 0;
	printf ("\n") ; 
    printf (" My shell supports the following commands:\n") ; 
    printf ("[%d]  help           : print help \n",++i) ; 
    printf ("[%d]  clear          : clear output screen\n",++i);
    printf ("[%d]  time           : print time and date\n",++i) ;
    printf ("[%d]  path           : print environment variable\n",++i);
	printf ("[%d]  addpath        : add or set environment variable\n",++i);
    printf ("[%d]  dir            : list all files in this directory\n",++i) ; 
    printf ("[%d]  cd             : change current directory\n",++i);
    printf ("[%d]  mkdir          : create new directory \n",++i);
	printf ("[%d]  deldir         : delete directory \n",++i);
    printf ("[%d] fil            : create new file\n",++i);
    printf ("[%d] delfil         : delete file\n",++i);
    printf ("[%d] <cmd>          : Execute the program given by <cmd>\n",++i);
    printf ("[%d] fore           : run program in forgeground\n",++i);
    printf ("[%d] back           : run in background\n",++i) ; 
    printf ("[%d] rbat           : run file bat\n",++i);
    printf ("[%d] list           : list the process id is running\n",++i) ; 
    printf ("[%d] killall        : kill all process is running\n",++i) ; 
    printf ("[%d] kill           : kill process with id is running\n",++i); 
    printf ("[%d] suspend/debug  : suspend a process\n",++i) ; 
    printf ("[%d] resume/done    : resume a stopping process\n",++i) ; 
    printf ("[%d] exit           : exit shell\n",++i) ; 
    printf("\n\n") ; 
}

// HAM XOA MAN HINH
void clear(){
	system("cls");
}


// HAM LAY RA NGAY THANG HIEN TAI
void currentTime(){
	time_t rawtime;
   	struct tm *info;
   	time( &rawtime );
   	info = localtime( &rawtime );
   	printf("%s", asctime(info));
}
// HAM THIET LAP THU MUC MAC DINH

int setCurrentDir(char* argv){
	// Ham SetCurrentDirectory can truyen vao lpPathName
	// Return 0 neu that bai va return mot gia tri khac khong neu thanh cong
     if( !SetCurrentDirectory((LPCSTR)argv))
    {
      printf("SetCurrentDirectory failed (%d)\n", GetLastError());
      
      return 0;
    }
    return 1;
}

// HAM IN RA LIST CAC FILE TRONG THU MUC HIEN TAI

void listCurrentDirectory()
{
	TCHAR Buffer[LSH_BUFFERSIZE];
    DWORD dwRet = GetCurrentDirectory(LSH_BUFFERSIZE, Buffer);
    DIR *d;
    struct dirent *dir;
    d = opendir(Buffer);  // ham mo thu muc, tra ve NULL neu khong tim thay thu muc
    if (d != NULL)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    else {
        printf("Couldn't find directory.") ; 
    }
    printf("\n");
 
}
// HAM IN RA ENVIROMENT VARIABLE

int printPath(){
    LPTSTR lpszVariable; 
    LPTCH lpvEnv; 
 
    // Get a pointer to the environment block. 
 
    lpvEnv = GetEnvironmentStrings();

    // If the returned pointer is NULL, exit.
    if (lpvEnv == NULL)
    {
        printf("GetEnvironmentStrings failed (%d)\n", GetLastError()); 
        return 0;
    }
 
    // Variable strings are separated by NULL byte, and the block is 
    // terminated by a NULL byte. 

    lpszVariable = (LPTSTR) lpvEnv;

    while (*lpszVariable)
    {
        _tprintf(TEXT("%s\n"), lpszVariable);
        lpszVariable += lstrlen(lpszVariable) + 1;
    }
    FreeEnvironmentStrings(lpvEnv);
    return 1;
}
// HAM THEM HOAC SUA BIEN MOI TRUONG

int addPath(char* argsName, char* argsValue){
	//Ham SetEnvironmentVariable can truyen 2 tham so lpName va lpValue
	// neu lpName khong ton tai thi tao moi
	//If the function succeeds, the return value is nonzero.
	//If the function fails, the return value is zero. To get extended error information, call GetLastError.
	bool bSetEnv = SetEnvironmentVariable(argsName,argsValue);
	if(!bSetEnv) printf("ERROR\n");
	return 1;
}


// HAM TAO THU MUC MOI TAI THU MUC HIEN TAI
int createDirectory(char* name){
	if(CreateDirectory(name, NULL)) printf("Directory %s has been created\n",name);
	else printf("This directory couldn't create\n%d",GetLastError());
	// Ham GetLastError co the tra ve 2 gia tri 
	// ERROR_ALREADY_EXISTS
	// ERRORR_PATH_NOT_FOUND
	return 1;
}
// HAM REMOVE THU MUC TAI THU MUC HIEN TAI

int removeDirectory(char* name){
	// Ham RemoveDirectory nhan vao 1 tham so la duong dan toi thu muc
	// Ham tra ve kieu BOOL
	if(RemoveDirectory(name)) printf("Directory %s has been remove\n", name);
	else printf("This directory couldn't remove\n%d",GetLastError());
	return 1;
}

// HAM TAO FILE MOI

int createFile(char* fileName){
	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,NULL );
	if(hFile ==  INVALID_HANDLE_VALUE) {
	printf("ERROR\n");
	return 0;
	}
	else printf("Create success\n");
	return 1;
}
// HAM XOA FILE 

int deleteFile(char* fileName){
	if(DeleteFile(fileName)) {
	printf("Delete file success\n");
	return 1;}
	else printf("ERROR\n");
	return 1;
}
void lsh_execute(char** args);
// HAM CHAY FILE .BAT
int runBatFile(char* fileName){
	int bufsize = LSH_BUFFERSIZE;
  	int position = 0;
 	char *buffer = (char*) malloc(sizeof(char) * bufsize);
  	char c;
  	FILE *fptr = fopen(fileName, "r");
  	if(fptr==NULL) {
	  printf("File not found\n");
	  return 0;
	}
  	while (1) {
  		c = fgetc(fptr);
// Neu gap ky tu ket thuc "EOF" hoac ky tu xuong dong "\n" thi gan no bang NULL và tra ve cau lenh
        if ( c == '\n') {
      		buffer[position] = '\0';
      		char** args = lsh_split_line(buffer);
      	    lsh_execute(args);
      		free(args);
      		free(buffer);
      		buffer = (char*) malloc(sizeof(char) * bufsize);
      		position = 0;
      		Sleep(3000);
    	} else {
      		buffer[position] = c;
      		position++;
    	}
    	 if( feof(fptr) ) break ;
  	}
  	fclose(fptr);
  	free(buffer);
  	return 1;
}
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************



//*************************************************************************************************************
// *********************************************PHAN THUC THI *************************************************
//  HAM THUC THI 

void lsh_execute(char** args){
	if (strcmp(args[0],"dir") == 0)
        listCurrentDirectory() ; 
    else if (strcmp(args[0],"fore") == 0)  
        foreground(args[1]) ; 
    else if (strcmp(args[0],"back") == 0) 
        background(args[1]) ; 
    else if(strcmp(args[0], "rbat") == 0)
        runBatFile(args[1]) ;
  	else if (strcmp(args[0],"list") == 0) 
        listAllRunningProcesses() ; 
    else if (strcmp(args[0],"killall") == 0 ) 
        killAll();
    else if (strcmp(args[0],"kill") == 0 ) 
        kill(DWORD (atoi(args[1])));  
    else if (strcmp(args[0],"debug") == 0) 
        debug( DWORD (atoi(args[1])) ); 
    else if (strcmp(args[0],"suspend") == 0)
        suspend(DWORD (atoi(args[1])) ) ; 
    else if (strcmp(args[0],"resume") == 0)
        resume(DWORD (atoi(args[1])) ) ;
    else if (strcmp(args[0],"time") == 0) 
        currentTime() ;
    else if (strcmp(args[0],"help") == 0) 
        printHelp() ; 
    else if(strcmp(args[0],"clear") == 0) 
        clear();
    else if(strcmp(args[0], "done") == 0) 
        done(DWORD (atoi(args[1])));
    else if(strcmp(args[0],"path") == 0)
        printPath();
    else if(strcmp(args[0],"addpath") == 0)
        addPath(args[1], args[2]);
    else if(strcmp(args[0], "cd") == 0)
        setCurrentDir(args[1]);
    else if(strcmp(args[0],"mkdir") == 0)
        createDirectory(args[1]);
    else if(strcmp(args[0], "fil") == 0)
        createFile(args[1]);
    else if(strcmp(args[0], "delfil") == 0)
        deleteFile(args[1]);
    else if(strcmp(args[0],"deldir") == 0) 
        removeDirectory(args[1]);
    else if (strcmp(args[0],"exit") == 0) 
        {
        killAll(); // sending kill signal to all child background mode 
		exit(0) ; 
		}
    else if(cmd(args[0])){}
    else Execute(args[0], args[1], args[2]);
}



// VONG LAP CAC HAM THUC THI
void lsh_loop(void)
{
  	char *line;
  	char **args;		// Mang cac tham so
  	int i;
	
	printHelp();
  	do {
  		TCHAR Buffer[LSH_BUFFERSIZE];
        DWORD dwRet = GetCurrentDirectory(LSH_BUFFERSIZE, Buffer);
        printf("%s\n",Buffer);
    	printf("> ");
    	line = lsh_read_line();
    	args = lsh_split_line(line);
    	lsh_execute(args);
    	free(line);
    	free(args);
  	} while (1);
}


int main()
{
  // Run command loop.
  lsh_loop();
  return EXIT_SUCCESS;
}
