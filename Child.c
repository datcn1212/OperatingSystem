#include <windows.h>
#include <string.h> 

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
int timer = 12;
char szClassName[ ] = "WindowsApp";

int WINAPI
WinMain (HINSTANCE hThisInstance,
         HINSTANCE hPrevInstance,
         LPSTR lpszArgument,
         int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
	UINT myTimer;
	
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "A child process", 	/* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           240,                 /* The programs width */
           150,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);
	myTimer = SetTimer (hwnd, 0, 1000, NULL ); /*set massage WM_TIMER to hwnd each 1000ms*/

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK
WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	char time[5];
	HFONT hFont=CreateFont(100,0,0,0,700,FALSE,FALSE,FALSE,0,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Comic Sans MS");
			
    switch (message)                  /* handle the messages */
    {
		case WM_TIMER:
			timer--;
			GetClientRect(hwnd, &rt);
			if(timer < 0) PostQuitMessage (0);  /* send a WM_QUIT to the message queue */
			else  RedrawWindow(hwnd,&rt,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_ERASENOW|RDW_NOCHILDREN);
			break; //no break here				
			
    	case WM_PAINT:
    		if(timer==0) sprintf(time,"%s","exit()");
    		else sprintf(time,"%d",timer);
			hdc = BeginPaint(hwnd, &ps);
			SelectObject(hdc,hFont);
			GetClientRect(hwnd, &rt);		
			DrawText(hdc, time, strlen(time), &rt, DT_CENTER);
			EndPaint(hwnd, &ps);
			DeleteObject(hFont);
			break;
			
        case WM_DESTROY:
            PostQuitMessage (0);    /* send a WM_QUIT to the message queue */
            break;
        default:                    /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
