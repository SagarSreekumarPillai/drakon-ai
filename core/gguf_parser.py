import struct
import mmap

class GGUFParser:
    def __init__(self, filepath: str):
        self.filepath = filepath
        self.header = {}
        self.metadata = {}
        self.tensor_index = []

        self._load()

    def _load(self):
        with open(self.filepath, "rb") as f:
            self.mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)

            # 1. Magic check
            magic = self.mm.read(4)
            if magic != b'GGUF':
                raise ValueError("Invalid GGUF magic number")

            # 2. Version
            self.header['version'], = struct.unpack("I", self.mm.read(4))

            # 3. Field count
            field_count, = struct.unpack("Q", self.mm.read(8))
            self.header['field_count'] = field_count

            # 4. Metadata (key-value map)
            for _ in range(field_count):
                key_len, = struct.unpack("I", self.mm.read(4))
                key = self.mm.read(key_len).decode()

                val_type, = struct.unpack("B", self.mm.read(1))

                if val_type == 0:  # string
                    val_len, = struct.unpack("I", self.mm.read(4))
                    value = self.mm.read(val_len).decode()
                elif val_type == 1:  # int
                    value, = struct.unpack("q", self.mm.read(8))
                else:
                    value = "Unsupported type"

                self.metadata[key] = value

            # 👇 Add tensor index parser here later
            self.tensor_index = []

    def get_metadata(self):
        return self.metadata

    def close(self):
        self.mm.close()
