"""Run with Python 3 and FFmpeg on either Mac architecture after building the package."""
from array import array
import ctypes as C
from itertools import groupby
from math import sin, tau
from pathlib import Path
from subprocess import run
from tempfile import TemporaryDirectory

root = Path(__file__).resolve().parents[2]
library = root / "mp3encode~.mxo/Contents/Frameworks/libmp3lame.0.dylib"
lame = C.CDLL(str(library))
pointer = C.c_void_p
lame.lame_init.restype = pointer
for name in ("num_channels", "in_samplerate", "out_samplerate", "brate", "VBR", "mode",
             "quality", "disable_reservoir", "bWriteVbrTag", "write_id3tag_automatic",
             "frame_bitrate"):
    getattr(lame, "lame_set_" + name).argtypes = [pointer, C.c_int]
for name in ("init_params", "get_framesize", "close"):
    getattr(lame, "lame_" + name).argtypes = [pointer]
samples = C.POINTER(C.c_float)
lame.lame_encode_buffer_ieee_float.argtypes = [pointer, samples, samples, C.c_int, pointer, C.c_int]
lame.lame_encode_flush.argtypes = [pointer, pointer, C.c_int]

def inspect(raw, hz, expected):
    br1 = [0,32,40,48,56,64,80,96,112,128,160,192,224,256,320]
    br2 = [0,8,16,24,32,40,48,56,64,80,96,112,128,144,160]
    offsets, rates, at = [], [], 0
    while at < len(raw):
        h = raw[at:at+4]
        assert len(h) == 4 and h[0] == 255 and ((h[1]>>1)&3) == 1
        version = (h[1] >> 3) & 3
        rate = [44100,48000,32000][(h[2]>>2)&3] // {3:1,2:2,0:4}[version]
        assert rate == hz
        kb = (br1 if version == 3 else br2)[h[2]>>4]
        offsets.append(at); rates.append(kb)
        at += (144000 if version == 3 else 72000)*kb//hz + ((h[2]>>1)&1)
    assert at == len(raw) and [k for k,_ in groupby(rates)] == expected, (hz,rates,expected)
    return len(offsets)

def check(hz, ch, path):
    choices = ([32,40,48,56,64,80,96,112,128,160,192,224,256,320] if hz>=32000 else
               [8,16,24,32,40,48,56,64,80,96,112,128,144,160] if hz>=16000 else
               [8,16,24,32,40,48,56,64])
    order = choices[::2] + choices[1::2][::-1] + [choices[0],choices[-1]]
    state = lame.lame_init()
    assert state
    for name, value in (("num_channels",ch),("mode",3 if ch==1 else 0),("in_samplerate",hz),
                        ("out_samplerate",hz),("brate",order[0]),("VBR",0),("quality",3),
                        ("disable_reservoir",1),("bWriteVbrTag",0),("write_id3tag_automatic",0)):
        getattr(lame,"lame_set_"+name)(state,value)
    assert lame.lame_init_params(state) == 0
    step = lame.lame_get_framesize(state)
    buffer = C.create_string_buffer(8640); raw = bytearray(); nframe = 0
    for kb in order:
        assert lame.lame_set_frame_bitrate(state,kb) == 0
        for block in range(4):
            wave = (C.c_float*step)(*(.1*sin(tau*431*(nframe*step+i)/hz) for i in range(step)))
            count = lame.lame_encode_buffer_ieee_float(state,wave,wave,step,buffer,len(buffer))
            assert count >= 0
            raw.extend(buffer.raw[:count]); nframe+=1
    count = lame.lame_encode_flush(state,buffer,len(buffer)); assert count >= 0
    raw.extend(buffer.raw[:count]); lame.lame_close(state)
    frames = inspect(raw,hz,order); path.write_bytes(raw)
    audio = run(["ffmpeg","-nostdin","-v","error","-xerror","-err_detect","explode","-i",str(path),
                 "-f","f32le","-"],check=True,capture_output=True).stdout
    assert len(audio) == frames*step*ch*4
    values = array("f"); values.frombytes(audio)
    for start in range(2*step*ch,len(values)-2*step*ch,step*ch):
        rms = (sum(x*x for x in values[start:start+step*ch])/(step*ch))**.5
        assert .04 < rms < .10, (hz,ch,start,rms)
    return frames

if __name__=="__main__":
    with TemporaryDirectory() as temp:
        for hz in (8000,11025,12000,16000,22050,24000,32000,44100,48000):
            n=sum(check(hz,ch,Path(temp)/"change.mp3") for ch in (1,2))
            print(f"{hz} Hz, mono and stereo: {n} frames",flush=True)
