const functions = require("firebase-functions");
const admin = require("firebase-admin");
const moment = require("moment");
const hub = require("./AnalyticHub/AnalyticHub");
const storage = require("./PersistanceStorage/PersistanceStorage");
const REGION = "europe-west1"; // region of the server where all the functions will be deployed
const uuidv1 = require("uuid/v1");
const cors = require("cors")({ origin: true });

const snr = require("./Model/Sensor");

//needed to initialize the functions
admin.initializeApp();

/**
 * [Format the the data for creating a response]
 *  @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function formatResponse(data, message, code) {
  return (packagedData = {
    message: message,
    code: code,
    data: data
  });
}

/**
 * [Create the records inside the database that rappresent the domain, for test purpose only]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
exports.postData = functions // create data
  .region(REGION)
  .https.onRequest((req, res) => {
    /* Here we are creating the two devices inside the database, 
    assuming they are allready created inside the Azure IOT Hub*/

    const WeatherStationTango = {
      name: "Tango",
      latitude: 9.582515,
      longitude: 45.202579
      //connectionString:"HostName=IOT2020SAP.azure-devices.net;DeviceId=tango;SharedAccessKey=eLsiqXmZw5hZpbsoyf4I8PaSFPLm4w1+8ovrCKu4Rbw=" //Using the Azure CLI: az iot hub device-identity show-connection-string --hub-name {YourIoTHubName} --device-id MyNodeDevice --output table
    };

    const WheatherStationCharlie = {
      name: "Charlie",
      latitude: 10.493317,
      longitude: 43.677184
    };

    const charlieID = uuidv1();
    const tangoID = uuidv1();

    const stations = {
      [tangoID]: WeatherStationTango,
      [charlieID]: WheatherStationCharlie
    };

    const sensors = {
      [uuidv1()]: {
        name: "temperatureCharlie",
        type: "temperature",
        stationID: charlieID,
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=temperatureCharlie;SharedAccessKey=eeebyroH2zVX6sVkoloUD23qG/tucAuOXRUEuEBWeNM="
      },
      [uuidv1()]: {
        name: "humidityCharlie",
        type: "humidity",
        stationID: charlieID,
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=humidityCharlie;SharedAccessKey=ImN4L7TnPnypAqeoZ7/u1fOy5N7Ua1QNwD8YCv3voa0="
      },
      [uuidv1()]: {
        name: "windDirectionCharlie",
        stationID: charlieID,
        type: "windDirection",
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=windDirectionCharlie;SharedAccessKey=n6QJ338al4jvikzb+ZMPFbMq9WCpoqrKGQZn4s6Tf6g="
      },
      [uuidv1()]: {
        name: "windIntensityCharlie",
        stationID: charlieID,
        type: "windIntensity",
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=windIntensityCharlie;SharedAccessKey=+Isam8KSrPaf0W2C/TDwXZbHddMNS+XgdvYwbf40CH4="
      },
      [uuidv1()]: {
        name: "rainHeightCharlie",
        stationID: charlieID,
        type: "rain",
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=rainCharlie;SharedAccessKey=tJGeZrub7Sv9OtiBiGntOkGKChwG/ssLmXHbXoBvpnE="
      },
      [uuidv1()]: {
        name: "temperatureTango",
        type: "temperature",
        stationID: tangoID,
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=temperatureTango;SharedAccessKey=adzu5ZKfDQg74kU5II3BQqR83nJ+ecY4oen+8tiuYoQ="
      },
      [uuidv1()]: {
        name: "humidityTango",
        type: "humidity",
        stationID: tangoID,
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=humidityTango;SharedAccessKey=6KiHdTlVUHcoJUGnngbwmFehhw5O8cupPU8zQzcW09Q="
      },
      [uuidv1()]: {
        name: "windDirectionTango",
        stationID: tangoID,
        type: "windDirection",
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=windDirectionTango;SharedAccessKey=XMJxXXBRuTJF6s/cS92xyv1+oik+t5eWOcuns5cHoJw="
      },
      [uuidv1()]: {
        name: "windIntensityTango",
        stationID: tangoID,
        type: "windIntensity",
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=windIntensityTango;SharedAccessKey=cXMZlxcGmFWSgTauq8+/7d16I7lGTvQEvoqr3i+VBQM="
      },
      [uuidv1()]: {
        name: "rainHeightTango",
        stationID: tangoID,
        type: "rain",
        connectionString:
          "HostName=IOTSAP2020.azure-devices.net;DeviceId=rainTango;SharedAccessKey=5a9pzXkjE2EnDHJUGwMSP+7zTtyIxh4S9f03IGO/PGI="
      }
    };

    return storage
      .updateRecord("WheatherStation", stations)
      .then(() => {
        return storage.updateRecord("Sensor", sensors).then(() => {
          var response = {};

          for (const IDStation in stations) {
            let station = stations[IDStation];
            station.sensors = {};

            for (const IDSensor in sensors) {
              if (sensors[IDSensor].stationID === IDStation) {
                station.sensors = Object.assign(station.sensors, {
                  [IDSensor]: sensors[IDSensor]
                });
              }
            }

            response = Object.assign(response, { [IDStation]: station });
          }

          return res.status(200).send(formatResponse(response, "ok", "200"));
        });
      })
      .catch(error => {
        return res.status(500).send(formatResponse(null, error.message, "500"));
      });
  });

