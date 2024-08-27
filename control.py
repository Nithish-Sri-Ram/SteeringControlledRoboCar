import pygame
import time
import requests
import json

# Initialize Pygame
pygame.init()

# Server URLs
post_server_url = "http://localhost:5000/test"
get_custom_url = "http://localhost:5000/endpoint"

# Sensitivity threshold for axis movement
AXIS_THRESHOLD = 0.1 #sends data only if this threshold is met

# Axis indices (adjust if needed)
STEERING_AXIS = 0
ACCELERATOR_AXIS = 4
BRAKE_AXIS = 3

# Previous axis values for change detection
prev_axis_values = {STEERING_AXIS: 0, ACCELERATOR_AXIS: -1, BRAKE_AXIS: -1} #initial value of axis

def send_axis_data(steering_value, accelerator_value, brake_value):
    # Create a dictionary to hold the axis values
    data = {
        "axes": {
            "steering": round(steering_value, 3),
            "accelerator": round(accelerator_value, 3),
            "brake": round(brake_value, 3)
        }
    }  #a nested dictionary with one key 'axes', and it is used in server.py as data['axes']

    try:
        # Send the data to the server via a POST request
        post_response = requests.post(post_server_url, json=data)
        
        # Process the POST response
        if post_response.headers.get('Content-Type') == 'application/json':
            try:
                print("POST Response JSON:", post_response.json())
            except json.JSONDecodeError:
                print("Response is not valid JSON:", post_response.text)
        else:
            print("Response is not JSON:", post_response.text)
        
        # Immediately send a GET request after the POST request
        get_response = requests.get(get_custom_url)
        if get_response.status_code == 200:
            print(f"GET Response JSON:", get_response.json())
        else:
            print(f"GET Request failed with status code: {get_response.status_code}")
    
    except requests.RequestException as e:
        print(f"Failed to send request: {e}")

if __name__ == '__main__':
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

                # Log the values for debugging
                print(f"Steering: {steering_value}, Accelerator: {accelerator_value}, Brake: {brake_value}")

                # Check if there is a significant change in any axis
                if (abs(steering_value - prev_axis_values[STEERING_AXIS]) > AXIS_THRESHOLD or
                    abs(accelerator_value - prev_axis_values[ACCELERATOR_AXIS]) > AXIS_THRESHOLD or
                    abs(brake_value - prev_axis_values[BRAKE_AXIS]) > AXIS_THRESHOLD or
                    accelerator_value == -1 or brake_value == -1):
                    
                    # Update the previous values
                    prev_axis_values[STEERING_AXIS] = steering_value
                    prev_axis_values[ACCELERATOR_AXIS] = accelerator_value
                    prev_axis_values[BRAKE_AXIS] = brake_value

                    # Send all three values together
                    send_axis_data(steering_value, accelerator_value, brake_value)

                # Reduce the delay to ensure responsiveness
                time.sleep(0.05)

        except KeyboardInterrupt:
            print("\nExiting...")
        finally:
            pygame.quit()
