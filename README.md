# GNOME Weather Location Manager
## Description
GNOME Weather Location Manager is a small command-line utility for GNOME Weather (libgweather 4.x) that allows users to add custom locations directly to the application's settings.

The utility creates a detached GWeatherLocation object, serializes it using the official libgweather API, and stores the serialized object in the org.gnome.Weather GSettings database. Because it relies on the library's own serialization functions, it remains compatible with the internal data format used by the installed version of libgweather, avoiding manual construction of GVariant values.

## Features
Add any custom location using latitude and longitude.
Uses the official libgweather API.
Automatically converts geographic coordinates from degrees to radians.
Generates the correct serialized location format for the installed version of libgweather.
Writes the location directly into the GNOME Weather GSettings database.
Avoids manual editing of GSettings or undocumented GVariant structures.
Lightweight, fast, and without external network dependencies.

## Usage
add-gnome-weather-location "Location Name" LATITUDE LONGITUDE

## Example:
add-gnome-weather-location "Chișinău" 47.0105 28.8638

## Requirements
 * GNOME Weather 48 or newer
 * libgweather 4.x
 * GLib/GIO
 * GSettings

## License
This utility uses only the public libgweather API and does not modify or patch GNOME Weather or libgweather.
