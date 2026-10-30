"""Check UF2 integrity validation without Arduino tools or hardware."""
import importlib.util
from pathlib import Path
import struct
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('firmware_artifacts', ROOT / 'tools/firmware_artifacts.py')
artifacts = importlib.util.module_from_spec(spec)
spec.loader.exec_module(artifacts)


def application():
    return struct.pack('<II', 0x20008000, 0x2101) + bytes(range(256)) * 3


def uf2(image):
    count = (len(image) + 255) // 256
    blocks = []
    for index in range(count):
        payload = image[index * 256:(index + 1) * 256].ljust(256, b'\0')
        header = struct.pack('<8I', 0x0A324655, 0x9E5D5157, 0x2000,
                             0x2000 + index * 256, 256, index, count, 0x68ED2B88)
        blocks.append(header + payload + bytes(220) + struct.pack('<I', 0x0AB16F30))
    return b''.join(blocks)


class FirmwareArtifactsTests(unittest.TestCase):
    def test_valid_application_and_uf2_round_trip(self):
        artifacts.verify_uf2(uf2(application()), application())

    def test_wrong_family_address_flags_size_and_block_numbers(self):
        for offset, value in ((8, 0), (12, 0), (16, 128), (20, 1), (24, 0), (28, 0)):
            with self.subTest(offset=offset):
                data = bytearray(uf2(application()))
                struct.pack_into('<I', data, offset, value)
                with self.assertRaises(ValueError):
                    artifacts.verify_uf2(data, application())

    def test_corrupt_magic_payload_and_last_block_padding(self):
        for offset in (0, 4, 32, 508, 512 + 32, len(uf2(application())) - 512 + 100):
            with self.subTest(offset=offset):
                data = bytearray(uf2(application()))
                data[offset] ^= 1
                with self.assertRaises(ValueError):
                    artifacts.verify_uf2(data, application())

    def test_missing_extra_and_reordered_blocks(self):
        valid = uf2(application())
        for data in (valid[:-512], valid + valid[:512], valid[512:1024] + valid[:512] + valid[1024:]):
            with self.assertRaises(ValueError):
                artifacts.verify_uf2(data, application())

    def test_reject_invalid_application_vectors_or_size(self):
        for image in (b'', bytes(7), bytes(0x3e001),
                      struct.pack('<II', 0x10000000, 0x2009) + bytes(256),
                      struct.pack('<II', 0x20008004, 0x2009) + bytes(256),
                      struct.pack('<II', 0x20008000, 0x1001) + bytes(256),
                      struct.pack('<II', 0x20008000, 0x2008) + bytes(256),
                      struct.pack('<II', 0x20008000, 0x3001) + bytes(256)):
            with self.subTest(size=len(image)):
                with self.assertRaises(ValueError):
                    artifacts.verify_application(image)

    def test_tag_must_match_firmware_date(self):
        for tag in ('', '2026-10-30', 'v2026-10-30'):
            artifacts.validate_tag(tag, '2026-10-30')
        for tag in ('v1.0', '2026-10-31', 'firmware-2026-10-30'):
            with self.assertRaises(ValueError):
                artifacts.validate_tag(tag, '2026-10-30')

    def test_release_date_must_be_present_and_valid(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'firmware_version.h'
            path.write_text('constexpr char firmwareVersion[] = "2026-10-30";\n')
            self.assertEqual(artifacts.read_version(path), '2026-10-30')
            for value in ('2026-02-30', '20261030', 'not-a-date'):
                path.write_text(f'constexpr char firmwareVersion[] = "{value}";\n')
                with self.assertRaises(ValueError):
                    artifacts.read_version(path)


if __name__ == '__main__':
    unittest.main()
