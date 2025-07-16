import subprocess
from pathlib import Path

def run_local_llm(prompt: str, model_path: Path, llama_binary: Path) -> str:
    command = [
        str(llama_binary),
        "-m", str(model_path),
        "-p", prompt,
        "-n", "128"  # number of tokens to generate
    ]

    result = subprocess.run(command, capture_output=True, text=True)

    if result.returncode != 0:
        return f"[ERROR] {result.stderr.strip()}"

    return result.stdout.strip()
