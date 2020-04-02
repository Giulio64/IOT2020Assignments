# IOT 2020 Assignments
Authors: 
    - Giulio Serra serra.1904089@studenti.uniroma1.it
    
The project model a distributed system where a number of environmental stations connected to five different sensors:

temperature (-50 ... 50 Celsius),
humidity (0 ... 100%),
wind direction (0 ... 360 degrees),
wind intensity (0 ... 100 m/s),
rain height (0 ... 50 mm / h),

sending data to a central hub.

### Tech

| Name |Description | Link |
| ------ | ------ |------|
| Node.js | javascript variation, used to buid the application logic.|[https://en.wikipedia.org/wiki/Node.js] |
| Firebase.com |all-in-one solution that provides both storage and code execution capabilities| [https://firebase.google.com] 
| Azure IOT Hub| Azure Hub to collect/analyze data from IOT Devices |[https://azure.microsoft.com/it-it/services/iot-hub/] |
| React Web| Framework developed by facebook to build web app using Javascript |[https://it.reactjs.org] |
| RIOT OS| OS for embedded device|[https://riot-os.org] |
| MOSQUITTO| broker for MQTT protocol comunication|[https://mosquitto.org] |
| PAHO| transparent gateway for MQTT protocol comunication|[https://www.eclipse.org/paho/components/mqtt-sn-transparent-gateway/] |

### Repo Structure

* [Analysis] Contains all the documents for the analysis/ designing of the system.
* [Application Logic] Contains all the piece of code / tests for the application logic
* [Presentation] Contains all the piece of code / tests for the presentation to the user

### Important Links:
1^Assignment

The following repo contains all the code and resources utilized in the 2020 IOT 1^st Homework/Assignment:

"In this assignment you will create a cloud-based IoT system that collects information from a set of virtual environmental sensors using the MQTT protocol. You will also create a simple web site to display the data collected from the sensors."

* [Demonstration] See a demonstration on youtube: https://www.youtube.com/watch?v=zjmsmxCqq48&feature=youtu.be
* [Tutorial]check the post on linkedin with the full tutorial: https://www.youtube.com/watch?v=fjALkbL98r8&feature=youtu.be <-- Please note that due some problems on the youtube video I re-uploaded it on March 26, the original tutorial is from March 24.

2^Assignment

"In this assignment we will built on-top of the cloud-based components developed in the first assignment. Now we will replace the virtual environmental stations developed using Python/Java, or any other language you decided, with new ones built using the RIOT-OS and MQTT-SN protocol. You will use the native emulator of RIOT-OS to run your stations and generate values over MQTT-SN that need to arrive to the cloud via the MQTT. You will also use IOT-LAB to execute your RIOT-OS application on real devices."

* [Demonstration] See a demonstration on youtube: https://youtu.be/t0e9Yn5L86A
* [Tutorial]check the post on linkedin for the full tutorial: https://www.linkedin.com/posts/giulio-serra-02966b151_iot-tutorial-2-st-assignment-activity-6651485074720198658-sDRH

please se inside Application Logic and presentation for the resources of the 2^nd assignment

PLEASE NOTE: due to github limitation i cannot upload the full working code with all the modules and frameworks, if you want a full functional copy please email me at: serra.1904089@studenti.uniroma1.it
