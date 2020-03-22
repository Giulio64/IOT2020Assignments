# IOT 2020 1^st Assignment
Authors: 
    - Giulio Serra serra.1904089@studenti.uniroma1.it
    
The following repo contains all the code and resources utilized in the 2020 IOT 1^st Homework/Assignment:

"In this assignment you will create a cloud-based IoT system that collects information from a set of virtual environmental sensors using the MQTT protocol. You will also create a simple web site to display the data collected from the sensors."

The project model a distributed system where a number of einvoirmental stations connected to five different sensors:

temperature (-50 ... 50 Celsius)
humidity (0 ... 100%)
wind direction (0 ... 360 degrees)
wind intensity (0 ... 100 m/s)
rain height (0 ... 50 mm / h)

Send data to a central hub.

### Tech
IOT2020 Mini utilizes the following tech:
| Name |Description | Link |
| ------ | ------ |------|
| Node.js | javascript variation, used to buid the application logic.|[https://en.wikipedia.org/wiki/Node.js] |
| Firebase.com |all-in-one solution that provides both storage and code execution capabilities| [https://firebase.google.com] 
| Azure IOT Hub| Azure Hub to collect/analyze data from IOT Devices |[https://azure.microsoft.com/it-it/services/iot-hub/] |


### Repo Structure

* [Analysis] Contains all the documents for the analysis/ designing of the system.
* [Application Logic] Contains all the piece of code / tests for the application logic
* [Presentation Logiv] Contain all the piece of code / tests for the presentation to the user
