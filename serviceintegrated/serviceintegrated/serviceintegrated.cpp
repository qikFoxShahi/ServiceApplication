#include<windows.h>
#include <tchar.h>
#include <strsafe.h>
#include<string>
#include<stack>
#include<vector>
#include<iostream>
#include<thread>
using namespace std;
#pragma comment(lib, "advapi32.lib")
#define SERVICE_NAME TEXT("ASPSERVICEVS")
SERVICE_STATUS ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;
HANDLE hServiceEvent = NULL;
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv);
void WINAPI ServiceControlHandler(DWORD dwControl);
void ServiceReportStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint);
void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv);
void ServiceInstall(void);
void ServiceDelete(void);
int ServiceStart(void);
void ServiceStop(void);
void CreateLog(TCHAR* filePath);

int __cdecl _tmain(int argc, TCHAR* argv[])
{
    vector<TCHAR*> vec;
    thread th[5];
    int j = 0;
    BOOL bStServiceCtrlDispatcher = 0;
    printf("Int Main Function Start\n");
    //ServiceInstall();
    // ServiceDelete();
     //ServiceStart();
    if (lstrcmpi(argv[1], L"install") == 0)
    {
        ServiceInstall();
        printf("Installation (Main) Success\n");
        return 0;
    }
    else

        if (lstrcmpi(argv[1], L"delete") == 0)
        {
            ServiceDelete();
            printf("ServiceDelete (Main)\n");
            return 0;
        }
        else
            if (lstrcmpi(argv[1], TEXT("start")) == 0)
            {
                if (ServiceStart())
                {
                    DWORD d = GetLogicalDrives();
                    // printf("getlogicaldrive d= %d\n", d);
                    if (argc <= 2)
                    {
                        TCHAR self[] = L".";
                        CreateLog(self);
                        //ServiceStart(self);
                    }
                    else
                    {
                        if ((argc >= 3) && (lstrcmpi(argv[2], TEXT("all")) != 0))
                        {
                            printf("First block\n");
                            CreateLog(argv[2]);
                        }
                        else
                        {
                            printf("second block:1\n");
                            if ((argc == 3) && (lstrcmpi(argv[2], TEXT("all")) == 0))
                            {
                                printf("second block:2\n");
                                DWORD d = GetLogicalDrives();
                                printf("d=:- %d\n", d);
                             //   thread th[5];
                              //  int j = 0;
                                for (int i = 0; i < 26; i++)
                                {
                                    TCHAR drive[MAX_PATH] = L"";
                                    if ((d & (1 << i)) && (i!=2))
                                    {
                                       // if (i == 2)
                                         //   continue;
                                        drive[0] = wchar_t('A' + i);
                                        drive[1] = wchar_t(':');
                                        vec.push_back(drive);
                                        j++;
                                        printf("Drive name:-  %c%c\n", drive[0], drive[1]);
                                        //getchar();
                                       // CreateLog(drive);
                                       // th[j++] = thread(CreateLog, drive);
                                        
                                       // TCHAR self[] = L".";
                                        //ServiceStart(self);
                                    }
                                }
                                for (int i = 0; i < j; i++)
                                {
                                    th[i] = thread(CreateLog, vec[i]);
                                    _tprintf(TEXT("Thread for %c\n"),vec[i][0]);
                                }
                                
                                //CreateLog(argv[2]);
                            }
                            else
                                printf("error block\n");
                        }
                    }
                }
                printf("Service Start(Main)\n");
            }
            else
                if (lstrcmpi(argv[1], TEXT("stop")) == 0)
                {
                    ServiceStop();
                    printf("Service Stop(Main)\n");
                }
    /*  else
          if (lstrcmpi(argv[1], TEXT("createlog")) == 0)
          {
              CreateLog(argv[2]);
          }*/
                else
                {

                    SERVICE_TABLE_ENTRY DispatchTable[] =
                    {
                        {LPWSTR(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
                        {NULL,NULL}
                    };
                    bStServiceCtrlDispatcher = StartServiceCtrlDispatcher(DispatchTable);
                    //    printf("val:-")bStServiceCtrlDispatcher)endl;
                    if (FALSE == bStServiceCtrlDispatcher)
                    {

                        printf("StartServiceCtrlDispatcher Failed\n");
                        printf("ERROR CODE:-%d \n", GetLastError());

                    }
                    else
                    {
                        printf("StartServiceCtrlDispatcher Success\n");
                    }
                }

    printf("Int main Function End\n");
    for (int i = 0; i < j; i++)
    {
        if (th[i].joinable())
        {
            printf("thread %d joining\n", i);
            th[i].join();
           
        }
        else
        {
            printf("thread %d unjoinable\n",i);
        }
    }
    system("PAUSE");
    return 0;
}
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv)
{
    printf("ServiceMain Start\n");
    BOOL bServiceStatus = FALSE;
    hServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceControlHandler);
    if (NULL == hServiceStatusHandle)
    {
        printf("RegisterServiceCtrlHandler Failed\n");
        printf("Error Code:- %d \n", GetLastError());

    }
    else
    {
        printf("RegisterServiceCtrlHandler Success\n");
    }
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
    /* bServiceStatus=SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
     if(FALSE==bServiceStatus)
     {
         printf("Service Status Initial Setup Failed\n");
         //printf("Error Code:- ")GetLastError();
     }
     else
     {
         printf("Service Status Initial Setup Success\n");
     }
     */
    ServiceInit(dwArgc, lpArgv);
    printf("ServiceMain End\n");

}
void WINAPI ServiceControlHandler(DWORD dwControl)
{
    printf("Service Control Handler begin\n");
    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:
    {
        ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
        printf("Service Stopped\n");
        SetEvent(hServiceEvent);
        ServiceReportStatus(ServiceStatus.dwCurrentState, NO_ERROR, 0);
        // break;
        return;

    }
    default:
        break;
    }
    printf("Service Control Handler end\n");
}
void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv)
{
    printf("ServiceInit Start\n");
    hServiceEvent = CreateEvent(NULL,
        TRUE,
        FALSE,
        NULL);
    if (NULL == hServiceEvent)
    {
        ServiceReportStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }
    else
    {
        ServiceReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
    }
    while (1)
    {
        WaitForSingleObject(hServiceEvent, INFINITE);
        ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }
    printf("ServiceInit End\n");
}
void ServiceReportStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint)
{
    printf("ServiceReportStatus Start\n");
    static DWORD dwCheckPoint = 1;
    BOOL bSetServiceStatus = FALSE;
    ServiceStatus.dwCurrentState = dwCurrentState;
    ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    ServiceStatus.dwWaitHint = dwWaitHint;
    if (dwCurrentState == SERVICE_START_PENDING)
    {
        ServiceStatus.dwControlsAccepted = 0;

    }
    else
    {
        ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;;
        // ServiceStatus.dwCheckPoint=dwCheckPoint++;
    }
    if (dwCurrentState == SERVICE_RUNNING || dwCurrentState == SERVICE_STOPPED)
    {
        ServiceStatus.dwCheckPoint = 0;
    }
    else
        ServiceStatus.dwCheckPoint = dwCheckPoint++;
    bSetServiceStatus = SetServiceStatus(hServiceStatusHandle,
        &ServiceStatus);
    if (FALSE == bSetServiceStatus)
    {
        printf("Service Status Failed\n");
    }
    else
        printf("Service Status Success\n");

    printf("ServiceReportStatus End\n");
}
void ServiceInstall(void)
{
    printf("Service Install Start\n");
    SC_HANDLE hScOpenSCManager = NULL;
    SC_HANDLE hScCreateService = NULL;
    DWORD dwGetModuleFileName = 0;
    TCHAR szPath[MAX_PATH];
    dwGetModuleFileName = GetModuleFileName(NULL,
        szPath,
        MAX_PATH);
    if (0 == dwGetModuleFileName)
    {
        printf("Service Installation Failed\n");
        printf("Error No:- %d \n", GetLastError());

    }
    else
    {
        printf("Successfully installed\n");
    }
    hScOpenSCManager = OpenSCManager(NULL,
        NULL,
        SC_MANAGER_ALL_ACCESS
    );
    if (NULL == hScOpenSCManager)
    {
        printf("OpenSCMAnager Failed\n");
        printf("Error No:-%d \n", GetLastError());
    }
    else
    {
        printf("OpenSCMAnager Success\n");
    }
    hScCreateService = CreateService(
        hScOpenSCManager,
        SERVICE_NAME,
        SERVICE_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        szPath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );
    if (NULL == hScCreateService)
    {
        printf("CreateService Failed\n");
        printf("Error no:- %d \n", GetLastError());
        CloseServiceHandle(hScOpenSCManager);
    }
    else
    {
        printf("CreateService Success\n");
    }
    CloseServiceHandle(hScCreateService);
    CloseServiceHandle(hScOpenSCManager);
    printf("ServiceInstall End\n");
}
void ServiceDelete(void)
{
    printf("ServiceDelete Start\n");
    SC_HANDLE hScOpenSCManager = NULL;
    SC_HANDLE hScOpenService = NULL;
    BOOL bDeleteService = FALSE;
    hScOpenSCManager = OpenSCManager(NULL,
        NULL,
        SC_MANAGER_ALL_ACCESS);
    if (NULL == hScOpenSCManager)
    {
        printf("OpenSCManager Failed\n");
        printf("Error no:-%d \n", GetLastError());
    }
    else
    {
        printf("OpenSCManager Success\n");
    }
    hScOpenService = OpenService(hScOpenSCManager,
        SERVICE_NAME,
        SERVICE_ALL_ACCESS
    );
    if (NULL == hScOpenService)
    {
        printf("OpenService Failed\n");
        printf("Error No:- %d \n", GetLastError());

    }
    else
    {
        printf("OpenService Success \n");
    }

    bDeleteService = DeleteService(hScOpenService);
    if (FALSE == bDeleteService)
    {
        printf("Delete Service Failed\n");
        printf("Error no:- %d \n", GetLastError());

    }
    else
    {
        printf("Delete Service Success\n");
    }
    CloseServiceHandle(hScOpenService);
    CloseServiceHandle(hScOpenSCManager);
    printf("ServiceDelete End\n");
}

