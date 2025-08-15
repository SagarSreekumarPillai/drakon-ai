from fastapi import FastAPI
from api.routes.module_routes import router as module_router

app = FastAPI(title="Drakon AI API", version="0.1.0")

@app.get("/health")
def health():
    return {"status": "ok"}

app.include_router(module_router, prefix="/v1")
