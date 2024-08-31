import logging
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI()

latest_axis_data = {}

class AxisData(BaseModel):
    steering: float
    accelerator: float
    brake: float

@app.post("/send")
async def receive_data(data: AxisData):
    global latest_axis_data
    latest_axis_data = data.dict()
    logging.info(f"Received axis data: {latest_axis_data}")
    return {"status": "success", "received_axes": latest_axis_data}

@app.get("/receive")
async def send_data():
    if latest_axis_data:
        return {"status": "success", "received_axes": latest_axis_data}
    logging.warning("No data available")
    raise HTTPException(status_code=400, detail="No data available")

@app.get("/")  # Root route for health check
async def health_check():
    return "The Healthcheck is successful!"

if __name__ == '__main__':
    import uvicorn
    uvicorn.run(app, host='0.0.0.0', port=5000)
