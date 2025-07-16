from fastapi import FastAPI
from api.routes import module_routes

app = FastAPI(title="Drakon AI Engine")
app.include_router(module_routes.router)
