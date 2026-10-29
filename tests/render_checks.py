"""Exercise renderer file safety using disposable files only."""
import os
from pathlib import Path
import signal
import struct
import subprocess
import sys
import tempfile

renderer = str(Path(sys.argv[1]).resolve())
with tempfile.TemporaryDirectory() as directory:
    root = Path(directory)
    source = root / 'input.f32'
    destination = root / 'output.f32'
    audio = struct.pack('<ff', .5, -.25) * 257
    source.write_bytes(audio)
    previous = b'previous output'

    def fails(input_path, output_path, **kwargs):
        result = subprocess.run([renderer, str(input_path), str(output_path)],
                                capture_output=True, **kwargs)
        assert result.returncode != 0, result.stdout
        assert source.read_bytes() == audio
        assert destination.read_bytes() == previous
        assert not list(root.glob('.strip-render-*'))

    destination.write_bytes(previous)
    fails(source, source)
    fails(root / 'missing.f32', destination)
    fails(root, destination)
    broken = root / 'incomplete.f32'
    broken.write_bytes(audio + b'!')
    fails(broken, destination)
    for name, link in [('hard-link.f32', os.link), ('symbolic-link.f32', os.symlink)]:
        alias = root / name
        try:
            link(source, alias)
        except OSError:
            continue  # Windows may not grant symlink creation permission.
        fails(source, alias)
        assert alias.read_bytes() == audio

    if os.name == 'posix':
        import resource

        def deny_file_writes():
            signal.signal(signal.SIGXFSZ, signal.SIG_IGN)
            resource.setrlimit(resource.RLIMIT_FSIZE, (0, 0))

        fails(source, destination, preexec_fn=deny_file_writes)

    if os.name == 'posix':
        destination.chmod(0o600)
    result = subprocess.run([renderer, str(source), str(destination)], capture_output=True)
    assert result.returncode == 0, result.stderr
    assert len(destination.read_bytes()) == len(audio)
    if os.name == 'posix':
        assert destination.stat().st_mode & 0o777 == 0o600
    assert source.read_bytes() == audio
    assert not list(root.glob('.strip-render-*'))
print('PASS: renderer rejects aliased/invalid input and preserves files on failure')
