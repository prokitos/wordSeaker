#include <windows.h>
#include <string>
#include <filesystem>

#define onButtonClicked 5

// объявление функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
HWND hwndButton;
HWND inputFirst;
HWND inputSecond;
HWND inputThird;


void testFunc();
void loadFile(LPCSTR path);
void FolderScan();
void nextFolderScan(LPCSTR pathLpc);

std::string result = "";

// функция вхождений программы WinMain
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // имя будущего класса
    LPCTSTR lpzClass = TEXT("My Window Class!");

    // регистрация класса
    if (!RegMyWindowClass(hInstance, lpzClass))
        return 1;

    // вычисление координат центра экрана
    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect); // разрешение экрана
    int x = screen_rect.right / 2 - 150;
    int y = screen_rect.bottom / 2 - 75;




    // создание диалогового окна
    HWND hWnd = CreateWindow(lpzClass, TEXT("Dialog Window"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, 700, 250, NULL, NULL, hInstance, NULL);

    hwndButton = CreateWindow("BUTTON", "OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 50, 50, hWnd, reinterpret_cast<HMENU>(onButtonClicked), NULL, NULL);     

    inputFirst = CreateWindow(TEXT("Edit"), TEXT("txt xml"), WS_CHILD | WS_VISIBLE | WS_BORDER, 150, 20, 450, 20, hWnd, NULL, NULL, NULL); 
    inputSecond = CreateWindow(TEXT("Edit"), TEXT("search word"), WS_CHILD | WS_VISIBLE | WS_BORDER, 150, 50, 450, 20, hWnd, NULL, NULL, NULL); 
    inputThird = CreateWindow(TEXT("Edit"), TEXT(" "), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 150, 80, 450, 80, hWnd, NULL, NULL, NULL); 



    // если окно не создано, описатель будет равен 0
    if (!hWnd) return 2;

    // цикл сообщений приложения
    MSG msg = { 0 };    // структура сообщения
    int iGetOk = 0;   // переменная состояния
    while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0) // цикл сообщений
    {
        if (iGetOk == -1) return 3;  // если GetMessage вернул ошибку - выход
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;  // возвращаем код завершения программы
}

////////////////////////////////////////////////////////////////////////// 
// функция регистрации класса окон
ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
    WNDCLASS wcWindowClass = { 0 };
    // адрес ф-ции обработки сообщений
    wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
    // стиль окна
    wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
    // дискриптор экземпляра приложения
    wcWindowClass.hInstance = hInst;
    // название класса
    wcWindowClass.lpszClassName = lpzClassName;
    // загрузка курсора
    wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // загрузка цвета окон
    wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
    return RegisterClass(&wcWindowClass); // регистрация класса
}

////////////////////////////////////////////////////////////////////////// 
// функция обработки сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // выборка и обработка сообщений
    switch (message)
    {
    case WM_COMMAND:
        switch (wParam)
        {
            case onButtonClicked:
            testFunc();
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);  // реакция на сообщение
        break;
    default:
        // все сообщения не обработанные Вами обработает сама Windows
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void testFunc()
{
    std::string temp;
    TCHAR buff[1024] = {0};
    GetWindowText(inputFirst, buff, 1024);
    temp = buff;
    int k = 0;

    //LPCSTR testPath = "G:\\game\\testFolder\\";
    //loadFile(testPath);
    FolderScan();
}


// начальный поиск внутри базовой папки
void FolderScan()
{
    
    std::string path = "G:/game/bsNew/ZettaServer/server_data";
    std::string path = "G:/game/testFolder";

    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        std::string tmp = entry.path().string();
        std::string replaceStr = "\\";
        int pos = tmp.find(replaceStr);

        if(pos > 0)
            tmp.replace(pos,1,"/");

        int pos1 = tmp.find('.');

        if(pos1 < 0)
            nextFolderScan(tmp.c_str());
        else
            loadFile(tmp.c_str());
    }

    // после всех сканов, вывести ничего если не найдено
    if(result == "")
        SetWindowText(inputThird, "nothing in file");   
    else
        SetWindowText(inputThird, result.c_str());
    

}

// нахождение папки внутри папки
void nextFolderScan(LPCSTR pathLpc)
{
    std::string path = pathLpc;
   
    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        std::string tmp = entry.path().string();
        std::string replaceStr = "\\";
        int pos = tmp.find(replaceStr);

        if(pos > 0)
            tmp.replace(pos,1,"/");

        int pos1 = tmp.find('.');
        int pos2 = tmp.find(".xml");

        if(pos1 < 0)
            nextFolderScan(tmp.c_str());
        if(pos2 > 0)
            loadFile(tmp.c_str());
    }
}

// поиск слов в тексте
int textEqual(std::string& text)
{
    std::string search = "Star_Blue";

    int pos = text.find(search);

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

