import typer

app = typer.Typer()

@app.command()
def greet(name: str):
    typer.echo(f"🔥 Drakon AI says hello, {name}!")

if __name__ == "__main__":
    app()
