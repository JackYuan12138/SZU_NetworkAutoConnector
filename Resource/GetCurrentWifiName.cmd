:: 该脚本用于获取当前连接的wifi名称
@echo off
netsh wlan show interfaces | findstr /C:"SSID"