#include "stdafx.h"
#include "draw2.h"
#include <string>
#include <vector>
#include <queue>
#include <cstdio>
#include <cmath>

#define MAX_LOADSTRING 100
#define TMR_1 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// buttons
HWND hwndButton;

// some useful variables
int count, count2, weight, FL, N;
double seconds;

// lift stuff
wchar_t* sign = L"=";
int LIFTdirection, PlaceToStand;
bool isOpened = false, isEmpty = false, isFULL = false, dir = true, ReadyToGoOut = false;
const int LiftSpace[5] = { 320, 306, 292, 278, 264 };
bool TakenSpace[5] = { false, false, false, false, false };
unsigned int WhosGoing[5] = { 0,0,0,0,0 };
std::vector <int> WayToGo;

// moving elements
int x, y, DOORy, legX, PeopleGoingIn;
std::vector <int> NextFloor;

// people
std::vector<Person> people;		// people in the lift and on the floors after leaving the lift
std::vector <Person> Queues[6]; // people in queues on the floors
std::vector<int> QueuesLastPos[6]; // to store the last X position for legs movement
std::vector<int> peopleLastPos;	   // to store the last X position for legs movement
std::vector<bool> inTheLift;
int destination, NumberOfPeople;

// Areas to repaint the window
RECT AreaOfSimulation = {2, 2, 332, 500};
RECT AreaOfNumbers = {401, 361, 569, 419};
RECT AreaOfNumbers2 = {481, 279, 580, 340};
RECT AreaOfNumbers3 = { 481, 154, 580, 183};

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

int PeopleInTheLift()
{
	int Number = 0;
	int size = inTheLift.size();
	for (int i = 0; i < size; i++)
		if (inTheLift[i])
			Number++;
	return Number;
}

bool alreadyGoing(int D)
{
	int NumberOfCommands = NextFloor.size();
	for (int i = 0; i < NumberOfCommands; i++)
	{
	if (D == NextFloor[i])
		return true;
	}
	return false;
}

void DeletePeople()
{
	unsigned int j = 0;
	while (j < people.size())
	{
		if (people[j].positionX < 0)
		{
			people.erase(people.begin() + j);
			inTheLift.erase(inTheLift.begin() + j);
			peopleLastPos.erase(peopleLastPos.begin()+j);

			for (int p = 0; p < 5; p++)
				if (WhosGoing[p] > j)
					WhosGoing[p]--;

		}
		else j++;
	}
}

