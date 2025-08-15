# drakon-ai/api/routes/module_routes.py
from fastapi import APIRouter, WebSocket, WebSocketDisconnect, HTTPException
from fastapi import Body
from pydantic import BaseModel
import asyncio
import json
import os
import sys
import shlex
import re
import subprocess

router = APIRouter()

# ---- REST example (kept minimal) ----
class InferRequest(BaseModel):
    model_path: str
    prompt: str

@router.post("/infer")
async def infer(req: InferRequest):
    """
    Simple REST: calls CLI and returns the full stdout as one string.
    For streaming/real-time, prefer the WebSocket endpoint.
    """
    cli_path = os.path.join(os.getcwd(), "build", "bin", "drakon")
    if not os.path.isfile(cli_path):
        raise HTTPException(status_code=500, detail="CLI binary not found. Build first.")

    cmd = f"{shlex.quote(cli_path)} {shlex.quote(req.model_path)} {shlex.quote(req.prompt)}"
    try:
        proc = await asyncio.create_subprocess_shell(
            cmd,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
        )
        out_bytes, _ = await proc.communicate()
        return {"ok": True, "output": out_bytes.decode("utf-8", errors="ignore")}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


# ---- WebSocket streaming ----
@router.websocket("/ws/inference")
async def ws_inference(ws: WebSocket):
    """
    Connect and then send a single JSON message:
      { "model_path": "build/dummy_model.bin", "prompt": "Hello!" }

    Server will run the C++ CLI and stream its stdout line-by-line.
    """
    await ws.accept()
    try:
        init_msg = await ws.receive_text()
        try:
            payload = json.loads(init_msg)
            model_path = payload.get("model_path")
            prompt = payload.get("prompt")
        except Exception:
            await ws.send_text(json.dumps({"type": "error", "message": "Invalid JSON"}))
            await ws.close(code=1003)
            return

        if not model_path or not prompt:
            await ws.send_text(json.dumps({"type": "error", "message": "model_path and prompt required"}))
            await ws.close(code=1003)
            return

        cli_path = os.path.join(os.getcwd(), "build", "bin", "drakon")
        if not os.path.isfile(cli_path):
            await ws.send_text(json.dumps({"type": "error", "message": "CLI binary not found. Build first."}))
            await ws.close(code=1011)
            return

        cmd = f"{shlex.quote(cli_path)} {shlex.quote(model_path)} {shlex.quote(prompt)}"
        await ws.send_text(json.dumps({"type": "info", "message": f"Starting: {cmd}"}))

        proc = await asyncio.create_subprocess_shell(
            cmd,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
        )

        # Stream stdout lines
        assert proc.stdout is not None
        async for raw_line in proc.stdout:
            line = raw_line.decode("utf-8", errors="ignore").rstrip("\n")
            await ws.send_text(json.dumps({"type": "log", "line": line}))

        rc = await proc.wait()
        await ws.send_text(json.dumps({"type": "done", "returncode": rc}))
        await ws.close()
    except WebSocketDisconnect:
        # Client disconnected
        return
    except Exception as e:
        try:
            await ws.send_text(json.dumps({"type": "error", "message": str(e)}))
        finally:
            await ws.close(code=1011)



class InferRequest(BaseModel):
    prompt: str
    model_path: str = "build/dummy_model.bin"  # default demo
    drakon_bin: str = "build/bin/drakon"       # default binary path

@router.post("/infer")
def infer(req: InferRequest):
    # Ensure binary exists
    if not os.path.exists(req.drakon_bin):
        raise HTTPException(status_code=500, detail=f"drakon binary not found at {req.drakon_bin}")

    # Build command
    cmd = f"{shlex.quote(req.drakon_bin)} {shlex.quote(req.model_path)} {shlex.quote(req.prompt)}"
    try:
        proc = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=60)
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Failed to run subprocess: {e}")

    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail=f"drakon failed: {proc.stderr or proc.stdout}")

    # Parse outputs of form: "[🔄] Token 9270 → Output: 101970"
    outputs = []
    for line in proc.stdout.splitlines():
        m = re.search(r"Token\s+(\d+)\s+→\s+Output:\s+([-+eE0-9\.]+)", line)
        if m:
            outputs.append({"token": int(m.group(1)), "value": float(m.group(2))})

    return {
        "ok": True,
        "raw": proc.stdout,
        "parsed": outputs
    }
