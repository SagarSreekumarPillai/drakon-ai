import sys
from pathlib import Path
sys.path.append(str(Path(__file__).resolve().parent.parent))


import typer
from pathlib import Path
from modules.llm_engine.runner import run_local_llm

app = typer.Typer()

@app.command()
def greet(name: str = typer.Option(..., "--name", "-n", help="Your name")):
    """Drakon AI greets you with fire 🐉"""
    typer.echo(f"🔥 Drakon AI says hello, {name}!")

@app.command("prompt-local")
def prompt_local(prompt: str):
    """Run a prompt on local GGUF model via llama.cpp"""
    llama_bin = Path("../llama.cpp/build/bin/llama-cli")  # Updated binary name
    model_path = Path("models/tinyllama.gguf")       # Update filename if different

    if not llama_bin.exists():
        typer.echo("❌ llama.cpp binary not found at: " + str(llama_bin))
        raise typer.Exit(code=1)

    if not model_path.exists():
        typer.echo("❌ GGUF model not found at: " + str(model_path))
        raise typer.Exit(code=1)

    typer.echo("🧠 Running Drakon AI LLM locally...")
    response = run_local_llm(prompt, model_path=model_path, llama_binary=llama_bin)
    typer.echo(f"\n🗣️ Response:\n{response}")

if __name__ == "__main__":
    app()
