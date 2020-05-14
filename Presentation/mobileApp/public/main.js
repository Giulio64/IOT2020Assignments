const WALKING_TRESH = 2;
const RUNNING_TRESH = 4;
const RAW_ENDPOINT = "https://europe-west1-iothomework-b9a5b.cloudfunctions.net/postUserReading"; // endpoint to post raw data to cumpute the activity in the cloud
const ACTIVITY_ENDPOINT ="https://europe-west1-iothomework-b9a5b.cloudfunctions.net/postUserActivityLog"; // endpoint to post a user activity

let readings = []; // array containing the readings of the magnitudes

$(document).ready(() => {
  try {
    if (window.Accelerometer) {
      this.getReadingsFromGenericSensor();
    } else {
      console.log({ error: "Generic API Accelerometer not available" });
      this.getDeviceMotionEventReadings();
    }
  } catch (error) {
    console.log({ error: error });
    this.handleSensorError();
  }
});

/**
 * Get the readings of the accelerometer from generic API
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function getDeviceMotionEventReadings() {
  DeviceMotionEvent.requestPermission()
    .then((response) => {
      if (response == "granted") {
        window.addEventListener("devicemotion", (e) => {
          const acc = e.accelerationIncludingGravity;
          handleSensorReading({ x: acc.x, y: acc.y, z: acc.z });
        });
        document.getElementById("device_status").innerHTML =
          " Using device motion API";
      } else {
        document.getElementById("device_status").innerHTML =
          " permissions not granted.";
      }
    })
    .catch((err) => {
      console.log({ error: error });
      this.handleSensorError();
    });
}

/**
 * Get the readings of the accelerometer from generic API
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function getReadingsFromGenericSensor() {
  try {
    const sensor = new Accelerometer({ frequency: 1 });
    sensor.onerror = (event) => {
      console.log({ error: event.error.message });
      this.handleSensorError();
    };
    sensor.onreading = () => {
      handleSensorReading({ x: sensor.x, y: sensor.y, z: sensor.z });
    };
    sensor.start();
    document.getElementById("device_status").innerHTML =
      " Using generic sensor API";
  } catch (error) {
    console.log({ error: error });
    this.handleSensorError();
  }
}

/**
 * Display the accelleration of the X Axis
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param {Number} Xreading [accelleration on X-axis]
 */
function displayXReading(Xreading) {
  document.getElementById("x_reading").innerHTML =
    "xAxis: " + Xreading + " m/s^2";
}

/**
 * Display the accelleration of the Y Axis
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param {Number} Yreading [accelleration on Y-axis]
 */
function displayYReading(Yreading) {
  document.getElementById("y_reading").innerHTML =
    "yAxis: " + Yreading + " m/s^2";
}

/**
 * Display the accelleration of the Z Axis
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param {Number} Zreading [accelleration on Z-axis]
 */
function displayZReading(Zreading) {
  document.getElementById("z_reading").innerHTML =
    "zAxis: " + Zreading + " m/s^2";
}

/**
 * Handle the reading of a new data from the accelerometer
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param reading {Object} object containing the reading on the 3-axis
 */
function handleSensorReading(reading) {
  console.log({
    message: "sensor reading",
    x: reading.x,
    y: reading.y,
    z: reading.z,
  });
  this.displayXReading(reading.x);
  this.displayYReading(reading.y);
  this.displayZReading(reading.z);
  readings.push(
    computeTriAxialMagnitude({ x: reading.x, y: reading.y, z: reading.z })
  );
  this.evaluateActivity();
}

/**
 * Display an error on the page
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function handleSensorError() {
  document.getElementById("device_status").innerHTML = " Not supported.";
}

/**
 * Evaluate the current activity of the user
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */
function evaluateActivity() {
  const n_readings = readings.length;

  if (n_readings < 2) {
    return;
  }

  let treshold = readings[0].magnitude - readings[1].magnitude;
  if (treshold < 0) treshold = Math.abs(treshold);

  console.log("reading 1 " + readings[0].magnitude);
  console.log("reading 2 " + readings[1].magnitude);
  console.log("treshold" + treshold);

  let activity = "no activity detected.";
  if (treshold < WALKING_TRESH) {
    activity = "still.";
  } else if (treshold >= WALKING_TRESH && treshold < RUNNING_TRESH) {
    activity = "walking";
  } else {
    activity = "running";
  }

  document.getElementById("activity").innerHTML = activity;

  const payload = {
    readings: readings,
    x: readings[1].x,
    y: readings[1].y,
    z: readings[1].z,
    activity: activity
  };
  readings = [];
  return postActivityAsync(payload).then((response) => {
    if (response.code !== "200") {
      console.log("error uploading data to the cloud" + response.error);
    }

  });
}


/**
 * Compute the magnitude from te accelleration on 3 axis
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {Object} accelleration [Containing the accelleration over 3 avis]
 * @return {Object} [Containing the activity of the user]
 */
function computeTriAxialMagnitude(accelleration) {
  const x_exp = Math.pow(accelleration.x, 2);
  const y_exp = Math.pow(accelleration.y, 2);
  const z_exp = Math.pow(accelleration.z, 2);

  let response = {
    x: accelleration.x,
    y: accelleration.y,
    z: accelleration.z,
    magnitude: Math.sqrt(x_exp + y_exp + z_exp),
  };

  return response;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//API CALLS

/**
 * Post the evaluation of the user's activity on the cloud plus the raw accelleration data to re-compute the activity on the cloud
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {Object} payload [Contains the raw readings plus the allready computed activity]
 */
function postActivityAsync(payload) {
  return postAsync(payload,RAW_ENDPOINT).then(()=>{
    return postAsync(payload,ACTIVITY_ENDPOINT).then(()=>{
      return {code:"200",message:"ok",data:null}
    })
  })
}


/**
 * Post some data to the cloud
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param  {Object} payload [Data to upload to the cloud]
 * @param  {String} url [Url to upload data to]
 */
function postAsync(payload,url){
  return new Promise((res, rej) => {
    try {
      const xmlhttp = new XMLHttpRequest();
      xmlhttp.open("POST", url, false);
      xmlhttp.setRequestHeader("Content-Type", "application/json");
      xmlhttp.onreadystatechange = (response) =>{
        console.log(response);
        return res(response)
      }
      xmlhttp.send(JSON.stringify(payload));
    } catch (err) {
      console.log(err);
      return res({code:"500",message:err.message,data:null});
    }
  })
 
}