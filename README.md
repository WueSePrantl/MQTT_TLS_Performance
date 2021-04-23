# Project Creation

- Create an empty project in PlatformIO in Visual Studio Code
- Modify the platformio.ini file as follows:

[env:my_build_env]
platform = espressif8266@1.8 \
board = nodemcuv2 \
framework = arduino \
monitor_speed = 115200 \
lib_deps = AsyncMqttClient 

# Select Workload Pattern

- Replace the contents of the main.cpp file with the contents of the desired workload pattern from file main_connection, main_sleep, or main_no_sleep.  
