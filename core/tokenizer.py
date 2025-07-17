# core/tokenizer.py

import os
import sentencepiece as spm

class Tokenizer:
    def __init__(self, model_path: str):
        if not os.path.exists(model_path):
            raise FileNotFoundError(f"Tokenizer model not found: {model_path}")

        self.sp: spm.SentencePieceProcessor = spm.SentencePieceProcessor()
        self.sp.load(model_path)  # type: ignore

    def tokenize(self, text: str) -> list[int]:
        """
        Tokenizes the given input text into a list of token IDs.
        """
        return self.sp.encode(text, out_type=int)  # type: ignore

    def decode(self, tokens: list[int]) -> str:
        """
        Converts token IDs back to string.
        """
        return self.sp.decode(tokens) # type: ignore

    def vocab_size(self) -> int:
        return self.sp.vocab_size()

    def token_to_str(self, token_id: int) -> str:
        return self.sp.id_to_piece(token_id) # type: ignore

    def str_to_token(self, token_str: str) -> int:
        return self.sp.piece_to_id(token_str) # type: ignore
