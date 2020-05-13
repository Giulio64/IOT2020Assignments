const WALKING_TRESH = 2;
const RUNNING_TRESH = 4;

const moment = require("moment");

/**
 * [Pure fabrication that compute the activity of the user]
 */

/**
 * [Compute the user activity from two accelleration readings ]
 * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
 * @param {Object} readings [contains the 2 readings to compute the difference in magnitude]
 * @return {Object} [containing the result of the acivity]
 */
exports.evaluateActivity = function (readings) {
  const first_reading = readings[0];
  const second_reading = readings[1];

  if (
    first_reading.x === undefined ||
    first_reading.y === undefined ||
    first_reading.z === undefined
  ) {
    throw new Error("missing one coordinate in first reading");
  }

  if (
    second_reading.x === undefined ||
    second_reading.y === undefined ||
    second_reading.z === undefined
  ) {
    throw new Error("missing one coordinate in first reading");
  }

  const first_magnitude = computeTriAxialMagnitude({
    x: first_reading.x,
    y: first_reading.y,
    z: first_reading.z,
  });
  const second_magnitude = computeTriAxialMagnitude({
    x: second_reading.x,
    y: second_reading.y,
    z: second_reading.z,
  });

  let payload = {
    x: second_reading.x,
    y: second_reading.y,
    z: second_reading.z,
    timestamp: moment().unix(),
  };

  let treshold = first_magnitude - second_magnitude;
  if (treshold < 0) treshold = Math.abs(treshold);
  console.log({
    first_magnitude: first_magnitude,
    second_magnitude: second_magnitude,
    treshold: treshold,
  });
  if (treshold < WALKING_TRESH) {
    payload.activity = "still.";
  } else if (treshold >= WALKING_TRESH && treshold < RUNNING_TRESH) {
    payload.activity = "walking";
  } else {
    payload.activity = "running";
  }

  return payload;
};

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

  return Math.sqrt(x_exp + y_exp + z_exp);
}
