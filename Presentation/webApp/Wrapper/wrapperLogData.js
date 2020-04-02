const moment = require("moment");

/**
 * Wrapper to get Log data formatted from raw informations from backend server
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
class wrapperLogData {
  constructor(data) {
    this.rawData = data;
  }

  /**
   * Print the raw data inside the wrapper
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   */
  printData() {
    console.log({ source: "wrapperLogData", rawData: this.rawData });
  }

  /**
   * Returns all the data in the logs ordered by weather station
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   */
  getDataOrderedByStation() {
    var response = {};

    if (this.isJSONObjectEmpty(this.rawData)) return response;

    var data = this.rawData.data.data;

    if (this.isJSONObjectEmpty(data)) return response;

    for (const IDStation in data) {
      const station = data[IDStation];

      if (response[station.name] === undefined) {
        response = Object.assign(response, { [station.name]: [] });
      }

      for (const SensorID in station.sensors) {
        var sensor = station.sensors[SensorID];

        for (const LogID in sensor.logs) {
          const log = sensor.logs[LogID];

          var formattedLog = {}; // here we create a formattedLog with all the informations from the backend

          formattedLog.date = moment
            .unix(log.timestamp)
            .format("MM/DD/YYYY HH:MM a");
          formattedLog.sensorName = log.sensorName;
          formattedLog.sensorType = log.sensorType;
          formattedLog.value = this.formatValue(log.value, log.sensorType);
          formattedLog.station = station.name;
          formattedLog.timestamp = log.timestamp;

          const logTime = moment.unix(log.timestamp);
          const check = logTime.diff(moment(), "days");

          if (check == 0)
            // check if log was generated today
            response[station.name].push(formattedLog);
        }
        response[station.name].sort(this.sortByDate)
      }
    }

    return response;
  }

  /**
   * Returns all the data in the logs ordered by sensor type
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   */
  getDataOrderedBySensor() {
    var response = {};

    if (this.isJSONObjectEmpty(this.rawData)) return response;

    var data = this.rawData.data.data;

    if (this.isJSONObjectEmpty(data)) return response;

    for (const IDStation in data) {
      const station = data[IDStation];

      for (const SensorID in station.sensors) {
        var sensor = station.sensors[SensorID];

        if (sensor.sensorType === undefined) {
          response = Object.assign(response, { [sensor.type]: [] });
        }

        for (const LogID in sensor.logs) {
          const log = sensor.logs[LogID];

          var formattedLog = {}; // here we create a formattedLog with all the informations from the backend
          formattedLog.date = moment
            .unix(log.timestamp)
            .format("MM/DD/YYYY HH:MM a");
          formattedLog.sensorName = log.sensorName;
          formattedLog.sensorType = log.sensorType;
          formattedLog.value = this.formatValue(log.value, log.sensorType);
          formattedLog.station = station.name;
          formattedLog.timestamp = log.timestamp;

          const logTime = moment.unix(log.timestamp);
          const check = logTime.diff(moment(), "hours");

          if (check == 0) {
            // check if log was generated in the last hour
            response[sensor.type].push(formattedLog);
          }
        }
        
        response[sensor.type].sort(this.sortByDate)
      }
    }

    
    return response;
  }

  /**
   * Format the value of a sensor based on it's type
   * @param {Number} value [Value coming from the sensor]
   * @param {String} dataType [Data type of the sensor]
   */
  formatValue(value, dataType) {
    if (dataType === "temperature") {
      return value.toString().slice(0, 5) + " " + "C";
    } else if (dataType === "humidity") {
      return value.toString().slice(0, 5) + " " + "%";
    } else if (dataType === "windDirection") {
      return value.toString().slice(0, 5) + " " + "degrees";
    } else if (dataType === "windIntensity") {
      return value.toString().slice(0, 5) + " " + "m/s";
    } else {
      return value.toString().slice(0, 5) + " " + "mm/h";
    }
  }

  sortByDate(a, b) {
    return b.timestamp -  a.timestamp;
  }

  /**
   * Checks if a json object is empty
   * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
   */
  isJSONObjectEmpty(jsonObject) {
    return (
      jsonObject === null ||
      jsonObject === undefined ||
      Object.keys(jsonObject).length === 0
    );
  }
}

exports.wrapperLogData = wrapperLogData;
