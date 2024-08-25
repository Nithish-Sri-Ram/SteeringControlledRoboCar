from flask import Flask, jsonify
from threading import Thread
import time

app = Flask(__name__)

# Shared variable to hold the current number
current_number = 0

# Background task to update the number every 0.5 seconds
def update_number():
    global current_number
    while True:
        current_number = int(time.time())  # Get the current epoch time in seconds
        time.sleep(0.5)

# Start the background thread
number_thread = Thread(target=update_number)
number_thread.daemon = True  # Daemonize thread to ensure it exits when the main program does
number_thread.start()

# Define the endpoint that returns the current number as JSON
@app.route('/current-number', methods=['GET'])
def get_current_number():
    return jsonify({'number': current_number})

# Run the Flask app
if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000)
