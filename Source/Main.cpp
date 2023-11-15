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
    //初始化
    std::map<std::string, std::string> wifi_Map;
    std::ifstream fini;
    std::string current_wifi;

    //读取配置文件，用于获取wifi名称与其对应的连接脚本
    fini.open("CampusNetwork AutoConnector.ini");
    Log("程序已启动！");

    //判断是否正确读取配置文件
    if (!fini.fail()) {

        //读入配置数据
        std::string input, key, value;
        while (std::getline(fini, input)) {
            key = input.substr(0, input.find(' '));
            value = input.substr(input.find(' ') + 1);
            wifi_Map[key] = value;
        }
        fini.close();

        //主循环
        bool last_connection_status = false;
        while (true) {

            if (IsConnnectedToInternet() == false) {

                if (last_connection_status == true) {
                    Log("已断网！");
                    last_connection_status = false;
                }

                //尝试连接校园网
                LoginCampusWifi(wifi_Map);

            } else {

                if (last_connection_status == false) {
                    Log("已联网！");
                    last_connection_status = true;
                }

            }
            Sleep(1000);

        }
    } else {
        Log("读取配置文件失败！");
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
        //printf("进程号：%d\n", pi.dwProcessId);
        //printf("主线程号：%d", pi.dwThreadId);
    }
    return bRet;
}

std::vector<std::string> RunScript(std::string script) {
    static std::string scriptcode, input;
    static std::ifstream fin;
    static std::ofstream fout;
    static std::vector<std::string> result;

    //对保存结果的数组进行初始化
    result.clear();

    //生成运行脚本
    script = "echo off\n" + script + "\ndel %0";
    fout.open("run.bat");
    fout << script;
    fout.close();

    //运行脚本
    HANDLE Hprocess;
    DWORD ExitCode;
    OrderRun("run.bat >result", false, &Hprocess);
    GetExitCodeProcess(Hprocess, &ExitCode);
    while (ExitCode == 259) {
        GetExitCodeProcess(Hprocess, &ExitCode);
        Sleep(100);

    }

    //保存结果
    fin.open("result");
    for (int i = 0; i < 2; i++) {
        std::getline(fin, input);
    }
    while (std::getline(fin, input)) {
        result.push_back(input);
    }
    fin.close();

    /*删除result
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
    result = RunScript("ping -n 3 www.baidu.com | findstr /C:\"丢失\"");
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

    //获取当前wifi名称并在现有列表中查找
    current_wifi = GetCurrentWifiName();
    it = wifi_list.find(current_wifi);

    //如果找到则执行对应的脚本
    if (it != wifi_list.end()) {
        RunScript((*it).second);
        Log("尝试连接wifi: " + current_wifi);
        Log(IsConnnectedToInternet() ? "连接成功!" : "连接失败！");
    } else {
        Log("当前wifi:" + current_wifi + "不在配置列表中！");
    }
}

void Log(const std::string &text) {
    static std::ifstream fin;
    static std::ofstream fout;
    static SYSTEMTIME syst;
    static std::stringstream buffer;
    std::string TimeFlag;

    //生成时间标签
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

    //写入日志文件
    fin.open("Log.log");
    if (fin.fail()) {
        fout.open("Log.log");
        fout << TimeFlag << "创建日志文件" << "\n";
        fout.close();
    } else {
        fin.close();
    }

    fout.open("Log.log", std::ios::app);
    fout << TimeFlag << text << "\n";
    fout.close();
}

