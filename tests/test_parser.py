from core.gguf_parser import GGUFParser

def test_gguf_parser():
    parser = GGUFParser("models/tinyllama.gguf")  # 🔁 Replace with your actual model path
    metadata = parser.get_metadata()
    print("[ℹ️] Metadata keys:", metadata.keys())
    parser.close()

if __name__ == "__main__":
    test_gguf_parser()
