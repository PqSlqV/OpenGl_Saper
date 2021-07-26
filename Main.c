#include <windows.h>
#include <gl/gl.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

#define mapH 10
#define mapW 10

typedef struct {
	BOOL open;
	BOOL mine;
	BOOL flag;
	int num;
}TCell;

TCell map[mapH][mapW];
int mines = 20;
int ClosedCell;
BOOL fail;

BOOL IsMap(int x, int y)
{
	return (x >= 0) && (y >= 0) && (x < mapW) && (y < mapH);
}

void ScreenOpenGl(HWND hwnd, int x, int y, float* ox, float* oy)
{
	RECT rct;
	GetClientRect(hwnd, &rct);
	*ox = x / (float)rct.right * mapW;
	*oy = (mapH + 1) - y / (float)rct.bottom * (mapH + 1);
}

void Line(float x, float y, float xs, float ys)
{
	glVertex2f(x, y);
	glVertex2f(xs, ys);
}

void OpenCellFlag(int, int);

void OpenCell(int x, int y)
{
	if (!IsMap(x, y) || map[y][x].flag || map[y][x].open) return;
	map[y][x].open = TRUE;
	ClosedCell -= 1;
	if (map[y][x].num == 0)
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				OpenCell(x + j, y + i);
	if (map[y][x].mine)
	{
		fail = TRUE;
		for (int i = 0; i < mapH; i++)
			for (int j = 0; j < mapW; j++)
				map[i][j].open = TRUE;
	}
}

void OpenCellFlag(int x, int y)
{
	int k = 0;
	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
			if (map[y + i][x + j].flag && IsMap(y + i, x + j)) k += 1;
	if (map[y][x].num == k)
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				if (!map[y + i][x + j].flag && IsMap(y + i, x + j))
				{
					if (map[y + i][x + j].num == 0) OpenCell(x + i, y + j);
					if (!map[y + i][x + j].open) ClosedCell -= 1;
					map[y + i][x + j].open = TRUE;
					if (map[y + i][x + j].mine && !map[y + i][x + j].flag)
					{
						fail = TRUE;
						for (int i = 0; i < mapH; i++)
							for (int j = 0; j < mapW; j++)
								map[i][j].open = TRUE;
					}
				}
}


void Show_Num(int i)
{

	glColor3f(1, 1, 0);
	glLineWidth(5);
	glBegin(GL_LINES);
	if ((i != 2) && (i != 5) && (i != 6)) Line(0.64f, 0.22f, 0.64f, 0.78f);
	if ((i != 1) && (i != 4)) Line(0.36f, 0.78f, 0.64f, 0.78f);
	if (i == 2) Line(0.64f, 0.5f, 0.64f, 0.78f);
	if (i == 2) Line(0.36f, 0.22f, 0.36f, 0.5f);
	if ((i == 5) || (i == 6)) Line(0.64f, 0.22f, 0.64f, 0.5f);
	if ((i == 4) || (i == 5) || (i == 9)) Line(0.36f, 0.5f, 0.36f, 0.78f);
	if ((i == 0) || (i == 6) || (i == 8)) Line(0.36f, 0.22f, 0.36f, 0.78f);
	if ((i != 1) && (i != 4) && (i != 7)) Line(0.36f, 0.22f, 0.64f, 0.22f);
	if ((i != 0) && (i != 1) && (i != 7)) Line(0.36f, 0.5f, 0.64f, 0.5f);
	glEnd();
}

void  Show_Cell()
{
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.8f, 0.8f, 0.8f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.7f, 0.7f, 0.7f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	glColor3f(0.6f, 0.6f, 0.6f);
	glVertex2f(1.0f, 0.0f);
	glEnd();
}

void  Show_Open_Cell()
{
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.4f, 0.7f, 0.4f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.3f, 0.6f, 0.3f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	glColor3f(0.2f, 0.5f, 0.2f);
	glVertex2f(1.0f, 0.0f);
	glEnd();
}
void Show_Mine()
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.2f, 0.8f);
	glVertex2f(0.8f, 0.8f);
	glVertex2f(0.8f, 0.2f);
	glVertex2f(0.2f, 0.2f);
	glEnd();
}

