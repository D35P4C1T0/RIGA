#!/usr/bin/env python3
"""Compare equal-length, stereo-interleaved float32 renders without alignment."""
import argparse
import array
import math
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('reference')
parser.add_argument('candidate')
parser.add_argument('--maximum-error', type=float)
args = parser.parse_args()

def read(path):
    result = array.array('f')
    with open(path, 'rb') as stream:
        raw = stream.read()
    if len(raw) % 8:
        parser.error('Audio must contain complete stereo float32 frames')
    result.frombytes(raw)
    if sys.byteorder != 'little':
        result.byteswap()
    if not result or not all(math.isfinite(x) for x in result):
        parser.error('Audio is empty or contains non-finite samples')
    return result

a, b = read(args.reference), read(args.candidate)
if len(a) != len(b):
    parser.error('Render lengths differ')
error = [float(x)-float(y) for x, y in zip(a, b)]
maximum = max(map(abs, error))
rms = math.sqrt(math.fsum(x*x for x in error)/len(error))
signal = math.sqrt(math.fsum(x*x for x in a)/len(a))
snr = 20*math.log10(signal/rms) if signal and rms else math.inf
print(f'{len(a)//2} stereo frames; max error {maximum:.10g}; RMS error {rms:.10g}; error SNR {snr:.2f} dB')
sys.exit(int(args.maximum_error is not None and maximum > args.maximum_error))
