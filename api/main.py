from fastapi import FastAPI
from api.routes import module_routes

app = FastAPI(title="Drakon AI")

# Add routes from module_routes
app.include_router(module_routes.router)
