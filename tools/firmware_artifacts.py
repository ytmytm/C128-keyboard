#!/usr/bin/env python3
"""Package a XIAO SAMD21 Arduino build as verified, versioned release artifacts."""
import argparse
from datetime import date
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
BOARD = 'Seeeduino:samd:seeed_XIAO_m0'
APPLICATION_START = 0x2000
FLASH_END = 0x40000
SAMD21_FAMILY = 0x68ED2B88


def read_version(path):
    match = re.search(r'constexpr char firmwareVersion\[\]\s*=\s*"(\d{4}-\d{2}-\d{2})";',
                      path.read_text())
    if not match:
        raise ValueError('firmware_version.h must contain a YYYY-MM-DD firmwareVersion')
    version = match.group(1)
    date.fromisoformat(version)
    return version


def validate_tag(tag, version):
    if tag and tag not in (version, 'v' + version):
        raise ValueError(f'Release tag {tag!r} must be {version!r} or {"v" + version!r}')


def verify_application(image):
    if not 8 <= len(image) <= FLASH_END - APPLICATION_START:
        raise ValueError('Application binary is empty, truncated or too large for XIAO SAMD21')
    stack, reset = struct.unpack_from('<II', image)
    if not 0x20000000 <= stack <= 0x20008000:
        raise ValueError('Application stack pointer is outside SAMD21 RAM')
    if not reset & 1 or not APPLICATION_START <= (reset & ~1) < APPLICATION_START + len(image):
        raise ValueError('Reset vector must point to Thumb code in the application at 0x2000')


def verify_uf2(data, image):
    verify_application(image)
    count = (len(image) + 255) // 256
    if len(data) != count * 512:
        raise ValueError('Unexpected UF2 file size')
    recovered = bytearray()
    for index in range(count):
        block = data[index * 512:(index + 1) * 512]
        magic0, magic1, flags, address, size, number, total, family = struct.unpack_from('<8I', block)
        end_magic, = struct.unpack_from('<I', block, 508)
        if (magic0, magic1, end_magic) != (0x0A324655, 0x9E5D5157, 0x0AB16F30):
            raise ValueError('Invalid UF2 magic')
        if flags != 0x2000 or family != SAMD21_FAMILY:
            raise ValueError('UF2 must target the SAMD21 family')
        if (address, size, number, total) != (APPLICATION_START + index * 256, 256, index, count):
            raise ValueError('UF2 block address, payload size or numbering is incorrect')
        recovered.extend(block[32:32 + size])
    if recovered[:len(image)] != image or any(recovered[len(image):]):
        raise ValueError('UF2 payload does not match the compiled application binary')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, default=ROOT / 'build')
    parser.add_argument('--output-dir', type=Path, default=ROOT / 'dist')
    parser.add_argument('--uf2conv', type=Path, required=True,
                        help='path to the official microsoft/uf2 utils/uf2conv.py')
    parser.add_argument('--release-tag', default='')
    args = parser.parse_args()
    try:
        version = read_version(ROOT / 'SEEED/C128_keyboard/firmware_version.h')
        validate_tag(args.release_tag, version)
        revision = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT).decode().strip()
        if not re.fullmatch(r'[0-9a-f]{40}', revision):
            raise ValueError('Cannot determine the source commit')
        binary = args.build_dir / 'C128_keyboard.ino.bin'
        elf = args.build_dir / 'C128_keyboard.ino.elf'
        image = binary.read_bytes()
        verify_application(image)
        if not elf.is_file():
            raise ValueError(f'Missing build output: {elf}')
        if args.output_dir.exists() and any(args.output_dir.iterdir()):
            raise ValueError('Output directory must be empty to avoid mixing builds')
        args.output_dir.mkdir(parents=True, exist_ok=True)
        stem = f'C128-keyboard-XIAO-SAMD21-{version}-{revision[:7]}'
        dest_binary = args.output_dir / (stem + '.bin')
        dest_elf = args.output_dir / (stem + '.elf')
        dest_uf2 = args.output_dir / (stem + '.uf2')
        shutil.copyfile(binary, dest_binary)
        shutil.copyfile(elf, dest_elf)
        subprocess.run([
            sys.executable, str(args.uf2conv.resolve()), '--convert', '--family', 'SAMD21',
            '--base', hex(APPLICATION_START), '--output', str(dest_uf2.resolve()),
            str(dest_binary.resolve()),
        ], check=True)
        verify_uf2(dest_uf2.read_bytes(), image)
        metadata = {
            'firmware_version': version, 'git_commit': revision, 'board': BOARD,
            'application_address': hex(APPLICATION_START), 'uf2_family': hex(SAMD21_FAMILY),
            'build_versions': {key: os.environ[key] for key in (
                'ARDUINO_CLI_VERSION', 'SEEED_SAMD_VERSION', 'U8G2_VERSION',
                'SHIFTREGISTER_VERSION', 'UF2_COMMIT') if key in os.environ},
        }
        manifest = args.output_dir / (stem + '.json')
        manifest.write_text(json.dumps(metadata, indent=2, sort_keys=True) + '\n')
        files = [dest_binary, dest_elf, dest_uf2, manifest]
        checksums = '\n'.join(f'{hashlib.sha256(path.read_bytes()).hexdigest()}  {path.name}'
                              for path in sorted(files))
        (args.output_dir / 'SHA256SUMS.txt').write_text(checksums + '\n')
        if os.environ.get('GITHUB_OUTPUT'):
            with open(os.environ['GITHUB_OUTPUT'], 'a') as output:
                output.write(f'artifact_name={stem}\n')
        print(f'Verified firmware artifacts: {args.output_dir}')
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'Firmware packaging failed: {error}\n')


if __name__ == '__main__':
    main()