/**
 * [Test the simulation of trasmission from all the sensors]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
exports.postSensorsTrasmission = functions
  .region(REGION)
  .https.onRequest((req, res) => {
    return simulateDataTrasmission()
      .then(logs => {
        return res.status(200).send(formatResponse(logs, "ok", "200"));
      })
      .catch(error => {
        return res.status(500).send(formatResponse(null, error.message, "500"));
      });
  });

/**
 * [Get all the logs inside the database organized by the weather station linked to the sensor that generate it ]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
exports.getLogs = functions.region(REGION).https.onRequest((req, res) => {
  cors(req, res, () => {

    return storage
    .getWeatherStations()
    .then(stations => {
      return res.status(200).send(formatResponse(stations, "ok", "200"));
    })
    .catch(error => {
      return res.status(500).send(formatResponse(null, error.message, "500"));
    });
    
  })
});


/*exports.getAzureLogs = functions.region(REGION).https.onRequest((req, res) => {
  cors(req, res, () => {
   return hub.getLogs()
    .then(logs => {
      return res.status(200).send(formatResponse(logs, "ok", "200"));
    })
    .catch(error => {
      console.log({AzureError:error})
      return res.status(500).send(formatResponse(null, error.message, "500"));
    });
    
  })
});*/

/**
 * [Cron simulating a new data trasmission every 30 minutes from all the devices]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
exports.cronStarter = functions
  .region(REGION)
  .pubsub.schedule("*/10 * * * *")
  .onRun(context => {
    /*return simulateDataTrasmission()
      .then(() => {
        console.log(
          "Simulated telemetry data incoming from the sensors, current time: " +
            moment().format("MMMM Do YYYY, h:mm:ss a")
        );
        return;
      })
      .catch(error => {
        console.log("Error sending simultaed telemetry " + moment().format("MMMM Do YYYY, h:mm:ss a"));
        return rej(error);
      });*/

      console.log(
        "Simulated telemetry is disabled, no call executed. current time: " +
          moment().format("MMMM Do YYYY, h:mm:ss a")
      );
  });

/**
 * Simulate the trasmission of the data from all the devices
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function simulateDataTrasmission() {
  return new Promise((res, rej) => {
    return storage.getSensors().then(sensors => {
      var trasmissionPromisses = [];

      for (const ID in sensors) {
        let wrapper = {
          ID: ID,
          connectionString: sensors[ID].connectionString
        };

        const sensor = new snr.Sensor(sensors[ID]);
        sensor.setID(ID);
        const data = sensor.simulateValue();

        trasmissionPromisses.push(hub.sendMQTTData(wrapper, data));
        //trasmissionPromisses.push(storage.updateRecord("Log",{[data.ID]:data})) -> now the logs aren0t store anymore
      }

      return Promise.all(trasmissionPromisses)
        .then(responses => {
          return res(responses);
        })
        .catch(error => {
          return rej(error);
        });
    });
  });
}