void MyOnPaint(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	Pen bluepen(Color(255, 0, 0, 255));

	SolidBrush brush(Color(255, 255, 0, 0));
	Font font(L"Arial", 8);
	Font font2(L"Arial", 20);

	// weight of lift's passengers
	std::wstring Wstring = std::to_wstring(weight);
	const wchar_t* WEIGHTchar = Wstring.c_str();

	// destination floor
	std::wstring Fstring = std::to_wstring(destination);
	const wchar_t* FLOORchar = Fstring.c_str();
	
	// starting floor
	std::wstring Nstring = std::to_wstring(N);
	const wchar_t* Nchar = Nstring.c_str();

	// graphics 
	// area of simulation
	graphics.DrawRectangle(&pen, 1, 1, 650, 500);
	graphics.DrawRectangle(&pen, 400, 360, 170, 60);

	// text:
	PointF TEXTpoint(410, 373);
	graphics.DrawString(L"Weight: ", -1, &font2, TEXTpoint, &brush);
	
	PointF Wpoint(510, 373);
	graphics.DrawString(WEIGHTchar, -1, &font2, Wpoint, &brush);

	PointF TXTpoint3(490, 140);
	PointF TXTpoint4(508, 155);
	graphics.DrawString(L"floor to go: ", -1, &font, TXTpoint3, &brush);
	graphics.DrawString(FLOORchar, -1, &font2, TXTpoint4, &brush);

	PointF TXTpoint(488, 265);
	PointF TXTpoint2(508, 280);
	graphics.DrawString(L"floor to spawn: ", -1, &font, TXTpoint, &brush);
	graphics.DrawString(Nchar, -1, &font2, TXTpoint2, &brush);
	
	PointF TXTpoint5(398, 340);
	graphics.DrawString(L"MAXIMUM CAPACITY 5 PERSONS!", -1, &font, TXTpoint5, &brush);

	// floors
	graphics.DrawLine(&pen, 1, 90, 250, 90);
	graphics.DrawLine(&pen, 1, 170, 250, 170);
	graphics.DrawLine(&pen, 1, 250, 250, 250);
	graphics.DrawLine(&pen, 1, 330, 250, 330);
	graphics.DrawLine(&pen, 1, 410, 250, 410);
	graphics.DrawLine(&pen, 1, 490, 650, 490);

	// lift 
	graphics.DrawRectangle(&pen, 250, 10, 80, 480);

	PointF signpoint(285, 408+y);
	graphics.DrawString(sign, -1, &font, signpoint, &brush);

	graphics.DrawRectangle(&pen, 252, 410+y, 78, 10);
	graphics.DrawRectangle(&pen, 280, 410+y, 20, 10);
	graphics.DrawRectangle(&pen, 252, 420+y, 78, 70);

	graphics.DrawLine(&pen, 251, 410+y, 251, 490+y+DOORy);

	// drawing people in the queues
	for (int i = 0; i < 6; i++)
	{
		//if (!Queues[i].empty())
		int PeopleInTheQueue = Queues[i].size();
		for (int j = 0; j < PeopleInTheQueue; j++)
		{
			graphics.DrawEllipse(&bluepen, Queues[i][j].positionX - 7, Queues[i][j].positionY - 56, 14, 14);
			graphics.DrawLine(&bluepen, Queues[i][j].positionX, Queues[i][j].positionY - 42, Queues[i][j].positionX, Queues[i][j].positionY - 24);
			graphics.DrawLine(&bluepen, Queues[i][j].positionX, Queues[i][j].positionY - 40, Queues[i][j].positionX - 7, Queues[i][j].positionY - 22);
			graphics.DrawLine(&bluepen, Queues[i][j].positionX, Queues[i][j].positionY - 40, Queues[i][j].positionX + 7, Queues[i][j].positionY - 22);
			graphics.DrawLine(&bluepen, Queues[i][j].positionX, Queues[i][j].positionY - 24, Queues[i][j].positionX - 7 + (abs(Queues[i][j].positionX - QueuesLastPos[i][j])*legX), Queues[i][j].positionY);
			graphics.DrawLine(&bluepen, Queues[i][j].positionX, Queues[i][j].positionY - 24, Queues[i][j].positionX + 7 - (abs(Queues[i][j].positionX - QueuesLastPos[i][j]) * legX), Queues[i][j].positionY);
		}
	}

	// drawing people elsewhere
	if(!people.empty())
		for (int i = 0; i < NumberOfPeople; i++)
		{
			graphics.DrawEllipse(&bluepen, people[i].positionX-7, people[i].positionY - 56, 14, 14);
			graphics.DrawLine(&bluepen, people[i].positionX, people[i].positionY - 42, people[i].positionX, people[i].positionY - 24);
			graphics.DrawLine(&bluepen, people[i].positionX, people[i].positionY - 40, people[i].positionX - 7, people[i].positionY - 22);
			graphics.DrawLine(&bluepen, people[i].positionX, people[i].positionY - 40, people[i].positionX + 7, people[i].positionY - 22);
			graphics.DrawLine(&bluepen, people[i].positionX, people[i].positionY - 24, people[i].positionX - 7 + (abs(people[i].positionX - peopleLastPos[i]) * legX), people[i].positionY);
			graphics.DrawLine(&bluepen, people[i].positionX, people[i].positionY - 24, people[i].positionX + 7 - (abs(people[i].positionX - peopleLastPos[i]) * legX), people[i].positionY);
		}
}

