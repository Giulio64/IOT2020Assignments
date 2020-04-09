/**
 * [Pure fabrication that rappresents the storage]
 */
const admin = require("firebase-admin");
const hub = require("../AnalyticHub/AnalyticHub");

/**
 * [update a record inside backend]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {String} path [the path of the record to update]
 * @param  {JSON} object [object describing the new record , must be in ID:{...} format]
 */
exports.updateRecord = function (path, object) {
  return new Promise((res, rej) => {
    return updateRecord(path, object)
      .then((data) => {
        return res(data);
      })
      .catch((error) => {
        return rej(error);
      });
  });
};

/**
 * [create a record inside backend]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {String} path [the path of the record to update]
 * @param  {JSON} object [object describing the new record , must be in ID:{...} format]
 */
exports.createRecord = function (path, object) {
  return new Promise((res, rej) => {
    admin
      .database()
      .ref(path)
      .set(object)
      .then(() => {
        return res(true);
      })
      .catch((error) => {
        return rej(error);
      });
  });
};

/**
 * [Local version of update record]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function updateRecord(path, object) {
  return new Promise((res, rej) => {
    admin
      .database()
      .ref(path)
      .update(object)
      .then(() => {
        return res(true);
      })
      .catch((error) => {
        return rej(error);
      });
  });
}

/**
 * Get all the weather Stations in the database
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
exports.getWeatherStations = function () {
  return new Promise((res, rej) => {
    getWeatherStations()
      .then((stations) => {
        return res(stations);
      })
      .catch((error) => {
        return rej(error);
      });
  });
};

/**
 * Local versions of get Weather Station
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function getWeatherStations() {
  return new Promise((res, rej) => {
    return admin
      .database()
      .ref("WheatherStation")
      .once("value")
      .then((snap) => {
        return getSensors().then((sensors) => {
          var response = {};
          const stations = snap.val();

          for (const IDStation in stations) {
            let station = stations[IDStation];
            station.sensors = {};

            for (const IDSensor in sensors) {
              if (sensors[IDSensor].stationID === IDStation) {
                station.sensors = Object.assign(station.sensors, {
                  [IDSensor]: sensors[IDSensor],
                });
              }
            }

            response = Object.assign(response, { [IDStation]: station });
          }

          return res(response);
        });
      })
      .catch((error) => {
        return rej(error);
      });
  });
}

/**
 * Get all the sensors in the the database
 */
exports.getSensors = function () {
  return new Promise((res, rej) => {
    return getSensors()
      .then((sensors) => {
        return res(sensors);
      })
      .catch((error) => {
        return rej(error);
      });
  });
};

function getLogs() {
  return new Promise((res, rej) => {
    return admin
      .database()
      .ref("Log")
      .once("value")
      .then((snapLogs) => {

        let logs = snapLogs.val();
        var response = {};

        return hub.getLogs().then((Azurelogs) => { // get logs from azure IOT HUB
          return logs = Object.assign(logs,Azurelogs);
        }).catch((error) => {
            console.log({log:"Error obtaining azure logs:",err:error})
        }).then(() => {

          for (const IDLog in logs) {

            const log = logs[IDLog];
            if (
              response[log.sensorID] === null ||
              response[log.sensorID] === undefined
            ) {
              response = Object.assign(response, { [log.sensorID]: {} });
            }
  
            response[log.sensorID] = Object.assign(response[log.sensorID], {
              [IDLog]: log,
            });
          }
  
          return res(response);

        }).catch((error) => {
          return rej(error);
        });
       
      })
      .catch((error) => {
        return rej(error);
      });
  });
}

/**
 * Local versions of get WeatherSensors
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function getSensors() {
  return new Promise((res, rej) => {
    return admin
      .database()
      .ref("Sensor")
      .once("value")
      .then((snapSensors) => {
        return getLogs()
          .then((logs) => {
            console.log({ logs: logs });

            const sensors = snapSensors.val();
            var response = {};

            for (const SensorID in sensors) {
              let sensor = sensors[SensorID];

              if (logs !== null && logs !== undefined) {
                sensor.logs = logs[SensorID];
              }
              response = Object.assign(response, { [SensorID]: sensor });
            }

            return res(response);
          })
          .catch((error) => {
            console.log(error);
            return res(snapSensors.val());
          });
      })
      .catch((error) => {
        console.log(error);
        return rej(error);
      });
  });
}
