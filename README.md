# 🧠 Drakon AI

Modular, offline-first AI engine powering the Drakon MVP suite.

- 🔌 LLM modules (TinyLlama, Mistral via llama.cpp)
- 📊 Predictors and planners
- 📄 Summarization, document understanding
- 🎛️ CLI + API accessible
- 🆓 100% free and locally runnable – no APIs, no cloud

## 🔧 How to Run

```bash
# Install deps
pip install -r requirements.txt

# Start API
uvicorn api.main:app --reload

# Run CLI
python cli/main.py