void Show_Flag()
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glVertex2f(0.4f, 0.2f);
	glVertex2f(0.4f, 0.8f);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.4f, 0.5f);
	glVertex2f(0.4f, 0.8f);
	glVertex2f(0.8f, 0.7f);
	glEnd();
}

void Game_Show()
{
	glLoadIdentity();
	glScalef(2.0f / mapW, 2.0f / (mapH + 1.0f), 0.0f);
	glTranslatef(-mapW * 0.5f, (-mapH - 1.0f) * 0.5f, 0.0f);
	for (int i = 0; i < mapH; i++)
		for (int j = 0; j < mapW; j++)
		{
			glPushMatrix();
			glTranslatef((float)j, (float)i, 0.0f);
			if (!map[i][j].open) Show_Cell();
			else if (map[i][j].mine)
			{
				Show_Open_Cell();
				Show_Mine();
			}
			else if (map[i][j].num != 0)
			{
				Show_Open_Cell();
				Show_Num(map[i][j].num);
			}
			else
			{
				Show_Open_Cell();
			}
			if (map[i][j].flag) Show_Flag();
			glPopMatrix();
		}

	if (ClosedCell >= 0)
	{
		int k = ClosedCell % 10;
		glPushMatrix();
		glTranslatef(5.0f, mapH, 0.0f);
		Show_Cell();
		Show_Num(k);
		glPopMatrix();
		if (ClosedCell > 9)
		{
			k = (ClosedCell % 100) / 10;
			glPushMatrix();
			glTranslatef(4.0f, mapH, 0.0f);
			Show_Cell();
			Show_Num(k);
			glPopMatrix();
			if (ClosedCell > 99)
			{
				k = ClosedCell / 100;
				glPushMatrix();
				glTranslatef(3.0f, mapH, 0.0f);
				Show_Cell();
				Show_Num(k);
				glPopMatrix();
			}
		}
	}
}

void Game_New()
{
	srand(time(NULL));
	ClosedCell = mapH * mapW - mines;
	memset(map, 0, sizeof(map));
	fail = FALSE;
	for (int i = 0; i < mines; i++)
	{
		int y = rand() % mapH;
		int x = rand() % mapW;
		if (map[y][x].mine) i--;
		else
		{
			map[y][x].mine = TRUE;
			if ((x - 1) >= 0) map[y][x - 1].num++;
			if ((x + 1) < mapW) map[y][x + 1].num++;
			if ((y - 1) >= 0) map[y - 1][x].num++;
			if ((y + 1) < mapH) map[y + 1][x].num++;
			if (((x + 1) < mapW) && ((y + 1) < mapH)) map[y + 1][x + 1].num++;
			if (((x - 1) >= 0) && ((y + 1) < mapH)) map[y + 1][x - 1].num++;
			if (((x + 1) < mapW) && ((y - 1) >= 0)) map[y - 1][x + 1].num++;
			if (((x - 1) >= 0) && ((y - 1) >= 0)) map[y - 1][x - 1].num++;
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	int n = 0;
	float theta = 0.0f;
	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "Open";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		"Open",
		"Saper",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		640,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);
	Game_New();
	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/* OpenGL animation code goes here */

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			Game_Show();
			SwapBuffers(hDC);
			Sleep(1);
		}
	}

	DisableOpenGL(hwnd, hDC, hRC);

	DestroyWindow(hwnd);

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{
		POINTFLOAT pt;
		ScreenOpenGl(hwnd, LOWORD(lParam), HIWORD(lParam), &pt.x, &pt.y);
		int x = (int)pt.x;
		int y = (int)pt.y;
		if (IsMap(x, y))
		{
			if (fail || (ClosedCell == 0)) Game_New();
			else if (!map[y][x].flag && !map[y][x].open) OpenCell(x, y);
			else if (map[y][x].open) OpenCellFlag(x, y);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		POINTFLOAT pt;
		ScreenOpenGl(hwnd, LOWORD(lParam), HIWORD(lParam), &pt.x, &pt.y);
		int x = (int)pt.x;
		int y = (int)pt.y;
		if (IsMap(x, y) && !map[y][x].flag && !map[y][x].open) map[y][x].flag = TRUE;
		else map[y][x].flag = FALSE;
	}
	break;
	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{


		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}