
#include <windows.h>
#include <string>
#include <filesystem>
#include <iterator>    
#include <sstream>
#include <vector>
#include <shlobj_core.h>

#define buttonSearchClick 5
#define buttonSelectClick 6


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);

HWND hwndButton;
HWND hwndFindButton;
HWND inputFormat;
HWND inputWord;
HWND outputPath;
HWND inputPath;


void searchFun();
void loadFile(LPCSTR path);
void mainFolderScan(LPCSTR path);
void nextFolderScan(LPCSTR pathLpc);
void buttonActivate(bool state);

std::vector<std::string> searchFormat;  // ������ � ��������� ������� ������
std::string result = "";                // �������� ������ � ������ ������, � ������� ���� �����
std::string searchWord = "";            // ������ ��� ������


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    LPCTSTR lpzClass = TEXT("My Window Class!");

    if (!RegMyWindowClass(hInstance, lpzClass))
        return 1;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect); // ���������� ������
    int x = screen_rect.right / 2 - 150;
    int y = screen_rect.bottom / 2 - 75;


    LPCSTR startType = ".txt .xml .lua .ini";
    LPCSTR srartWord = "1300901";
    LPCSTR startPath = "G:/game/bsNew/ZettaServer/data";


    HWND hWnd = CreateWindow(lpzClass, TEXT("Dialog Window"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, 620, 400, NULL, NULL, hInstance, NULL);

    hwndButton = CreateWindowA("BUTTON", "search", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 150, 250, 100, 30, hWnd, reinterpret_cast<HMENU>(buttonSearchClick), NULL, NULL);
    hwndFindButton = CreateWindowA("BUTTON", "folder select", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 350, 250, 100, 30, hWnd, reinterpret_cast<HMENU>(buttonSelectClick), NULL, NULL);

    inputFormat = CreateWindowA("Edit", startType, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 20, 550, 20, hWnd, NULL, NULL, NULL);
    inputWord = CreateWindowA("Edit", srartWord, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 50, 550, 20, hWnd, NULL, NULL, NULL);
    inputPath = CreateWindowA("Edit", startPath, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 80, 550, 20, hWnd, NULL, NULL, NULL);
    outputPath = CreateWindowA("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 30, 110, 550, 120, hWnd, NULL, NULL, NULL);





    if (!hWnd) return 2;

    MSG msg = { 0 };
    int iGetOk = 0;
    while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (iGetOk == -1) return 3;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}


ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
    WNDCLASS wcWindowClass = { 0 };
    wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
    wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
    wcWindowClass.hInstance = hInst;
    wcWindowClass.lpszClassName = lpzClassName;
    wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
    return RegisterClass(&wcWindowClass);
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case buttonSearchClick:
            searchFun();
            break;
        case buttonSelectClick:

            char pchSelectedF[MAX_PATH] = "";

            BROWSEINFO bi =
            {
                hWnd,
                0,
                (LPWSTR)pchSelectedF,
                L"�������� �����",
                0, 0, 0, 0
            };

            TCHAR szDir[MAX_PATH];
            LPITEMIDLIST lpItem = SHBrowseForFolder(&bi);
            SHGetPathFromIDList(lpItem, szDir);

            LPWSTR temp1 = szDir;
            size_t nLength;
            wcstombs_s(&nLength, NULL, 0, temp1, 0);
            LPSTR temp2 = new CHAR[nLength];
            wcstombs_s(&nLength, temp2, nLength, temp1, nLength);

            SetWindowTextA(inputPath, temp2);

            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// ������� ����� ��� ����� �����
DWORD WINAPI thread2(LPVOID t)
{
    // �������� ���������� ��� ������
    LPCSTR mainPath = "";
    TCHAR buff[1024] = { 0 };
    GetWindowText(inputPath, buff, 1024);

    // ������� ���������� � ���������� ���
    LPWSTR temp1 = buff;
    size_t nLength;
    wcstombs_s(&nLength, NULL, 0, temp1, 0);
    LPSTR temp2 = new CHAR[nLength];
    wcstombs_s(&nLength, temp2, nLength, temp1, nLength);
    mainPath = temp2;

    // ��������� ����� ������ ������� �����
    nextFolderScan(mainPath);

    // ����� �����������
    if (result == "")
        SetWindowTextA(outputPath, "nothing in file");
    else
        SetWindowTextA(outputPath, result.c_str());

    // ���������� ������, � ������� ��������� ������
    buttonActivate(true);
    return 0;
}

void searchFun()
{
    TCHAR buff[1024] = { 0 };

    // �������� ������� ������
    GetWindowText(inputFormat, buff, 1024);

    // ������� �������� � ���� ���
    LPWSTR temp1 = buff;
    size_t nLength;
    wcstombs_s(&nLength, NULL, 0, temp1, 0);
    LPSTR temp2 = new CHAR[nLength];
    wcstombs_s(&nLength, temp2, nLength, temp1, nLength);


    std::istringstream iss(temp2);   // ������ � ������, ����� � ������
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(searchFormat));

    // �������� ������ ��� ������
    buff[1023] = { 0 };
    GetWindowText(inputWord, buff, 1024);

    // ������� �������� � ���� ���
    temp1 = buff;
    wcstombs_s(&nLength, NULL, 0, temp1, 0);
    temp2 = new CHAR[nLength];
    wcstombs_s(&nLength, temp2, nLength, temp1, nLength);

    searchWord = temp2;

    // �������� ������ � ��������� ��������� ������
    HANDLE thread = CreateThread(NULL, 0, thread2, NULL, 0, NULL);
    buttonActivate(false);
}

// ����� ��������� � �������� ������
void buttonActivate(bool state)
{
    if (state == false)
        SetWindowText(hwndButton, L"Waiting...");
    else
        SetWindowText(hwndButton, L"search");

    EnableWindow(hwndButton, state);
}

// ���������� ������ � ����� ������ �����
void nextFolderScan(LPCSTR pathLpc)
{
    std::string path = pathLpc;


    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        // ����� ������ ������� � �����, � ������ � ���� � ���� ��� ����� �� ����
        std::string tmp = entry.path().string();
        std::string replaceStr = "\\";
        int pos = tmp.find(replaceStr);
        if (pos > 0)
            tmp.replace(pos, 1, "/");

        // ���� ������� ���������� �����, �� ��������� � ���������
        for (size_t i = 0; i < searchFormat.size(); i++)
        {
            int pos2 = tmp.find(searchFormat[i]);
            if (pos2 > 0)
                loadFile(tmp.c_str());
        }

        // ���� ����� � �������� �� �������, �� ������ ��� �����, � ������ ��������� ������ ��
        int pos1 = tmp.find('.');
        if (pos1 < 0)
            nextFolderScan(tmp.c_str());

    }
}

// ����� ���� � ������
int textEqual(std::string& text)
{

    int pos = text.find(searchWord);

    if (pos > 0)
        return 1;
    else
        return 0;

}

// ������� ����� �� ���������� ����
void loadFile(LPCSTR path)
{
    int bufferSize = 160000;
    char textBuffer[160000]{};  // ���� ������. ��� {} ���������� ������ �� ��������� ���� � ������ ������������.

    HANDLE FileToLoad = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD bytesIterated;
    ReadFile(FileToLoad, textBuffer, bufferSize, &bytesIterated, NULL);

    std::string tempOut = textBuffer;

    int res = textEqual(tempOut);

    if (res == 1)
        result = result + path + "\n";

}

