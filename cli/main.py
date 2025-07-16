import typer

app = typer.Typer()

@app.command()
def greet(name: str = typer.Option(..., "--name", "-n", help="Your name")):
    """Drakon AI greets you with fire 🐉"""
    typer.echo(f"🔥 Drakon AI says hello, {name}!")

if __name__ == "__main__":
    app()
