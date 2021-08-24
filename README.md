# NodeMCU_Inside_Outside_Temperature_Humidity_System
Simple project to display weather of a city, your room temperature and humidity on an OLED display. The location can be changed through a blynk app. The system has dynamic WIFI management.
This project uses Openweathermap.org API and a DHT 11 temperature and humidity sensor.
The dyanamic WIFI management is achieved by using this library - https://github.com/tzapu/WiFiManager.
You can rest the EEPROM by pressing single push button connected to the nodeMCU - this makes use of interrupts.
Once the EEPROM is reset all the WIFI settings is erased.
Then you can connect to the temporary hotspot enabled by the nodeMCU to connect to your desired network.