int ServiceStart(void)
{
    int flag = 0;
    printf("Inside ServiceStart Function\n");
    BOOL bStartService = FALSE;
    SERVICE_STATUS_PROCESS SvcStatusProcess;
    SC_HANDLE hOpenSCManager = NULL;
    SC_HANDLE hOpenService = NULL;
    BOOL bQueryServiceStatus = FALSE;
    DWORD dwBytesNeeded;
    hOpenSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == hOpenSCManager)
    {
        printf("hOpenSCManager Failed\n");
        printf("Error No:- %d\n", GetLastError());
    }
    else
    {
        printf("hOpenSCManager Success\n");
    }
    hOpenService = OpenService(hOpenSCManager, SERVICE_NAME, SC_MANAGER_ALL_ACCESS);
    if (NULL == hOpenService)
    {
        printf("hOpenService Failed\n");
        printf("Error No:- %d\n", GetLastError());
    }
    else
    {
        printf("hOpenService Success\n");
    }
    bQueryServiceStatus = QueryServiceStatusEx(hOpenService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&SvcStatusProcess,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded);
    if (FALSE == bQueryServiceStatus)
    {
        printf("QueryService Failed\n");
        printf("Error No:- %d\n", GetLastError());
    }
    else
    {
        printf("QueryService Success\n");
    }
    if ((SvcStatusProcess.dwCurrentState != SERVICE_STOPPED) && (SvcStatusProcess.dwCurrentState != SERVICE_STOP_PENDING))
    {
        printf("Service is already running\n");
    }
    else
    {
        printf("Service is already stopped\n");
    }
    while (SvcStatusProcess.dwCurrentState == SERVICE_STOP_PENDING)
    {
        bQueryServiceStatus = QueryServiceStatusEx(hOpenService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&SvcStatusProcess,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded);
        if (FALSE == bQueryServiceStatus)
        {
            printf("QueryService Failed\n");
            printf("Error No:- %d\n", GetLastError());
            CloseServiceHandle(hOpenService);
            CloseServiceHandle(hOpenSCManager);
        }
        else
        {
            printf("QueryService Success\n");
        }
    }
    bStartService = StartService(hOpenService,
        NULL,
        NULL);

    if (FALSE == bStartService)
    {
        printf("ServiceStart Failed\n");
        printf("Error No:- %d\n", GetLastError());
    }
    else
    {
        printf("ServiceStart success\n");
        flag = 1;
       /* if ((lstrcmpi(filepath, TEXT("all")) == 0))
        {
            printf("second block:2\n");
            DWORD d = GetLogicalDrives();
            printf("d=:- %d\n", d);
            thread th[5];
            int j=0;
            for (int i = 0; i < 26; i++)
            {
                TCHAR drive[MAX_PATH] = L"";
                if (d & (1 << i))
                {
                    if (i == 2||i==5||i==6)
                        continue;

                    drive[0] = wchar_t('A' + i);
                    drive[1] = wchar_t(':');
                    th[j++] = thread(CreateLog, drive);
                    //  if ((lstrcmpi(drive[0], L"C") == 0))
                      //    continue;
                    //cout << TEXT("in drive:") << drive << endl;
                 //   CreateLog(drive);
                    //   printf("Drive name:-  %c%c\n", drive[0],drive[1]);
                      // TCHAR self[] = L".";
                       //ServiceStart(self);
                }
            }
            for (int i = 0; i < j; i++)
            {
                th[i].join();
            }
        }
        else
        {
            CreateLog(filepath);
        }*/
    }
    bQueryServiceStatus = QueryServiceStatusEx(
        hOpenService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&SvcStatusProcess,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded);
    if (FALSE == bQueryServiceStatus)
    {
        printf("QueryService Failed\n");
        printf("Error No:- %d\n", GetLastError());
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hOpenSCManager);
    }
    else
    {
        printf("QueryService Success\n");
    }
    if (SvcStatusProcess.dwCurrentState == SERVICE_RUNNING)
    {
        printf("Service Started Running..............\n");

    }
    else
    {
        printf("Service Running Failed\n");
        printf("Error No:- %d\n", GetLastError());
        CloseServiceHandle(hOpenService);
        CloseServiceHandle(hOpenSCManager);
    }
    CloseServiceHandle(hOpenService);
    CloseServiceHandle(hOpenSCManager);
    printf("ServiceStart function END\n");
    return flag;
}
void ServiceStop()
{
    printf("Inside Service Stop\n");
    SERVICE_STATUS_PROCESS SvcStatusProcess;
    SC_HANDLE hScOpenManager = NULL;
    SC_HANDLE hScOpenService = NULL;
    BOOL bQueryServiceStatus = TRUE;
    BOOL bControlService = TRUE;
    DWORD dwBytesNeeded;
    hScOpenManager = OpenSCManager(NULL,
        NULL,
        SC_MANAGER_ALL_ACCESS);
    if (NULL == hScOpenManager)
    {
        printf("OpenSCManager Failed\n");
        printf("Error no:- %d\n", GetLastError());
    }
    else
        printf("OpenSCManager Success\n");
    hScOpenService = OpenService(hScOpenManager,
        SERVICE_NAME,
        SC_MANAGER_ALL_ACCESS);
    if (NULL == hScOpenService)
    {
        printf("OpenSCService Failed\n");
        printf("Error no:- %d\n", GetLastError());
        CloseServiceHandle(hScOpenManager);
    }
    else
        printf("OpenSCService Success\n");
    bQueryServiceStatus = QueryServiceStatusEx(hScOpenService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&SvcStatusProcess,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded);
    if (FALSE == bQueryServiceStatus)
    {
        printf("QueryServiceFailed\n");
        printf("Error no:- %d\n", GetLastError());
        CloseServiceHandle(hScOpenService);
        CloseServiceHandle(hScOpenManager);
    }
    else
        printf("QueryServiceSuccess\n");

    bControlService = ControlService(hScOpenService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&SvcStatusProcess);
    if (FALSE == bControlService)
    {
        printf("ControlService Failed\n");
        printf("Error no:- %d\n", GetLastError());
        CloseServiceHandle(hScOpenService);
        CloseServiceHandle(hScOpenManager);
    }
    else
        printf("Control Service Success\n");
    while (SvcStatusProcess.dwCurrentState != SERVICE_STOPPED)
    {
        bQueryServiceStatus = QueryServiceStatusEx(hScOpenService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&SvcStatusProcess,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded);
        if (FALSE == bQueryServiceStatus)
        {
            printf("QueryServiceFailed\n");
            printf("Error no:- %d\n", GetLastError());
            CloseServiceHandle(hScOpenService);
            CloseServiceHandle(hScOpenManager);
        }
        else
            printf("QueryServiceSuccess\n");
        if (SvcStatusProcess.dwCurrentState == SERVICE_STOPPED)
        {
            printf("SERVICE STOPPED SUCCESS\n");
            break;
        }
        else
        {
            printf("Service Stopped Failed\n");
            printf("Error no:- %d\n", GetLastError());
            CloseServiceHandle(hScOpenService);
            CloseServiceHandle(hScOpenManager);
        }

    }
    CloseServiceHandle(hScOpenService);
    CloseServiceHandle(hScOpenManager);
    printf("Service stop end\n");
}
void CreateLog(TCHAR* filePath)
{
    wstring mask = L"*";
    wstring path = filePath;
    //wstring path = L"E:\\c++";
   // vector<wstring> files;
    //cout << path.c_str();
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    wstring spec;
    wstring temp;
    stack<wstring> directories;

    directories.push(path);
    // files.clear();

    while (!directories.empty()) {
        //cout << endl << "Inside while" << endl;
        path = directories.top();
        spec = path + L"\\" + mask;
        // _tprintf(TEXT("spec= %s\n"), spec.c_str());
        // _tprintf(TEXT("Inside Folder %s\n"), directories.top().c_str());
        directories.pop();
        wstring p = path + L"\\" + L"DemoLogFileFInal.txt";
        //_tprintf(TEXT("p=%s\n"), p.c_str());
        //string t = TEXT(directories.top().c_str());

        HANDLE hFile = CreateFile(
            p.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
           // printf(" Failed to open/create file\n");
            //  return 2;
        }
        //else
           // printf("file created\n");

        // CloseHandle(hFile);
        hFind = FindFirstFile((spec.c_str()), &ffd);
        if (hFind == INVALID_HANDLE_VALUE) {
            // return false;
          //  printf("Dindn't find any file\n");
        }

        do {
            //cout<<"inside do while"<<endl;
            if (wcscmp(ffd.cFileName, L".") != 0 &&
                wcscmp(ffd.cFileName, L"..") != 0 && (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) && (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)))
            {
                if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    temp = path + L"\\" + ffd.cFileName;
                    directories.push(temp);
                  //  _tprintf(TEXT("PUSHED Folder name %s\n"), ffd.cFileName);

                }
                else {
                    temp = path + L"\\" + ffd.cFileName;
                    //files.push_back(temp);
                     // _tprintf(TEXT("PUSHED File name %s\n"), ffd.cFileName);
                    BOOL bwritefun = FALSE;
                    wstring t = ffd.cFileName;
                    string s(t.begin(), t.end());
                    s = s + '\n';
                    bwritefun = WriteFile(
                        hFile,
                        &s[0],
                        s.length(),
                        NULL,
                        NULL

                    );
                    if (FALSE == bwritefun)
                    {
                       // printf("Writing File error\n");
                        CloseHandle(hFile);
                    }
                    else
                    {
                      //  printf("Writing File Success\n");
                    }
                }
            }
        } while (FindNextFile(hFind, &ffd) != 0);
        CloseHandle(hFile);

        if (GetLastError() != ERROR_NO_MORE_FILES) {
            FindClose(hFind);
            //return false;
        }

        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

}
