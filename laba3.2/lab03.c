#include <windows.h>
#include <stdio.h>

#define ID_BEEP 1000
#define ID_QUIT 1001
#define ID_TEXT 1002
#define ID_EDIT 1003
#define ID_EDIT_COPY 1004
#define ID_EDIT_PASTE 1005
#define ID_EDIT_CUT 1006
#define ID_FILE_OPEN 1007
#define ID_FILE_SAVE 1008
#define ID_HELP_ABOUT 1009

const char g_szClassName[] = "myWindowClass";
HWND edit;
char g_szTextBuffer[1024] = "";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
            edit = CreateWindowW(L"Edit", L"You can write text here",
                                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
                                 15, 15, 300, 200, hwnd, (HMENU)ID_EDIT, NULL, NULL);
            CreateWindowW(L"Button", L"Beep", WS_VISIBLE | WS_CHILD, 20, 250, 80, 25, hwnd,
                          (HMENU)ID_BEEP, NULL, NULL);
            CreateWindowW(L"Button", L"Text", WS_VISIBLE | WS_CHILD, 120, 250, 80, 25, hwnd,
                          (HMENU)ID_TEXT, NULL, NULL);
            CreateWindowW(L"Button", L"Quit", WS_VISIBLE | WS_CHILD, 220, 250, 80, 25, hwnd,
                          (HMENU)ID_QUIT, NULL, NULL);

            // Создание главного меню
            HMENU hMenu, hSubMenu;
            hMenu = CreateMenu();
            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, "Open");
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, "Save");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "File");

            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_HELP_ABOUT, "About");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "Help");

            SetMenu(hwnd, hMenu);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_BEEP:
                    MessageBeep(MB_OK);
                    break;
                case ID_TEXT:;
                    char buff[1024];
                    GetWindowText(edit, buff, 1024);
                    MessageBox(NULL, buff, "Info!", MB_ICONINFORMATION | MB_OK);
                    break;
                case ID_QUIT:
                    PostQuitMessage(0);
                    break;
                case ID_EDIT_COPY:
                    SendMessage(edit, WM_COPY, 0, 0);
                    break;
                case ID_EDIT_PASTE:
                    SendMessage(edit, WM_PASTE, 0, 0);
                    break;
                case ID_EDIT_CUT:
                    SendMessage(edit, WM_CUT, 0, 0);
                    break;
                case ID_FILE_OPEN:
                    {
                        OPENFILENAME ofn;
                        char szFile[260];
                        HANDLE hf;

                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                        ofn.nFilterIndex = 1;

                        if (GetOpenFileName(&ofn) == TRUE) {
                            hf = CreateFile(ofn.lpstrFile,
                                GENERIC_READ,
                                0,
                                (LPSECURITY_ATTRIBUTES)NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                (HANDLE)NULL);

                            DWORD dwFileSize = GetFileSize(hf, NULL);
                            if (dwFileSize != INVALID_FILE_SIZE) {
                                DWORD dwRead;
                                ReadFile(hf, g_szTextBuffer, dwFileSize, &dwRead, NULL);
                                g_szTextBuffer[dwFileSize] = '\0';
                                SetWindowText(edit, g_szTextBuffer);
                            }

                            CloseHandle(hf);
                        }
                    }
                    break;
                case ID_FILE_SAVE:
                    {
                        OPENFILENAME ofn;
                        char szFile[260];
                        HANDLE hf;

                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                        ofn.nFilterIndex = 1;

                        if (GetSaveFileName(&ofn) == TRUE) {
                            hf = CreateFile(ofn.lpstrFile,
                                GENERIC_WRITE,
                                0,
                                (LPSECURITY_ATTRIBUTES)NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                (HANDLE)NULL);

                            DWORD dwBytesWritten;
                            WriteFile(hf, g_szTextBuffer, strlen(g_szTextBuffer), &dwBytesWritten, NULL);

                            CloseHandle(hf);
                        }
                    }
                    break;
                case ID_HELP_ABOUT:
                    MessageBox(hwnd, "Text editor\nVersion 1.0\nCreated by Your Name", "About", MB_OK | MB_ICONINFORMATION);
                    break;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    HWND hwnd;
    hwnd =
        CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, "Text Editor", WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT, 350, 330, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

