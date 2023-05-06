# Comp 465 - Final Project: Solar System Planets

# Task
We are rendering the Solar System with the tools we have learnt in Comp465: Computer Graphics. The code is adapted from one of our homework to achieve this task.


// update info for planets moving
earthPositions = updatePlanetPositions(currentTime, 8.96f, 365.0f);

// Mercury
mercuryPositions = updatePlanetPositions(currentTime, 3.87f, 87.97f);

// Venus
venusPositions = updatePlanetPositions(currentTime, 7.24f, 224.7f);

// Moon
moonPositions = earthPositions + vec3(2.0f, 0.0f, 0.0f);

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
