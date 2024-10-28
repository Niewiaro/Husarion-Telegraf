set PATH=c:\Users\pj80bs\.vscode\HusarionTools\bin\;%PATH%
cd c:\Users\pj80bs\Documents\GitHub\Husarion-Telegraf || exit 1
start /wait st-flash write myproject.bin 0x08010000 || exit 1
start st-util
arm-none-eabi-gdb %*