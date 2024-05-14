#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>
#include <tchar.h>

char hardwareInfo[2048] = "";

void GetHardwareInfo() {
    // Получение информации о процессоре
    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    
    // Обработка архитектуры процессора
    char architecture[50];
    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            strcpy(architecture, "x64 (AMD64 or Intel EM64T)");
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            strcpy(architecture, "ARM");
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            strcpy(architecture, "ARM64");
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            strcpy(architecture, "Intel Itanium-based");
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            strcpy(architecture, "x86");
            break;
        default:
            strcpy(architecture, "Unknown");
            break;
    }

    snprintf(hardwareInfo, sizeof(hardwareInfo), "Processor Architecture: %s\n", architecture);
    
    // Получение информации о ревизии процессора
    snprintf(hardwareInfo + strlen(hardwareInfo), sizeof(hardwareInfo) - strlen(hardwareInfo),
             "Processor Revision: %d\n", sysInfo.wProcessorRevision);
    
    // Получение информации о версии операционной системы
    OSVERSIONINFOEX osInfo;
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (GetVersionEx((OSVERSIONINFO*)&osInfo)) {
        snprintf(hardwareInfo + strlen(hardwareInfo), sizeof(hardwareInfo) - strlen(hardwareInfo),
                 "Operating System Version: %lu.%lu (Build %lu)\n",
                 osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber);
    }
    
    // Получение информации о количестве ядер и потоков
    snprintf(hardwareInfo + strlen(hardwareInfo), sizeof(hardwareInfo) - strlen(hardwareInfo),
             "Number of Cores: %lu\n", sysInfo.dwNumberOfProcessors);
    
    // Получение информации о памяти
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    snprintf(hardwareInfo + strlen(hardwareInfo), sizeof(hardwareInfo) - strlen(hardwareInfo),
             "Total Physical Memory: %lld bytes\n", memInfo.ullTotalPhys);

    // Получение информации о диске
    ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
    GetDiskFreeSpaceEx(NULL, &freeBytes, &totalBytes, &totalFreeBytes);
    snprintf(hardwareInfo + strlen(hardwareInfo), sizeof(hardwareInfo) - strlen(hardwareInfo),
             "Total Disk Space: %lld bytes\n", totalBytes.QuadPart);
}

BOOL SaveReport(const char *report) {
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        FILE *file = fopen(ofn.lpstrFile, "w");
        if (file == NULL) {
            MessageBox(NULL, "Failed to open file for writing!", "Error", MB_ICONERROR | MB_OK);
            return FALSE;
        }

        fprintf(file, "%s", report);
        fclose(file);

        return TRUE;
    }

    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;

    switch (msg) {
        case WM_CREATE:
            GetHardwareInfo();
            hwndEdit = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY,
                                     10, 10, 300, 200, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"Button", L"Save Report", WS_VISIBLE | WS_CHILD, 10, 220, 100, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindowW(L"Button", L"Exit", WS_VISIBLE | WS_CHILD, 120, 220, 100, 30, hwnd, (HMENU)2, NULL, NULL);
            SetWindowText(hwndEdit, hardwareInfo);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    if (SaveReport(hardwareInfo)) {
                        MessageBox(NULL, "Report saved successfully!", "Info", MB_ICONINFORMATION | MB_OK);
                    }
                    break;
                case 2:
                    DestroyWindow(hwnd);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
    wc.lpszClassName = "HardwareInfoWindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd;
    hwnd = CreateWindowEx(0, "HardwareInfoWindowClass", "Hardware Info", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          350, 300, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

