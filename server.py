from flask import Flask, request, jsonify

app = Flask(__name__)

# Store the latest axis data
latest_axis_data = {}

@app.route('/test', methods=['POST'])    #for post data
def receive_data():
    global latest_axis_data
    data = request.json
    if 'axes' in data:
        latest_axis_data = data['axes']
        print(f"Received axis data: {latest_axis_data}")
        return jsonify({"status": "success", "received_axes": latest_axis_data}), 200
    return jsonify({"status": "error", "message": "No axis data received"}), 400

@app.route('/endpoint', methods=['GET'])  #for get data
def send_data():
    if latest_axis_data:
        return jsonify({"status": "success", "received_axes": latest_axis_data}), 200
    return jsonify({"status": "error", "message": "No data available"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
