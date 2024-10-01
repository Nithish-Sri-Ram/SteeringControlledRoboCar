# Steering Controller Robot for RC Car

https://github.com/user-attachments/assets/1943d9a4-25af-40ad-9340-090db66099d2

# Steer-Bot

This repository contains the code and setup instructions for a steering controller robot that can control an RC car remotely, just like driving a real car. The project uses an ESP8266 microcontroller with Wi-Fi connectivity to send and receive data via the cloud, enabling control of the car from anywhere in the world.

## Project Overview

- **Controller:** Steering wheel
- **Microcontroller:** ESP8266 (with Wi-Fi)
- **Communication:** The steering controller sends a GET request every second to a FastAPI-based server. The car receives the direction data from the cloud and moves accordingly.
- **Range:** Unlimited, as long as both the controller and the RC car are connected to the internet.

## Key Features

- Control an RC car from anywhere in the world through internet connectivity.
- Utilizes real-time GET requests for direction mapping.
- Compatible with any Wi-Fi enabled RC car.
- The system mimics real car driving dynamics using a steering wheel and pedal controls.

## Technologies Used

- **Arduino:** For the microcontroller programming of the ESP8266.
- **FastAPI:** To create the API server that handles direction mapping.
- **ESP8266:** Wi-Fi-enabled microcontroller to manage the steering input and communicate with the car.

## Components

- ESP8266 with Wi-Fi connectivity
- Steering wheel
- RC car (Wi-Fi-enabled)
- Arduino board
- FastAPI server for handling requests

## How It Works

1. The steering controller reads the direction input and sends it to the FastAPI server as a GET request every second.
2. The FastAPI server maps the direction and forwards it to the RC car.
3. The RC car receives the mapped direction from the server and moves accordingly.
4. With internet connectivity, this setup allows the RC car to be controlled from anywhere.

## Setup Instructions

### Hardware

1. Assemble the steering controller with the ESP8266, Arduino, and RC car components.
2. Ensure the RC car has Wi-Fi capabilities to receive data from the cloud.

### Software

1. Clone this repository:
   ```bash
   git clone https://github.com/Nithish-Sri-Ram/SteeringControlledRoboCar.git
   ```
2. Set up the FastAPI server locally or deploy it to the cloud(preferred).
3. Flash the ESP8266 with the Arduino code provided in the `roboCar` folder.
4. Run the FastAPI server and connect your devices.

### Deployment

- Deploy the FastAPI server on any cloud platform of your choice (Vercel, AWS, Heroku, etc.).
- Ensure both the steering controller and RC car are connected to the same server via Wi-Fi.

## Contributions

Feel free to submit issues or pull requests if you'd like to contribute!
