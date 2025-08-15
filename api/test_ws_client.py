# drakon-ai/api/test_ws_client.py
import asyncio
import json
import websockets  # pip install websockets

URI = "ws://127.0.0.1:8000/api/ws/inference"

async def run():
    async with websockets.connect(URI) as ws:
        msg = {"model_path": "build/dummy_model.bin", "prompt": "Hello from WS!"}
        await ws.send(json.dumps(msg))
        try:
            while True:
                resp = await ws.recv()
                print(resp)
        except websockets.ConnectionClosed:
            pass

if __name__ == "__main__":
    asyncio.run(run())
