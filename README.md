# Comp 465 - Final Project: Solar System Planets
We, Tri and Long, are rendering the Solar System with the tools we have learned in Comp465: Computer Graphics. The code we are using is adapted from one of our homework assignments to achieve this task.
![image](https://user-images.githubusercontent.com/98071520/236651088-115334af-db80-4f7f-a1b7-0eeb138897b1.png)


1. [Task ](#task)
2. [Update](#update)
3. [Run code](#runcode)
4. [updatePosition method](#updatePlanetPositionsMethod)

## Task
In this project, we are using the tools we have learned in Comp465: Computer Graphics to render the Solar System.

Here is the original information for the movement of all the planets, based on the correct ratio of distances and periods of revolution in the Solar System:

```
earthPositions = updatePlanetPositions(currentTime, 8.96f, 365.0f);
// Mercury
mercuryPositions = updatePlanetPositions(currentTime, 3.87f, 87.97f);
// Venus
venusPositions = updatePlanetPositions(currentTime, 7.24f, 224.7f);
// Mars
marsPositions = updatePlanetPositions(currentTime, 15.0f, 687.0f);
// Jupiter
jupiterPositions = updatePlanetPositions(currentTime, 52.0f, 4332.0f);
// Saturn
saturnPositions = updatePlanetPositions(currentTime, 95.0f, 10759.0f);
// Uranus
uranusPositions = updatePlanetPositions(currentTime, 192.0f, 30685.0f);
// Neptune
neptunePositions = updatePlanetPositions(currentTime, 301.0f, 60190.0f);
// Pluto
plutoPositions = updatePlanetPositions(currentTime, 394.0f, 90560.0f);
```

## Update
We have found that our initial simulation ratio for the planets in the Solar System is not suitable, as not all planets can be seen on the screen. Therefore, we need to choose new numbers that will allow all the planets to appear on the screen.

To accomplish this, we have adjusted the values for the planets' positions using the updatePlanetPositions function. Specifically, we needed to modify the parameters for the planets' distances and periods of revolution to achieve the desired result.

<a name="runcode"></a>
## Run Code
To run the code, you will need to adjust the config file by commenting out all the Windows-related configurations. This is because the code is currently configured for Windows, and commenting out these configurations will allow the code to run on other platforms. Once you have made the necessary adjustments, you should be able to run the program without any issues.

<a name="updatePlanetPositionsMethod"></a>
## updatePlanetPositions Method
Here is our implementation of the updatePlanetPositions method that uses the Runge-Kutta method to simulate the planets' movement in the Solar System:

```
glm::vec3 acceleration(glm::vec3 position) {
    glm::vec3 sunPosition(0.0f, 0.0f, 0.0f);
    float G = 6.6743e-11f; // gravitational constant
    glm::vec3 direction = glm::normalize(sunPosition - position);
    float distance = glm::length(sunPosition - position);
    float magnitude = G * 1.989e30f / (distance * distance);
    return magnitude * direction;
}

glm::vec3 updatePlanetPositions(float currentTime, float distance, float period) {
    // Define the time step
    float h = 0.01f;

    // Calculate the number of time steps to take
    int numSteps = static_cast<int>(std::round(period / h));

    // Define the initial position and velocity
    glm::vec3 position(distance, 0.0f, 0.0f);
    glm::vec3 velocity(0.0f, 0.0f, 0.0f);

    // Update the position and velocity for each time step
    for (int i = 0; i < numSteps; ++i) {
        // Calculate the four sets of intermediate values
        glm::vec3 k1v = h * acceleration(position);
        glm::vec3 k1r = h * velocity;
        glm::vec3 k2v = h * acceleration(position + k1r / 2.0f);
        glm::vec3 k2r = h * (velocity + k1v / 2.0f);
        glm::vec3 k3v = h * acceleration(position + k2r / 2.0f);
        glm::vec3 k3r = h * (velocity + k2v / 2.0f);
        glm::vec3 k4v = h * acceleration(position + k3r);
        glm::vec3 k4r = h * (velocity + k3v);

        // Update the position and velocity
        position += (k1r + 2.0f * k2r + 2.0f * k3r + k4r) / 6.0f;
        velocity += (k1v + 2.0f * k2v + 2.0f * k3v + k4v) / 6.0f;
    }

    // Return the updated position
    return position;
}
```
But right now, we still have to investigate more to debug this function before integrating it to our program.
