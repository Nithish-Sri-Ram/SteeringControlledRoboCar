from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import pygame
import asyncio
import requests

# Initialize Pygame
pygame.init()

app = FastAPI()

# Sensitivity threshold for axis movement
AXIS_THRESHOLD = 0.1

# Axis indices
STEERING_AXIS = 0
ACCELERATOR_AXIS = 4
BRAKE_AXIS = 3

# Previous axis values for change detection
prev_axis_values = {STEERING_AXIS: 0, ACCELERATOR_AXIS: -1, BRAKE_AXIS: -1}

# The URL of the remote FastAPI server
REMOTE_SERVER_URL = "https://steerbot-6he48xydx-nithishs-projects-8d26ce03.vercel.app/send"

# Global variable to store the latest axis data
latest_axis_data = None

class AxisData(BaseModel):
    steering: float
    accelerator: float
    brake: float

async def controller_logic():
    if pygame.joystick.get_count() == 0:
        print("No steering wheel detected.")
        return
    else:
        wheel = pygame.joystick.Joystick(0)
        wheel.init()
        print(f"Steering wheel connected: {wheel.get_name()}")

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    return

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

                # Create the axis data
                axis_data = AxisData(
                    steering=round(steering_value, 3),
                    accelerator=round(accelerator_value, 3),
                    brake=round(brake_value, 3)
                )

                # Send the data to the remote server
                try:
                    response = requests.post(REMOTE_SERVER_URL, json=axis_data.dict())
                    if response.status_code == 200:
                        print("Data sent successfully")
                    else:
                        print(f"Failed to send data: {response.status_code}, {response.text}")
                except requests.RequestException as e:
                    print(f"Error sending data: {e}")

            await asyncio.sleep(0.00)

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

@app.get("/")  # Root route for health check
async def health_check():
    return "The Healthcheck is successful!"

if __name__ == '__main__':
    # Run the controller logic in a background task
    asyncio.run(controller_logic())

    # Run the FastAPI app using Uvicorn
    import uvicorn
    uvicorn.run(app, host='0.0.0.0', port=5000)