void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea)
{
	if (drawArea==NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	MyOnPaint(hdc);

	EndPaint(hWnd, &ps);
}

int OnCreate(HWND window)
{
	SetWindowText(window, L"LIFT SIMULATOR 2020 - ACiR1 - project no 4");
	return 0;
}

// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// initializing values of some variables
	x = 0, y = 0, DOORy = 0, legX = 0, PeopleGoingIn = 0, NumberOfPeople = 0,
	LIFTdirection = 0, PlaceToStand = 0,
	count = 0, count2 = 0, weight = 0, FL = 0, N = 0;

	seconds = 0;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	// create button and store the handle       

	hwndButton = CreateWindow(TEXT("button"), // The class name required is button
		TEXT("START"),                       // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		400, 10,                                  // the left and top co-ordinates
		150, 60,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("CALL"),                          
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		400, 140,                                
		80, 50,                              
		hWnd,                               
		(HMENU)ID_BUTTON2,                   
		hInstance,                           
		NULL);         

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("SPAWN PERSON"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		400, 200,
		150, 50,
		hWnd,
		(HMENU)ID_BUTTON3,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("-1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		400, 260,
		40, 50,
		hWnd,
		(HMENU)ID_BUTTON4,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("+1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		440, 260,
		40, 50,
		hWnd,
		(HMENU)ID_BUTTON5,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),      
		TEXT("5"),               
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  
		580, 10,
		60, 60,
		hWnd,                           
		(HMENU)ID_BUTTON6,                 
		hInstance,                  
		NULL);                      

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("4"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		580, 80,
		60, 60,
		hWnd,
		(HMENU)ID_BUTTON7,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("3"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		580, 150,
		60, 60,
		hWnd,
		(HMENU)ID_BUTTON8,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		580, 220,
		60, 60,
		hWnd,
		(HMENU)ID_BUTTON9,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		580, 290,
		60, 60,
		hWnd,
		(HMENU)ID_BUTTON10,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		580, 360,
		60, 60,
		hWnd,
		(HMENU)ID_BUTTON11,
		hInstance,
		NULL);

	// create button and store the handle                                                       

	hwndButton = CreateWindow(TEXT("button"), TEXT("Simulation ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		400, 450, 120, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Simulation OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		520, 450, 120, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
PAINTSTRUCT ps;
HDC hdc;
Person man(N, destination);

switch (message)
{
case WM_COMMAND:
	wmId = LOWORD(wParam);
	wmEvent = HIWORD(wParam);

	// MENU & BUTTON messages
	// Parse the menu selections:
	switch (wmId)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	case ID_BUTTON1:
		repaintWindow(hWnd, hdc, ps, NULL);
		break;
	case ID_BUTTON2:
		NextFloor.push_back((6 - destination) * 80 + 10);
		break;
	case ID_BUTTON3:
		if (N != destination)
		{
			Queues[N].push_back(man);
			QueuesLastPos[N].push_back(0);
		}
		repaintWindow(hWnd, hdc, ps, &AreaOfSimulation);
		break;
	case ID_BUTTON4:
		N -= 1;
		if (N < 0)
			N = 0;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers2);
		break;
	case ID_BUTTON5:
		N += 1;
		if (N > 5)
			N = 5;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers2);
		break;
	case ID_BUTTON6:
		destination = 5;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers3);
		break;
	case ID_BUTTON7:
		destination = 4;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers3);
		break;
	case ID_BUTTON8:
		destination = 3;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers3);
		break;
	case ID_BUTTON9:
		destination = 2;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers3);
		break;
	case ID_BUTTON10:
		destination = 1;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers3);
		break;
	case ID_BUTTON11:
		destination = 0;
		repaintWindow(hWnd, hdc, ps, &AreaOfNumbers3);
		break;
	case ID_RBUTTON1:
		SetTimer(hWnd, TMR_1, 40, 0);
		break;
	case ID_RBUTTON2:
		KillTimer(hWnd, TMR_1);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
