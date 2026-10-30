#!/usr/bin/env python3
"""Rebuild or verify the Arduino-installable ZIP of the bundled USB library."""
import argparse
from pathlib import Path
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[1]
LIBRARY = ROOT / 'SEEED/C64_USB_Host_Library_SAMD'
ARCHIVE = LIBRARY.with_suffix('.zip')


def source_files():
    names = subprocess.check_output(
        ['git', 'ls-files', '-z', '--', str(LIBRARY.relative_to(ROOT))], cwd=ROOT
    ).decode().split('\0')
    return {
        LIBRARY.name + '/' + Path(name).relative_to(LIBRARY.relative_to(ROOT)).as_posix():
        (ROOT / name).read_bytes().replace(b'\r\n', b'\n')
        for name in sorted(names) if name
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='verify without modifying the archive')
    args = parser.parse_args()
    expected = source_files()
    if args.check:
        with zipfile.ZipFile(ARCHIVE) as archive:
            actual = {entry.filename: archive.read(entry).replace(b'\r\n', b'\n')
                      for entry in archive.infolist() if not entry.is_dir()}
        mismatches = sorted(name for name in expected.keys() | actual.keys()
                            if expected.get(name) != actual.get(name))
        if mismatches:
            parser.exit(1, 'USB library ZIP is stale; run python3 tools/package_usb_library.py\n'
                        + '\n'.join(mismatches) + '\n')
        print(f'PASS: USB library ZIP matches {len(expected)} source files')
    else:
        # Fixed timestamps, permissions and ordering make rebuilds reproducible.
        with zipfile.ZipFile(ARCHIVE, 'w', compression=zipfile.ZIP_DEFLATED,
                             compresslevel=9) as archive:
            for name, data in expected.items():
                entry = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
                entry.compress_type = zipfile.ZIP_DEFLATED
                entry.create_system = 3
                entry.external_attr = 0o100644 << 16
                archive.writestr(entry, data, compresslevel=9)
        print(f'Updated {ARCHIVE.relative_to(ROOT)} ({len(expected)} files)')


if __name__ == '__main__':
    main()
