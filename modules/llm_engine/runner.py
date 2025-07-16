import subprocess
from pathlib import Path

def run_llama_cpp(prompt: str, model_path: str) -> str:
    llama_exec = Path("./llama.cpp/main")  # Adjust as needed
    command = [str(llama_exec), "-m", model_path, "-p", prompt]
    result = subprocess.run(command, capture_output=True, text=True)
    return result.stdout