case WM_PAINT:
	hdc = BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
	break;
case WM_DESTROY:
	PostQuitMessage(0);
	break;

case WM_TIMER:
	switch (wParam)
	{
	case TMR_1:

		// legs moving
		if (dir)
		{
			legX++;
			if (legX == 7)
				dir = false;
		}
		else
		{
			legX--;
				if (legX == 0)
				dir = true;
		}

		// calculating weight
		weight = 0;
		int num = inTheLift.size();
		for (int i = 0; i < num; i++)
			if (inTheLift[i])
				weight += 70;

		// deleting people going off screen
		DeletePeople();

		// moving people in the queues
		for (int i = 0; i < 6; i++)
		{
			int NumberOfPeopleInTheQueue = Queues[i].size();
			for (int j = 0; j < NumberOfPeopleInTheQueue; j++)
			{
				QueuesLastPos[i][j] = Queues[i][j].positionX;

				if (Queues[i][j].positionX < 234 - (j*18))
					Queues[i][j].positionX++;

				if (Queues[i][j].positionX == 234 )
				{
					if (!alreadyGoing((6 - i) * 80 + 10))
						NextFloor.push_back((6 - i) * 80 + 10);
				}
			}
		}

		// people who arrived at their destination floor
		NumberOfPeople = people.size();
		for (int i = 0; i < NumberOfPeople; i++)
		{
			peopleLastPos[i] = people[i].positionX;
			if (people[i].positionX < 250 && people[i].positionY == ((6 - people[i].getDestination()) * 80) + 10)
				people[i].positionX--;
		}
 
		// getting the right lift direction and sign
		if (NextFloor.empty())
		{
			LIFTdirection = 0;
			sign = L"=";
		}
		else
		{
			if (490 + y < NextFloor.front())
			{
				sign = L"v";
				LIFTdirection = 1;
			}
			else
				if (490 + y > NextFloor.front())
				{
					sign = L"^";
					LIFTdirection = 2;
				}
				else
				{
					sign = L"=";
					LIFTdirection = 0;
					FL = 6 - ((490 + y - 10) / 80);
				}
		}

		// lift animation
		switch (LIFTdirection)
		{
		case 0:         // stopped lift

			if (NextFloor.empty() && weight == 0 && Queues[FL].empty())
			{
				// waiting 5 seconds and going down
				if (seconds < 5)
					seconds += 0.04;
				else
				{
					seconds = 0;
					if (FL != 0)
					{
						NextFloor.push_back(490);
						isEmpty = true;
					}
				}
			}
			else
			{
				// if the lift came down empty we don't want the door to open
				if (isEmpty)
				{
					isEmpty = false;
					NextFloor.erase(NextFloor.begin());
				}
				else
				{
					// opening the door
					if ((DOORy > -70 && !isOpened && !NextFloor.empty()) || (isOpened && DOORy > -70 && !Queues[FL].empty() && !isFULL))
						DOORy -= 2;
					else
						isOpened = true;
					
					// after the door has been opened
					if (isOpened)
					{
						// people's interaction with the lift
						for (int i = 0; i < NumberOfPeople; i++)
						{
							if (TakenSpace[0] && TakenSpace[1] && TakenSpace[2] && TakenSpace[3] && TakenSpace[4])
								isFULL = true;
							else
								isFULL = false;

							if (people[i].positionY == (6 - people[i].getFloor()) * 80 + 10 && people[i].positionY == (6 - FL) * 80 + 10 && !isFULL)
							{
								// variable to store the index of an empty place in the lift
								int k = -1;

								// looking for an empty place in the lift

								for (int j = 0; j < 5; j++)
								{
									if (!TakenSpace[j] && WhosGoing[j] == i + 1)
									{
										k = j;
										break;
									}

									bool Going = false;
									for (int p = 0; p < 5; p++)
										if (WhosGoing[p] == i + 1)
										{
											Going = true;
											break;
										}

									if (!TakenSpace[j] && !WhosGoing[j] && !Going)
									{
										WhosGoing[j] = i + 1;
										k = j;
										break;
									}
								}

								if (k != -1)
								{

									if (people[i].positionX < LiftSpace[k])
										people[i].positionX++;

									if (people[i].positionX == 250 && !inTheLift[i])
									{
										inTheLift[i] = true;
										PeopleGoingIn--;
									}

									if (people[i].positionX == LiftSpace[k] && !TakenSpace[k])
									{
										TakenSpace[k] = true;
										if (!alreadyGoing((6 - people[i].getDestination()) * 80 + 10))
											NextFloor.push_back((6 - people[i].getDestination()) * 80 + 10);
									}

								}
							}

							// leaving the empty place in the lift after arrival
							if (ReadyToGoOut)
							{
								for (int j = 0; j < 5; j++)
									for (int p = 0; p < NumberOfPeople; p++)
										if (people[p].positionX == LiftSpace[j] && TakenSpace[j] && people[p].positionY == ((6 - people[p].getDestination()) * 80) + 10 && FL == people[p].getDestination())
										{
											TakenSpace[j] = false;
											WhosGoing[j] = 0;
										}

								ReadyToGoOut = false;
							}

							if (inTheLift[i] && people[i].positionY == ((6 - people[i].getDestination()) * 80) + 10 && FL == people[i].getDestination())
								people[i].positionX--;
							
							if (inTheLift[i] && people[i].positionX < 250 && people[i].positionY == ((6 - people[i].getDestination()) * 80) + 10)
								inTheLift[i] = false;
						}

						
						// adding first person from the queue to the people's vector
						if (!Queues[FL].empty())
						{
							// checking whether there's already 5 people in front of the 1st person in the queue
							int ppl = 0;
							for (int p = 0; p < NumberOfPeople; p++)
								if (people[p].positionX >= 234 && people[p].positionY == ((6 - FL) * 80 + 10))
									ppl++;

							if (Queues[FL].front().positionX >= 234 && ppl < 5)
							{
								people.push_back(Queues[FL].front());
								inTheLift.push_back(false);
								
								peopleLastPos.push_back(Queues[FL].front().positionX);

								Queues[FL].erase(Queues[FL].begin());
								QueuesLastPos[FL].erase(QueuesLastPos[FL].begin());
								PeopleGoingIn++;
							}
						}
							
						// counting number of people in the lift and people that have already taken their places
						// then comparing it to decide if we can close the door
						count = 0;
						for (int i = 0; i < 5; i++)
							if (TakenSpace[i])
								count++;

						count2 = 0;
						if (!inTheLift.empty())
						{
							int size = inTheLift.size();
							for (int i = 0; i < size; i++)
								if (inTheLift[i])
									count2++;
						}

						// closing the door
						if ((!PeopleGoingIn && count == count2 && Queues[FL].empty()) || (isFULL))
							DOORy += 2;

						if (DOORy == 0)
						{
							isOpened = false;
							ReadyToGoOut = true;

							// removing last call
							if (!NextFloor.empty())
								NextFloor.erase(NextFloor.begin());
						}
					}
				}
			}
			repaintWindow(hWnd, hdc, ps, &AreaOfNumbers);
			repaintWindow(hWnd, hdc, ps, &AreaOfSimulation);
			break;
		case 1:                                    // going down
			y += 2;
			for (int i = 0; i < NumberOfPeople; i++)
			{
				if (inTheLift[i] && !isOpened)
				{
					people[i].positionY += 2;
				}
			}
			repaintWindow(hWnd, hdc, ps, &AreaOfSimulation);
			break;
		case 2:                                    // going up
			y -= 2;
			for (int i = 0; i < NumberOfPeople; i++)
			{
				if (inTheLift[i] && !isOpened)
				{
					people[i].positionY -= 2;
				}
			}
			repaintWindow(hWnd, hdc, ps, &AreaOfSimulation);
			break;
		default: break;
		}
	break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
