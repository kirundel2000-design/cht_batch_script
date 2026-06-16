#include <QtCore/QCoreApplication>
#include <QtCore/qglobal.h>
#include <QDir>
#include "uiAutoFunc.h"
#include "ChangeFiles.h"

#define CANT_OPEN_BC 66
#define WASNT_START 78

typedef LONG (NTAPI *NtSuspendProcess_t)(HANDLE ProcessHandle);
typedef LONG (NTAPI *NtResumeProcess_t)(HANDLE ProcessHandle);


std::vector<DWORD> getPIDbyName(const std::wstring& processName){
    std::vector<DWORD> pids;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snap == INVALID_HANDLE_VALUE)
        return pids;
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if(Process32FirstW(snap,&entry)){
        do
        {
            auto thisDw = entry.szExeFile;
            if (processName == thisDw)
            {
                pids.push_back(entry.th32ProcessID);
            }
        } while (Process32NextW(snap,&entry));
    }

    CloseHandle(snap);
    return pids;
}

DWORD findChildProcess(DWORD parentPid, const std::wstring& name){
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);
    if(Process32FirstW(snap,&entry)){
        do{
            if(entry.th32ParentProcessID == parentPid &&
                name == entry.szExeFile)
            {
                CloseHandle(snap);
                return entry.th32ProcessID;
            }
        }while(Process32NextW(snap,&entry));
    }
    CloseHandle(snap);
    return 0;
}

int main(int argc, char* argv[])
{
    QFile log ("D:\\Engineers_files\\BarIcePrj\\Cht_Batch_Res.txt");
    log.open(QIODevice::WriteOnly|QIODevice::Truncate);
    QTextStream in(&log);
    in<<0;
    log.close();

    QString path;
    int heating;
    int updateInterface;
    int whatsIps;

    if(argc==1){
        QFile* Bc = new QFile("D:\\Engineers_files\\BarIcePrj\\CHTStart.txt");
        QTextStream* ts = new QTextStream;
        if(Bc->open(QIODevice::ReadOnly)){
            ts->setDevice(Bc);
        } else return CANT_OPEN_BC;
        QString thisLn;
        thisLn = ts->readLine(); path = thisLn;
        thisLn = ts->readLine(); updateInterface = thisLn.toInt();
        thisLn = ts->readLine(); whatsIps = thisLn.toInt();
        Bc->close();
    }
    else
    {
        path = argv[1];
        updateInterface = QString(argv[2]).QString::toInt();
        whatsIps = QString(argv[3]).QString::toInt();
    }

    DWORD fensPid; std::vector<DWORD> pids;
    while(pids.empty()) pids = getPIDbyName(L"fensapiceGUI.exe");
    fensPid = pids.front() ;pids.clear();
    QString nativePath = QDir::fromNativeSeparators(path);

    CoInitialize(NULL);
    IUIAutomation* automation = NULL;
    CoCreateInstance(
        CLSID_CUIAutomation,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IUIAutomation,
        (void**)&automation
    );
    
    //Закомментировать если не нужно обновлять файлы сетки
    if(updateInterface==1){
        UpdateMesh(path);
    }

    if(whatsIps == 1){
        TurnOnIPS();
    }
    else if(whatsIps == 0){
        TurnOffIPS();
    }


    //ИСПРАВИТЬ ПОД СВОЮ ВЕРСИЮ FENSAP
    HWND hwnd = FindWindow(NULL, L"FENSAP-ICE [2024 R2]");
    HWND ViewPort;

    ShowWindow(hwnd, SW_RESTORE);   // восстановить если свернуто
    ShowWindow(hwnd, SW_MAXIMIZE);  // развернуть
    SetForegroundWindow(hwnd);      // сделать активным

    IUIAutomationElement* root = NULL;

    automation->ElementFromHandle(hwnd, &root);

    findViewPort(hwnd); ViewPort = hwnd;

    //ВВЕСТИ СВОЮ КООРДИНАТЫ
    doubleClickClientPoint(hwnd,55,165);
    if(updateInterface==1) Sleep(8000);
    else Sleep(2400);

    hwnd = FindWindow(NULL, L"Warning");
    if(hwnd!=NULL) {clickOK(hwnd); Sleep(400);}
    //Если выскакакивает warning жмем ок

    hwnd = FindWindow(NULL, L"CHT3D");
    ShowWindow(hwnd, SW_RESTORE);   // восстановить если свернуто
    ShowWindow(hwnd, SW_MAXIMIZE);  // развернуть
    SetForegroundWindow(hwnd);      // сделать активным
    Sleep(600);

    findRunFrame(hwnd);
    btnClick(hwnd);
    Sleep(400);

    hwnd = FindWindow(NULL, L"CHT3D");
    findStartFrame(hwnd);
    btnClick(hwnd);

    Sleep(750);

    hwnd = FindWindow(NULL, L"Information");
    if(hwnd!=NULL) clickOK(hwnd);

    Sleep(1250);

    hwnd = FindWindow(NULL, L"Warning");
    if(hwnd!=NULL) clickOK(hwnd);


    bool itsAll = false;


    DWORD ntiShPid=0; //проверяем запустился ли расчетный алгоритм
    int iterat = 0;
    while (ntiShPid==0){
        ntiShPid=findChildProcess(fensPid, L"nti_sh.exe");
        Sleep(100);
        iterat++;
        if(iterat>= 30)
            break;
    }

    //qDebug()<<ntiShPid;

    HANDLE hProcess = OpenProcess(SYNCHRONIZE,FALSE,ntiShPid);
    if(hProcess){ //ждем окончания расчета
        //qDebug()<<"Start watching";
        WaitForSingleObject(hProcess, INFINITE);
        CloseHandle(hProcess);
        //qDebug()<<"End Watching";
    }

    Sleep(200);
    hwnd = FindWindow(NULL, L"CHT3D");
    PostMessage(hwnd, WM_CLOSE,0,0);
    //int a=2;

    log.open(QIODevice::WriteOnly|QIODevice::Truncate);
    in.setDevice(&log);
    in<<1;
    log.close();

    return 0;
}

