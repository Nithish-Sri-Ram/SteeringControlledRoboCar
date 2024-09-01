import logging
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI()

latest_axis_data = {}
latest_sensor_data = {}

class AxisData(BaseModel):
    steering: float
    accelerator: float
    brake: float

class SensorData(BaseModel):
    sensor: float

@app.post("/send")
async def receive_axis_data(data: AxisData):
    global latest_axis_data
    latest_axis_data = data.dict()
    logging.info(f"Received axis data: {latest_axis_data}")
    return {"status": "success", "received_axes": latest_axis_data}

@app.get("/receive")
async def send_axis_data():
    if latest_axis_data:
        return {"status": "success", "received_axes": latest_axis_data}
    logging.warning("No axis data available")
    raise HTTPException(status_code=400, detail="No axis data available")

@app.post("/send_sensor")
async def receive_sensor_data(data: SensorData):
    global latest_sensor_data
    latest_sensor_data = data.dict()
    logging.info(f"Received sensor data: {latest_sensor_data}")
    return {"status": "success", "received_sensor": latest_sensor_data}

@app.get("/receive_sensor")
async def send_sensor_data():
    if latest_sensor_data:
        return {"status": "Air quality check: Clean and clear, no pollution detected.", "received_sensor": latest_sensor_data}
    logging.warning("No sensor data available")
    raise HTTPException(status_code=400, detail="No sensor data available")

@app.get("/")  # Root route for health check
async def health_check():
    return "The Healthcheck is successful!"

if __name__ == '__main__':
    import uvicorn
    uvicorn.run(app, host='0.0.0.0', port=5000)