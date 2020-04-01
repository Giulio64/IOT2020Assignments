
const moment = require("moment");
const uuidv1 = require("uuid/v1");

/**
 * Class that rappresents a sensor
 * @param {JSON} info [informations of the class in json Format] 
 */
function Sensor(info) {

  let ID = info.ID;
  let name = info.name;
  let stationID = info.stationID;
  let type = info.type;
  let connectionString = info.connectionString;

  this.setID = function(sensorID){
    ID = sensorID;
  }

  /**
   * Simulate a value from the sensor depending on the type
   */
  this.simulateValue = function(){

    var response = {
      timestamp:moment().unix(),
      sensorName:name,
      sensorType:type,
      origin:"simulated",
      sensorID:ID,
      ID:uuidv1()
    };

    if(type === "temperature"){// Method gives a random value between -50 to 50 Celsius
      response = Object.assign(response,{value:(-50 + Math.random() * 100)});
    }else if(type === "humidity"){
      response = Object.assign(response,{value:(Math.random() * 100)});
    }else if(type === "windDirection"){
      response = Object.assign(response,{value:(Math.random() * 360)});
    }else if(type === "windIntensity"){
      response = Object.assign(response,{value:(Math.random() * 100)}); //m/s
    }else{
      response = Object.assign(response,{value:(Math.random() * 50)}); ////mm/h
    }

    return response;
  };
}


exports.Sensor = Sensor;