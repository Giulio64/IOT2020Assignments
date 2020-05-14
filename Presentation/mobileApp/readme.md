# User's activity recognition model

Assuming that the accellerometer output a reading of 3-dimensional vector (x,y,z) measuring the acceleration of a user/device, we can compute the magnitude of a reading (vector) by using the formula:

magnitude = sqrt(x^2 + y^2 + z^2) 

The magnitude measures the distance of a vector(in this case on 3-axis), so we can compare the difference in magnitude of two reading very close one to another, to determine the user's activity.

By computing the difference between two readings and comparing it to a treshold, we can determine, with a fair degree of accuracy, the user's activity.

See the **computeTriAxialMagnitude()** method for further informations.
