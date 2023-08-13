#include <windows.h>
#include <string>
#include <filesystem>
#include <iterator>    
#include <sstream>
#include <vector>

#define onButtonClicked 5


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
HWND hwndButton;
HWND inputFormat;
HWND inputWord;
HWND outputPath;
HWND inputPath;


void searchFun();
void loadFile(LPCSTR path);
void mainFolderScan(LPCSTR path);
void nextFolderScan(LPCSTR pathLpc);
void buttonActivate(bool state);

std::vector<std::string> searchFormat;  // вектор с форматами которые ищутся
std::string result = "";                // выходная строка с путями файлов, в которых есть слово
std::string searchWord = "";            // строка для поиска


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    LPCTSTR lpzClass = TEXT("My Window Class!");

    if (!RegMyWindowClass(hInstance, lpzClass))
        return 1;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect); // разрешение экрана
    int x = screen_rect.right / 2 - 150;
    int y = screen_rect.bottom / 2 - 75;


    LPCSTR startType = ".txt .xml .lua .ini";
    LPCSTR srartWord = "1300901";
    LPCSTR startPath = "G:/game/bsNew/ZettaServer/data";


    HWND hWnd = CreateWindow(lpzClass, TEXT("Dialog Window"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, 620, 400, NULL, NULL, hInstance, NULL);

    hwndButton = CreateWindow("BUTTON", "search", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 250, 100, 30, hWnd, reinterpret_cast<HMENU>(onButtonClicked), NULL, NULL); 
    inputFormat = CreateWindow(TEXT("Edit"), TEXT(startType), WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 20, 550, 20, hWnd, NULL, NULL, NULL); 
    inputWord = CreateWindow(TEXT("Edit"), TEXT(srartWord), WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 50, 550, 20, hWnd, NULL, NULL, NULL); 
    inputPath = CreateWindow(TEXT("Edit"), TEXT(startPath), WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 80, 550, 20, hWnd, NULL, NULL, NULL); 
    outputPath = CreateWindow(TEXT("Edit"), TEXT(" "), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 30, 110, 550, 120, hWnd, NULL, NULL, NULL); 
    




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
            case onButtonClicked:
            searchFun();
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

// фоновый поток для скана папок
DWORD WINAPI thread2(LPVOID t)
{
    // получаем директорию для поиска
    LPCSTR mainPath = "";
    TCHAR buff[1024] = {0};
    GetWindowText(inputPath, buff, 1024);
    mainPath = buff;

    // начальный поиск внутри базовой папки
    nextFolderScan(mainPath);

    // вывод результатов
    if(result == "")
        SetWindowText(outputPath, "nothing in file");   
    else
        SetWindowText(outputPath, result.c_str());

    // завершение потока, и возврат состояния кнопки
    buttonActivate(true);
    return 0;
}

void searchFun()
{
    TCHAR buff[1024] = {0};

    // получаем форматы файлов
    GetWindowText(inputFormat, buff, 1024);
    std::istringstream iss(buff);   // строка с путями, потом в вектор
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(searchFormat));

    // получаем строку для поиска
    buff[1024] = {0};
    GetWindowText(inputWord, buff, 1024);
    searchWord = buff;

    // создание потока и изменение состояния кнопки
    HANDLE thread = CreateThread(NULL,0,thread2,NULL, 0, NULL);
    buttonActivate(false);
}

// смена состояний и названий кнопки
void buttonActivate(bool state)
{
    if(state == false)
        SetWindowText(hwndButton, "Waiting...");
    else
        SetWindowText(hwndButton, "search");
    
    EnableWindow(hwndButton,state);
}

// нахождение файлов и папок внутри папки
void nextFolderScan(LPCSTR pathLpc)
{
    std::string path = pathLpc;
   
    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        // берем каждый элемент в папке, и меняем у него в пути две черты на одну
        std::string tmp = entry.path().string();
        std::string replaceStr = "\\";
        int pos = tmp.find(replaceStr);
        if(pos > 0)
            tmp.replace(pos,1,"/");

        // если совпало расширение файла, то открываем и проверяем
        for (size_t i = 0; i < searchFormat.size(); i++)
        {
            int pos2 = tmp.find(searchFormat[i]);
            if(pos2 > 0)
                loadFile(tmp.c_str());
        }
        
        // если точки в названии не найдено, то значит это папка, и теперь проверяем внутри неё
        int pos1 = tmp.find('.');
        if(pos1 < 0)
            nextFolderScan(tmp.c_str());

    }
}

// поиск слов в тексте
int textEqual(std::string& text)
{

    int pos = text.find(searchWord);

    if(pos > 0)
        return 1;
    else
        return 0;
    
}

// загрука файла по указанному пути
void loadFile(LPCSTR path)
{
    int bufferSize = 160000;
    char textBuffer[bufferSize] {};  // была ошибка. без {} оставались данные на следующий круг и ложные срабатывания.

    HANDLE FileToLoad = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD bytesIterated;
    ReadFile(FileToLoad, textBuffer, bufferSize, &bytesIterated, NULL);

    std::string tempOut = textBuffer;

    int res = textEqual(tempOut);

    if(res == 1)
        result = result + path + "\n"; 

}

