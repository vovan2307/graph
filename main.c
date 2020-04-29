#include <windows.h>
#include "graph.h"
#include "panel.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK MainWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

void CreatePanel(HWND parent, HWND *btns);
void RemapPanel(HWND parent, HWND *btns);
void draw(HDC surface, PAINTSTRUCT *ps);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{

	MSG  msg={0};
	WNDCLASS wc={0};
    int width, height;
	
	wc.style                = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc          = MainWindowProc; // Оконная процедура
	wc.hInstance            = hInstance;
	wc.hbrBackground		= CreateSolidBrush(RGB(255,255,255)); // Белый фон
	wc.hCursor				= LoadCursor(wc.hInstance, IDC_ARROW);
	wc.lpszClassName        = L"Graph-Window";
	
	// Узнаём разрешение экрана
	width=GetSystemMetrics(SM_CXSCREEN);
	height=GetSystemMetrics(SM_CYSCREEN);
	
	RegisterClass(&wc);

	// Создаём главное окно 
	HWND window = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT, wc.lpszClassName, wc.lpszClassName,
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		width/16,height/16,width*3/4,height*3/4,
		0,0, hInstance, 0);
	
	while (GetMessage(&msg,0,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(wc.lpszClassName, hInstance);
	return msg.wParam;
}

// Процедура обработки сообщений главного окна
LRESULT CALLBACK MainWindowProc(HWND window, UINT message,
                         WPARAM wparam, LPARAM lparam)
{
	static HWND btns[7] = {NULL}; // Кнопки панели управления
	static PAINTSTRUCT ps = {0}; 
	HDC hdc = NULL;

	switch(message)
    {		
		case WM_SIZE:
		break;
		case WM_COMMAND:
			ProcessButton(window, LOWORD(wparam)); // Обработать нажатие кнопки
			break;
		case WM_LBUTTONDOWN:
			ProcessMouseClick(window, LOWORD(lparam), HIWORD(lparam)); // Обработать щелчок мыши
			break;
		case WM_PAINT:
			hdc = BeginPaint(window, &ps);
			draw(hdc, &ps);        // Вызвать рисующий код
			EndPaint(window, &ps);
			break;

		case WM_CREATE:
			CreateMenu1(window);
			break;
		
		case WM_CLOSE:
			DestroyWindow(window);
        break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		default:
			return DefWindowProc(window, message, wparam, lparam);
    }
    return 0;
}

void draw(HDC surface, PAINTSTRUCT *ps){
	drawGraph(surface, ps);
}