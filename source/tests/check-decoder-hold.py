"""macOS: python3 check-decoder-hold.py /path/to/max-sdk-base (needs mpg123 and FFmpeg)."""
import argparse
from array import array
from math import isfinite, log10
from pathlib import Path
from subprocess import check_call, check_output
from tempfile import TemporaryDirectory

def samples(data):
    pcm = array("f")
    pcm.frombytes(data)
    return pcm

def residual(actual, reference):
    if len(actual) != len(reference) or not reference:
        raise ValueError("Audio lengths differ or the reference is empty")
    energy = sum(v*v for v in reference)
    error = sum((x-y)**2 for x, y in zip(actual, reference))
    if not isfinite(energy + error) or energy <= 0:
        raise ValueError("Nonfinite audio or silent reference")
    return 10 * log10(max(error / energy, 1e-30))

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sdk", type=Path)
    args = parser.parse_args()
    src = Path(__file__).resolve().parent.parent
    support = args.sdk.resolve() / "c74support"
    with TemporaryDirectory() as tmp:
        dest = Path(tmp)
        exe, mp3, pcm = [dest / name for name in ("check", "expected.mp3", "actual.f32")]
        check_call(["cc", "-std=c11", "-O2", "-fvisibility=hidden", "-Wl,-dead_strip",
                    "-Wl,-undefined,dynamic_lookup", "-I", support / "max-includes",
                    "-I", support / "msp-includes", src / "tests/check-decoder-hold.c",
                    src / "pack.c", src / "pack_writer.c", src / "spectral_rate.c", src / "layer3.c", "-o", exe])
        for hz in (8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000):
            for ch in (1, 2):
                for mixed in range(1 if hz == 8000 else 2):
                    check_call([exe, str(hz), str(ch), str(mixed), mp3, pcm])
                    actual = samples(pcm.read_bytes())
                    mpg = samples(check_output(["mpg123", "-q", "--no-gapless", "-e", "f32", "-s", mp3]))
                    ff = samples(check_output(["ffmpeg", "-nostdin", "-v", "error", "-xerror",
                                               "-err_detect", "explode", "-c:a", "mp3float", "-i", mp3,
                                               "-f", "f32le", "-"]))
                    mpg_db, ff_db = residual(actual, mpg), residual(actual, ff)
                    start = 36 * (1152 if hz >= 32000 else 576) * ch
                    end = start + 1152 * ch
                    tail_db = residual(actual[start:end], ff[start:end])
                    if mpg_db > -105 or (not mixed and ff_db > -105) or tail_db > -105 or any(actual[end:]):
                        raise ValueError((hz, ch, mixed, mpg_db, ff_db, tail_db))
                    print(f"{hz} Hz, {ch} ch, mixed={mixed}: mpg123 {mpg_db:.1f} dB, "
                          f"FFmpeg {ff_db:.1f} dB, release {tail_db:.1f} dB")

if __name__ == "__main__":
    main()
