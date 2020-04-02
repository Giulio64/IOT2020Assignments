/**
 * [Pure fabrication that rappresent the Analytic Hub to send data from the IOT Devices]
 */

const uuidv1 = require("uuid/v1");
const storage = require("../PersistanceStorage/PersistanceStorage");

var Mqtt = require("azure-iot-device-mqtt").Mqtt;
var DeviceClient = require("azure-iot-device").Client;
var Message = require("azure-iot-device").Message;

/**
 * [Create a new log inside the hub and inside the persistance Storage]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {JSONS} sensor [Sensor that generated the data]
 * @param  {JSON} data [Data generated by the sensor]
 */
exports.sendData = function(sensor, data) {
  
  return new Promise((res, rej) => {

    if (sensor.ID === null || sensor.ID === undefined) {
      return rej(new Error("Missing sensor ID"));
    }

    if (
      sensor.connectionString === null ||
      sensor.connectionString === undefined
    ) {
      return rej(new Error("Missing sensor connection string"));
    }

    console.log({event:"sending trasmision to azure hub:",data:data,sensor:sensor})

    return createHubEvent(data, sensor.connectionString)
      .then(() => {
        const newLog = { [uuidv1()]: data };
        return storage.updateRecord("Log/" + sensor.ID, newLog).then(() => {
          return res(newLog);
        });
      })
      .catch(error => {
        return rej(error);
      });
  });
};

/**
 * Create a new Event in the HUB
 * @param {JSON} data [JSON with the data to send to the IOT hub]
 * @param {String} connectionString [Connection String of the sensor that sent the data]
 */
function createHubEvent(data, connectionString) {
  return new Promise((res, rej) => {

    const client = DeviceClient.fromConnectionString(connectionString, Mqtt);
    const message = new Message(JSON.stringify(data));

    return client
      .sendEvent(message)
      .then(response => {
        console.log({ response: response });
        return res(response);
      })
      .catch(error => {
        console.log({errorAzureHub:error});
        return rej(error);
      });
  });
}