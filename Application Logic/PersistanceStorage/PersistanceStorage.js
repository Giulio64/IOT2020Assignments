/**
 * [Pure fabrication that rappresents the storage]
 */
const admin = require("firebase-admin");

/**
 * [update a record inside backend]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {String} path [the path of the record to update]
 * @param  {JSON} object [object describing the new record , must be in ID:{...} format]
 */
exports.updateRecord = function(path, object) {
  return new Promise((res, rej) => {
    return updateRecord(path, object)
      .then(data => {
        return res(data);
      })
      .catch(error => {
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
exports.createRecord = function(path, object) {
  return new Promise((res, rej) => {
    admin
      .database()
      .ref(path)
      .set(object)
      .then(() => {
        return res(true);
      })
      .catch(error => {
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
      .catch(error => {
        return rej(error);
      });
  });
}

/**
 * Get all the weather Stations in the database
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
exports.getWeatherStations = function() {
  return new Promise((res, rej) => {
    getWeatherStations()
      .then(stations => {
        return res(stations);
      })
      .catch(error => {
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
      .then(snap => {
        return getSensors().then(sensors => {
          var response = {};
          const stations = snap.val();

          for (const IDStation in stations) {
            let station = stations[IDStation];
            station.sensors = {}

            for(const IDSensor in sensors){
              if(sensors[IDSensor].stationID === IDStation){
                station.sensors = Object.assign(station.sensors,{[IDSensor]:sensors[IDSensor]})
              }
            }

            response = Object.assign(response, { [IDStation]: station });
          }

          return res(response);
        });
      })
      .catch(error => {
        return rej(error);
      });
  });
}

/**
 * Get all the sensors in the the database
 */
exports.getSensors = function() {
  return new Promise((res, rej) => {
    return getSensors()
      .then(sensors => {
        return res(sensors);
      })
      .catch(error => {
        return rej(error);
      });
  });
};

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
      .then(snapSensors => {
        return admin
          .database()
          .ref("Log")
          .once("value")
          .then(snapLog => {
            const sensors = snapSensors.val();
            const logs = snapLog.val();

            var response = {};

            for (const SensorID in sensors) {
              let sensor = sensors[SensorID];

              if (logs !== null && logs !== undefined) {
                sensor.logs = logs[SensorID];
              }
              response = Object.assign(response, { [SensorID]: sensor });
            }

            return res(response);
          });
      })
      .catch(error => {
        console.log(error);
        return rej(error);
      });
  });
}
