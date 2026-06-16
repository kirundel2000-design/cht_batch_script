#include <WinSock2.h>
#include <WS2tcpip.h>
#define NOMIMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <uiautomation.h>
#include <uiautomationclient.h>
#include <tlHelp32.h>
#include <oleacc.h>
#include <QVector>
#include <QString>
#include <QDebug>

int getNumOfChilds(HWND hwnd){
    int counter=0;
    bool canFind=true;
    HWND child;
    child = GetWindow(hwnd, GW_CHILD);
    //qDebug()<<child;
    if(child != NULL) {counter++;}
    if(counter==1) while(canFind){
            child = GetWindow(child, GW_HWNDNEXT);
            if(child != NULL) {counter++; /*qDebug()<<child;*/}
            else {canFind=false;}
        }
    return counter;
}

void btnClick(HWND hwnd,IUIAutomation* automation = NULL){
    PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, 0);
    Sleep(10);
    PostMessage(hwnd, WM_LBUTTONUP, 0, 0);
}

int PrintRole(HWND hwnd)
{
    int answer =0;
    IAccessible* acc = nullptr;

    HRESULT hr = AccessibleObjectFromWindow(
        hwnd,
        OBJID_CLIENT,
        IID_IAccessible,
        (void**)&acc
        );

    if (SUCCEEDED(hr) && acc)
    {
        VARIANT v;
        v.vt = VT_I4;
        v.lVal = CHILDID_SELF;

        VARIANT role;
        VariantInit(&role);

        if (SUCCEEDED(acc->get_accRole(v, &role)))
        {
            if (role.vt == VT_I4)
                answer = role.lVal;
        }

        VariantClear(&role);
        acc->Release();
    }
    return answer;
}

void cutWindowName(QString& str) {
    int size = str.size();
    int indSlesh=size;
    for(int i=0;i<size;i++){
        if(str.at(i) == "\\"){
            indSlesh=i;
            break;
        }
    }
    QString answ;
    for(int i=0;i<indSlesh;i++){
        answ.push_back(str.at(i));
    }
    str = answ;
}

void mouseLeftClick()
{
    INPUT input[2] = {};

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(2, input, sizeof(INPUT));
}

void PressKey(WORD vk){
    INPUT input[2] = {};

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = vk;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = vk;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2,input,sizeof(INPUT));
    Sleep(80);
}

void ClickClientPoint(HWND hwnd, int clientX, int clientY)
{
    POINT pt{clientX, clientY};

    ShowWindow(hwnd, SW_RESTORE);
    SetForegroundWindow(hwnd);
    SetCursorPos(pt.x, pt.y);

    Sleep(20);
    mouseLeftClick();
    Sleep(250);
}

void doubleClickClientPoint(HWND hwnd, int clientX, int clientY)
{
    POINT pt{clientX, clientY};

    SetForegroundWindow(hwnd);
    SetCursorPos(pt.x, pt.y);

    Sleep(80);
    mouseLeftClick();
    Sleep(40);
    mouseLeftClick();

    Sleep(320);
}

void findViewPort(HWND& hwnd) {
    QVector<QString> names = {"qt_central_widget","aSplitter","aFrameIconView","aViewStack","aViewPage_Run","aRunCanvas","qt_viewport" };

    int size = names.size();


    for(int i=0;i<size;i++) {
        bool find=false;
        HWND child = GetWindow(hwnd, GW_CHILD);
        while(!find){
            wchar_t windowText[256] = L"";
            GetWindowTextW(child, windowText, 256);
            QString name = QString::fromWCharArray(windowText);
            cutWindowName(name);
            int role = PrintRole(child);
            if( (name == names[i]) && (role == 10)) {
                find = true;
            }
            else {
                child = GetWindow(child, GW_HWNDNEXT);
            }
        }
        hwnd = child;
    }
}

void findRunFrame(HWND& hwnd) {
    HWND child = GetWindow(hwnd, GW_CHILD);//переход к первому unnamed у которого в сиблингах прокрутка
    child = GetWindow(child, GW_CHILD);
    child = GetWindow(child, GW_HWNDNEXT); //почему то поломалось, возникает hwnd и не считывается inspector
    child = GetWindow(child, GW_CHILD);

    child = GetWindow(child, GW_HWNDNEXT);
    hwnd = child;
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam){
    wchar_t className[256];
    GetClassName(hwnd, className,256);

    qDebug()<<"HWND: "<< hwnd <<"Class: "<<className<<"\n";

    return TRUE;
}

bool findingFrame18(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="frame18") find=findingFrame18(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

bool findingStartStopSetTab(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="aBtnStartStop_SettingsTab") find=findingStartStopSetTab(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

void findStartFrame(HWND&hwnd){
    findingFrame18(hwnd);
    hwnd = GetWindow(hwnd, GW_CHILD);
    findingStartStopSetTab(hwnd);
}

void clickOK(HWND hwnd){
    int size = getNumOfChilds(hwnd);
    HWND child=GetWindow(hwnd, GW_CHILD);;
    for(int i=0;i<size;i++){
        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name=="aBtn0") {
            btnClick(child);
            break;
        }
        else {child=GetWindow(child, GW_HWNDNEXT);};
    }
}

bool findingOpenBtnHwnd(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="&Открыть") find=findingOpenBtnHwnd(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

bool findingSaveBtnHwnd(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="Со&хранить") find=findingSaveBtnHwnd(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

bool findingIceBcBtn(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="aAutoFileButton_ICE_FLUXBC") find=findingIceBcBtn(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

bool findingTabControl(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="tab control") find=findingTabControl(child);
        //if(name!="aBtnStartStop_GraphTab") find=findingStartStopSetTab(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

bool findingBcListView(HWND& hwnd){
    bool find=false;

    int size = getNumOfChilds(hwnd);
    if (size==0) return false;
    HWND child;
    for(int i=0;i<size;i++){
        if (i==0)  child = GetWindow(hwnd, GW_CHILD);
        else child = GetWindow(child, GW_HWNDNEXT);

        wchar_t className[256];
        GetWindowTextW(child, className,256);
        QString name = QString::fromWCharArray(className);
        cutWindowName(name);
        if(name!="aBCListView") find=findingBcListView(child);
        else {find=true;}

        if(find) {hwnd=child; break;}
    }
    return find;
}

void SendText(QString text){
    for(QChar c : text){
        SHORT vk = VkKeyScanA(c.unicode());

        if( vk== -1) continue;

        BYTE vkCode = vk & 0xFF;
        BYTE shiftState = (vk>>8) & 0xFF;

        if(shiftState & 1) PressKey(VK_SHIFT);
        PressKey(vkCode);

        if(shiftState & 1) {
            INPUT shiftUp = {};
            shiftUp.type = INPUT_KEYBOARD;
            shiftUp.ki.wVk = VK_SHIFT;
            shiftUp.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &shiftUp, sizeof(INPUT));
        }
    }
}
