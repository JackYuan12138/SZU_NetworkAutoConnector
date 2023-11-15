#include <iostream>
#include <fstream>
#include <windows.h>
#include <map>
#include <string>
#include <vector>
#include <sstream>


std::string GetCurrentWifiName();

bool OrderRun(const std::string &CmdLine, bool ShowWindow, HANDLE *phProcess, HANDLE *phThread);

std::vector<std::string> RunScript(std::string script);

bool IsConnnectedToInternet();

void LoginCampusWifi(std::map<std::string, std::string> &wifi_list);

void Log(const std::string &text);


int main() {
    //��ʼ��
    std::map<std::string, std::string> wifi_Map;
    std::ifstream fini;
    std::string current_wifi;

    //��ȡ�����ļ������ڻ�ȡwifi���������Ӧ�����ӽű�
    fini.open("CampusNetwork AutoConnector.ini");
    Log("������������");

    //�ж��Ƿ���ȷ��ȡ�����ļ�
    if (!fini.fail()) {

        //������������
        std::string input, key, value;
        while (std::getline(fini, input)) {
            key = input.substr(0, input.find(' '));
            value = input.substr(input.find(' ') + 1);
            wifi_Map[key] = value;
        }
        fini.close();

        //��ѭ��
        bool last_connection_status = false;
        while (true) {

            if (IsConnnectedToInternet() == false) {

                if (last_connection_status == true) {
                    Log("�Ѷ�����");
                    last_connection_status = false;
                }

                //��������У԰��
                LoginCampusWifi(wifi_Map);

            } else {

                if (last_connection_status == false) {
                    Log("��������");
                    last_connection_status = true;
                }

            }
            Sleep(1000);

        }
    } else {
        Log("��ȡ�����ļ�ʧ�ܣ�");
    }
    return 0;
}

std::string GetCurrentWifiName() {
    std::string scriptline, scriptcode;
    std::ifstream fin;
    std::vector<std::string> result;
    std::string wifiname;

    fin.open("GetCurrentWifiName.cmd");
    while (std::getline(fin, scriptline)) {
        scriptcode += scriptline + "\n";
    }
    fin.close();

    result = RunScript(scriptcode);

    if (!result.empty()) {
        wifiname = result[0];
        wifiname = wifiname.substr(wifiname.rfind(' ') + 1);
    }
    return wifiname;
}

bool
OrderRun(const std::string &CmdLine, bool ShowWindow = false, HANDLE *phProcess = nullptr, HANDLE *phThread = nullptr) {
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = ShowWindow;
    bool bRet = ::CreateProcess(nullptr, (char *) CmdLine.c_str(), nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr,
                                nullptr, &si,
                                &pi);
    if (bRet) {
        //::CloseHandle(pi.hProcess);
        if (phProcess == nullptr) {
            CloseHandle(pi.hProcess);
        } else {
            *phProcess = pi.hProcess;
        }
        if (phThread == nullptr) {
            CloseHandle(pi.hThread);
        } else {
            *phThread = pi.hThread;
        }
        //printf("���̺ţ�%d\n", pi.dwProcessId);
        //printf("���̺߳ţ�%d", pi.dwThreadId);
    }
    return bRet;
}

std::vector<std::string> RunScript(std::string script) {
    static std::string scriptcode, input;
    static std::ifstream fin;
    static std::ofstream fout;
    static std::vector<std::string> result;

    //�Ա�������������г�ʼ��
    result.clear();

    //�������нű�
    script = "echo off\n" + script + "\ndel %0";
    fout.open("run.bat");
    fout << script;
    fout.close();

    //���нű�
    HANDLE Hprocess;
    DWORD ExitCode;
    OrderRun("run.bat >result", false, &Hprocess);
    GetExitCodeProcess(Hprocess, &ExitCode);
    while (ExitCode == 259) {
        GetExitCodeProcess(Hprocess, &ExitCode);
        Sleep(100);

    }

    //������
    fin.open("result");
    for (int i = 0; i < 2; i++) {
        std::getline(fin, input);
    }
    while (std::getline(fin, input)) {
        result.push_back(input);
    }
    fin.close();

    /*ɾ��result
    script = "del result\ndel %0";
    fout.open("run.bat");
    fout << script;
    fout.close();

    OrderRun("run.bat >nul", true, &Hprocess);
    GetExitCodeProcess(Hprocess, &ExitCode);
    while (ExitCode == 259) {
        GetExitCodeProcess(Hprocess, &ExitCode);
        Sleep(1000);
    }*/
    return result;

}

bool IsConnnectedToInternet() {
    std::vector<std::string> result;
    result = RunScript("ping -n 3 www.baidu.com | findstr /C:\"��ʧ\"");
    if (result.empty()) {
        return false;
    } else {
        //std::cout << result[0]<<"\n";
        if (result[0][(result[0]).rfind('=') + 2] > '0') {
            return false;
        }
    }
    return true;
}

void LoginCampusWifi(std::map<std::string, std::string> &wifi_list) {
    std::string current_wifi;
    std::map<std::string, std::string>::iterator it;

    //��ȡ��ǰwifi���Ʋ��������б��в���
    current_wifi = GetCurrentWifiName();
    it = wifi_list.find(current_wifi);

    //����ҵ���ִ�ж�Ӧ�Ľű�
    if (it != wifi_list.end()) {
        RunScript((*it).second);
        Log("��������wifi: " + current_wifi);
        Log(IsConnnectedToInternet() ? "���ӳɹ�!" : "����ʧ�ܣ�");
    } else {
        Log("��ǰwifi:" + current_wifi + "���������б��У�");
    }
}

void Log(const std::string &text) {
    static std::ifstream fin;
    static std::ofstream fout;
    static SYSTEMTIME syst;
    static std::stringstream buffer;
    std::string TimeFlag;

    //����ʱ���ǩ
    GetLocalTime(&syst);
    buffer.clear();
    buffer << "[";
    for (WORD *p = &syst.wYear; p != &syst.wSecond; p++) {
        if (p != &syst.wDayOfWeek) {
            if ((*p) < 10) {
                buffer << "0";
            }
            buffer << *p << "-";
        }
    }
    if (syst.wSecond < 10) {
        buffer << "0";
    }
    buffer << syst.wSecond << "]";
    buffer >> TimeFlag;

    //д����־�ļ�
    fin.open("Log.log");
    if (fin.fail()) {
        fout.open("Log.log");
        fout << TimeFlag << "������־�ļ�" << "\n";
        fout.close();
    } else {
        fin.close();
    }

    fout.open("Log.log", std::ios::app);
    fout << TimeFlag << text << "\n";
    fout.close();
}

