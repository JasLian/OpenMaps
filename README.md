## UIC Open Maps Application

This project is a navigation application for the UIC campus map.
Map data is sourced from Open Streets Map.

Project originally based and original files from UIC Data Structures course.

## How it works

First, the user is asked to choose a map data file.

Then the user is asked to input 2 buildings as two starting points. The program then finds the shortest path possible between the midpoint of said buildings.
The path returned is a list of path nodes collected using the map data, along with the distance one would need to travel from each start points to the midpoint.

## Files

* application.cpp - The main file of the project. Contains the main functionality of the project.
* graph.h - An implementation of a graph as an adjaceny list. Used to store the map data.
* dist.cpp - Contains helper functions to calculate distance between points
* osm.cpp, tinyxml2.cpp - Used to extract information from map data
* map.osm, uic.osm - Map data files
* application.exe - An compliled executeable of the project
