#!/usr/bin/env python3
"""Package Riga bundles and source with executable permissions preserved."""
import argparse
import hashlib
import json
import platform
from pathlib import Path
import zipfile


def write_archive(destination, entries):
    for source, _ in entries:
        if not source.exists():
            raise FileNotFoundError(source)
    with zipfile.ZipFile(destination, 'w', zipfile.ZIP_DEFLATED) as archive:
        for source, name in entries:
            files = [source] if source.is_file() else sorted(source.rglob('*'))
            for file in files:
                if file.is_file() and '__pycache__' not in file.parts:
                    target = Path(name) / file.relative_to(source) if source.is_dir() else Path(name)
                    archive.write(file, target.as_posix())
    with zipfile.ZipFile(destination) as archive:
        bad = archive.testzip()
        if bad:
            raise RuntimeError(f'Corrupt archive member: {bad}')
    print(destination)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, default=Path('build'))
    parser.add_argument('--output', type=Path, default=Path('dist'))
    parser.add_argument('--architecture', default=platform.machine())
    parser.add_argument('--platform', choices=['macOS', 'Windows'],
                        default='macOS' if platform.system() == 'Darwin' else 'Windows')
    parser.add_argument('--source-only', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    args.output.mkdir(parents=True, exist_ok=True)
    common = [(root / name, name) for name in ['README.md', 'CONTRIBUTING.md', 'docs']]
    sources = ['CMakeLists.txt', '.gitignore', '.clang-format', '.github', 'Source', 'tools', 'tests']
    outputs = [args.output / 'Riga-Source.zip']
    write_archive(outputs[0], common + [(root / name, name) for name in sources])
    if not args.source_only:
        artifacts = args.build_dir / 'Riga_artefacts' / 'Release'
        app = 'Riga.app' if args.platform == 'macOS' else 'Riga.exe'
        outputs.append(args.output / f'Riga-{args.platform}-{args.architecture}.zip')
        write_archive(outputs[-1], common + [(artifacts / 'VST3/Riga.vst3', 'Riga.vst3'),
                                           (artifacts / 'Standalone' / app, app)])
    checksums = {path.name: hashlib.sha256(path.read_bytes()).hexdigest() for path in sorted(args.output.glob('Riga-*.zip'))}
    (args.output / 'checksums.json').write_text(json.dumps(checksums, indent=2) + '\n')


if __name__ == '__main__':
    main()
