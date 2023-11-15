:: author: kalila-cc
:: co-author: Jack-Y
@echo off
:: DRCOM_CARD_ID 为校园卡号
SET DRCOM_CARD_ID=1245678
:: DRCOM_PASSWORD 为校园卡密码
SET DRCOM_PASSWORD=123456
SET LOGIN_URL=http://172.30.255.42:801/eportal/portal/login
@echo off
curl -m 5 -v %LOGIN_URL%?callback=result^&user_account=%%2C0%%2C%DRCOM_CARD_ID%^&user_password=%DRCOM_PASSWORD%