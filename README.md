# NodeMCU_Inside_Outside_Temperature_Humidity_System
Simple project to display weather of a city, your room temperature and humidity on an OLED display. The location can be changed through a blynk app. The system has dynamic WIFI management.
This project uses Openweathermap.org API and a DHT 11 temperature and humidity sensor.
The dyanamic WIFI management is achieved by using this library - https://github.com/tzapu/WiFiManager.
You can rest the EEPROM by pressing single push button connected to the nodeMCU - this makes use of interrupts.
Once the EEPROM is reset all the WIFI settings is erased.
Then you can connect to the temporary hotspot enabled by the nodeMCU to connect to your desired network.
![20210822_011746](https://user-images.githubusercontent.com/70072589/130663155-cc07cb43-e7de-49df-a400-b40474ae5e40.jpg)
![20210822_011851](https://user-images.githubusercontent.com/70072589/130663160-040a350b-2289-4903-942a-cdf6a7491f10.jpg)
![20210823_172206](https://user-images.githubusercontent.com/70072589/130663161-653d0a90-53e9-4f2c-856d-45e82cc5ead6.jpg)
![Screenshot_20210824-230637_Blynk](https://user-images.githubusercontent.com/70072589/130663548-43a88e86-c873-488d-bc23-de1a0f96e624.jpg)
