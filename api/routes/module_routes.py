from fastapi import APIRouter

router = APIRouter()

@router.get("/")
def root():
    return {"message": "Drakon AI API is up!"}
