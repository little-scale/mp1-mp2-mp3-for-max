"""Check converted file fields, the writer, replay, anti-alias rejection and MP3 headers on macOS."""
from array import array
from cmath import exp
from math import ceil, log10, pi
from pathlib import Path
from subprocess import check_output
from tempfile import TemporaryDirectory
import json

source = Path(__file__).resolve().parents[1]

def command(*args):
    return check_output([str(arg) for arg in args])

def read_samples(path, extra=()):
    raw = command("ffmpeg", "-nostdin", "-v", "error", "-xerror", "-err_detect", "explode",
                  "-i", path, *extra, "-f", "f32le", "-")
    samples = array("f")
    samples.frombytes(raw)
    return samples

def amplitude(samples, hz, frequency):
    # Exactly one second in the steady-state part of the test signal.
    start = hz // 3
    block = samples[start:start+hz]
    return 2*abs(sum(v*exp(-2j*pi*frequency*i/hz) for i,v in enumerate(block)))/len(block)

def test(executable, temp, hz, factor, channels, kbps):
    output_hz = hz // factor
    high = round(.64*output_hz)
    reference, packed, read, replay = (temp / name for name in
                                       ("input.mp3", "packed.mp3", "reader.mp3", "replay.mp3"))
    signal = f"aevalsrc=0.17*sin(2*PI*431*t)+0.17*sin(2*PI*{high}*t):s={hz}:d=1.87"
    command("ffmpeg", "-nostdin", "-v", "error", "-y", "-f", "lavfi", "-i", signal,
            "-ac", channels, "-c:a", "libmp3lame", "-b:a", "320k" if hz >= 32000 else "128k",
            "-write_xing", "0", "-id3v2_version", "0", reference)
    command(executable, reference, factor, kbps, packed, read, replay)
    assert packed.read_bytes() == read.read_bytes() == replay.read_bytes()
    meta = json.loads(command("ffprobe", "-v", "error", "-show_streams", "-of", "json", read))
    stream = meta["streams"][0]
    assert int(stream["sample_rate"]) == output_hz and int(stream["bit_rate"]) == kbps*1000
    assert stream["channels"] == channels
    before = read_samples(reference)
    after = read_samples(packed)
    assert len(after) // channels == ceil(len(before)/(channels*factor*576))*576
    before, after = before[::channels], after[::channels]
    reference_down = read_samples(reference, ("-ar", output_hz))[::channels]
    assert amplitude(before, hz, high) > .07
    dry, wet = amplitude(reference_down, output_hz, 431), amplitude(after, output_hz, 431)
    assert .7 < wet/dry < 1.3, (hz, factor, dry, wet)
    alias_db = 20*log10(max(1e-15, amplitude(after, output_hz, output_hz-high))/wet)
    assert alias_db < -42, (hz, factor, alias_db)
    print(f"{hz} -> {output_hz} Hz, {channels} ch, {kbps} kb/s, folded tone {alias_db:.1f} dB")

if __name__ == "__main__":
    with TemporaryDirectory() as folder:
        temp = Path(folder)
        exe = temp / "check"
        command("cc", "-std=c11", "-O2", source/"tests/check-downsample.c",
                *(source / f for f in ("layer3.c", "pack.c", "field_file.c", "pack_writer.c",
                                       "spectral_rate.c")), "-o", exe)
        for args in [(48000,2,1,8), (48000,3,2,64), (48000,4,2,64), (48000,6,1,8),
                     (44100,2,2,96), (44100,4,1,32), (32000,2,2,64), (24000,3,2,32),
                     (22050,2,1,32), (16000,2,1,32)]:
            test(exe, temp, *args)
