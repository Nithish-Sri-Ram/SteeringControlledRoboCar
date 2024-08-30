from fastapi import FastAPI, Request, HTTPException
from pydantic import BaseModel
import pygame
import time
import requests
import json
import threading

# Initialize Pygame
pygame.init()

app = FastAPI()

# Store the latest axis data
latest_axis_data = {}

# Sensitivity threshold for axis movement
AXIS_THRESHOLD = 0.1

# Axis indices
STEERING_AXIS = 0
ACCELERATOR_AXIS = 4
BRAKE_AXIS = 3

# Previous axis values for change detection
prev_axis_values = {STEERING_AXIS: 0, ACCELERATOR_AXIS: -1, BRAKE_AXIS: -1}

# Server URLs
post_server_url = "https://steeringcontrolledrobocar.onrender.com/send"
get_custom_url = "https://steeringcontrolledrobocar.onrender.com/receive"

class AxisData(BaseModel):
    steering: float
    accelerator: float
    brake: float

def send_axis_data(steering_value, accelerator_value, brake_value):
    data = {
        "axes": {
            "steering": round(steering_value, 3),
            "accelerator": round(accelerator_value, 3),
            "brake": round(brake_value, 3)
        }
    }

    try:
        post_response = requests.post(post_server_url, json=data)
        if post_response.headers.get('Content-Type') == 'application/json':
            try:
                print("POST Response JSON:", post_response.json())
            except json.JSONDecodeError:
                print("Response is not valid JSON:", post_response.text)
        else:
            print("Response is not JSON:", post_response.text)

        get_response = requests.get(get_custom_url)
        if get_response.status_code == 200:
            print(f"GET Response JSON:", get_response.json())
        else:
            print(f"GET Request failed with status code: {get_response.status_code}")

    except requests.RequestException as e:
        print(f"Failed to send request: {e}")

def controller_logic():
    # Check for steering wheel connection
    if pygame.joystick.get_count() == 0:
        print("No steering wheel detected.")
    else:
        wheel = pygame.joystick.Joystick(0)
        wheel.init()
        print(f"Steering wheel connected: {wheel.get_name()}")

        try:
            running = True
            while running:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        running = False

                # Get current axis values for steering, accelerator, and brake
                steering_value = wheel.get_axis(STEERING_AXIS)
                accelerator_value = wheel.get_axis(ACCELERATOR_AXIS)
                brake_value = wheel.get_axis(BRAKE_AXIS)

                print(f"Steering: {steering_value}, Accelerator: {accelerator_value}, Brake: {brake_value}")

                if (abs(steering_value - prev_axis_values[STEERING_AXIS]) > AXIS_THRESHOLD or
                    abs(accelerator_value - prev_axis_values[ACCELERATOR_AXIS]) > AXIS_THRESHOLD or
                    abs(brake_value - prev_axis_values[BRAKE_AXIS]) > AXIS_THRESHOLD or
                    accelerator_value == -1 or brake_value == -1):

                    prev_axis_values[STEERING_AXIS] = steering_value
                    prev_axis_values[ACCELERATOR_AXIS] = accelerator_value
                    prev_axis_values[BRAKE_AXIS] = brake_value

                    send_axis_data(steering_value, accelerator_value, brake_value)

                time.sleep(0.05)

        except KeyboardInterrupt:
            print("\nExiting...")
        finally:
            pygame.quit()

@app.post("/send")
async def receive_data(data: AxisData):
    global latest_axis_data
    latest_axis_data = data.dict()
    print(f"Received axis data: {latest_axis_data}")
    return {"status": "success", "received_axes": latest_axis_data}

@app.get("/receive")
async def send_data():
    if latest_axis_data:
        return {"status": "success", "received_axes": latest_axis_data}
    raise HTTPException(status_code=400, detail="No data available")

if __name__ == '__main__':
    # Start the controller logic in a separate thread
    controller_thread = threading.Thread(target=controller_logic)
    controller_thread.daemon = True  # Allows the program to exit even if the thread is running
    controller_thread.start()

    # Start the FastAPI app
    import uvicorn
    uvicorn.run(app, host='0.0.0.0', port=5000)
