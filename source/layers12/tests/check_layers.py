"""Build and exercise Layer I/II codecs on macOS. Requires cc, FFmpeg and mpg123."""
from array import array
from pathlib import Path
from tempfile import TemporaryDirectory
import math
import subprocess
import sys

SOURCE = Path(__file__).resolve().parents[1]
RATES = (16000, 22050, 24000, 32000, 44100, 48000)
BITRATES = {
    (1, True): tuple(range(32, 449, 32)),
    (1, False): (32,48,56,64,80,96,112,128,144,160,176,192,224,256),
    (2, True): (32,48,56,64,80,96,112,128,160,192,224,256,320,384),
    (2, False): (8,16,24,32,40,48,56,64,80,96,112,128,144,160),
}


def call(*args, **kwargs):
    return subprocess.run([str(a) for a in args], check=True, stdout=subprocess.PIPE, **kwargs).stdout


def packets(data):
    pos = 0
    while pos < len(data):
        a, b, c, d = data[pos:pos + 4]
        version = (b >> 3) & 3
        layer = 4 - ((b >> 1) & 3)
        assert a == 255 and b & 224 == 224 and version in (2, 3)
        hz = (44100, 48000, 32000)[(c >> 2) & 3] // (1 if version == 3 else 2)
        kb = BITRATES[layer, version == 3][(c >> 4) - 1]
        channels = 1 if d >> 6 == 3 else 2
        pad = (c >> 1) & 1
        size = (12000 * kb // hz + pad) * 4 if layer == 1 else 144000 * kb // hz + pad
        frame = data[pos:pos + size]
        assert len(frame) == size
        yield frame, layer, hz, kb, channels
        pos += size


def legal(layer, hz, ch):
    rates = set(BITRATES[layer, hz >= 32000])
    if layer == 2 and hz >= 32000:
        rates = {k for k in rates if k <= 192} if ch == 1 else rates - {32,48,56,80}
    return rates


def ffmpeg_decode(file, layer):
    return call("ffmpeg", "-v", "error", "-xerror", "-err_detect", "crccheck+explode", "-f", "mp3",
                "-c:a", f"mp{layer}float", "-i", file, "-f", "f32le", "-")


def relative(signal, reference):
    assert len(signal) == len(reference) and len(signal) > 0
    a, b = array("f", signal), array("f", reference)
    error = sum((x - y) ** 2 for x, y in zip(a, b))
    power = sum(y * y for y in b)
    return 10 * math.log10(max(error / power, 1e-30))


def compile_tests(folder):
    one, two = folder / "codec", folder / "streams"
    common = ["cc", "-O2", "-std=c11", "-DMPEG_AUDIO_LAYERS_12", SOURCE/"subband.c", SOURCE/"filter.c"]
    call(*common, SOURCE/"tests/check_codec.c", "-o", one)
    call(*common, SOURCE/"tests/check_streams.c", SOURCE/"convert.c", SOURCE/"reader.c",
         SOURCE/"worker.c", "-o", two)
    return one, two


def check_matrix(tmp, core, streams):
    worst = -300
    cases = frames = 0
    for layer in (1, 2):
        for hz in RATES:
            for ch in (1, 2):
                path = tmp/f"matrix-{layer}-{hz}-{ch}.mp{layer}"
                call(streams, "matrix", layer, hz, ch, path)
                headers = list(packets(path.read_bytes()))
                rates = legal(layer, hz, ch)
                assert {h[3] for h in headers} == rates
                assert all(h[1:3]==(layer,hz) and h[4]==ch for h in headers)
                cases += len(rates)
                frames += len(headers)
                copy, pcm = tmp/"copy", tmp/"pcm"
                call(core, "compare", path, pcm, copy)
                assert copy.read_bytes() == path.read_bytes()
                reference = ffmpeg_decode(path,layer)
                delta = relative(pcm.read_bytes(),reference)
                assert delta < -83, (layer, hz, ch, delta)
                other = call("mpg123","-q","--no-gapless","-e","f32","-s",path)
                assert relative(other, reference) < -80, (layer, hz, ch)
                worst = max(worst, delta)
    print(f"{cases} legal layer/rate/channel/bitrate combinations, {frames} frames; "
          f"worst synthesis difference from FFmpeg {worst:.1f} dB")


def check_conversions(tmp, core, streams):
    worst_alias=-300
    for layer in (1,2):
        for hz, divisor in ((48000,2),(48000,3),(44100,2),(32000,2)):
            source, a, b = tmp/"tone",tmp/"read-divided",tmp/"write-divided"
            target=hz//divisor
            high=round(.64*target)
            call(core,"encode",layer,hz,160,2,source,high)
            call(streams,"transport",layer,divisor,divisor,source,a)
            call(streams,"transport",layer,1,divisor,source,b)
            assert a.read_bytes()==b.read_bytes(),(layer,hz,divisor)
            target = hz//divisor
            headers=list(packets(a.read_bytes()))
            assert len(headers)==math.ceil(60/divisor) and all(h[2]==target for h in headers)
            decoded=array("f",ffmpeg_decode(b,layer))
            # Independent frequencies exercise both sides of the subband conversion matrix.
            for ch, freq in ((0,431),(1,1231)):
                x=decoded[ch::2]
                start, end = len(x)//4,3*len(x)//4
                data=x[start:end]
                tone=abs(sum(v*complex(math.cos(i*2*math.pi*freq/target),
                                       math.sin(i*2*math.pi*freq/target)) for i,v in enumerate(data)))*2/len(data)
                rms=math.sqrt(sum(v*v for v in data)/len(data))
                assert .12<tone<.20 and tone/rms>1.33,(layer,hz,divisor,ch,tone,rms)
                def amplitude(samples, rate, frequency):
                    phase=2*math.pi*frequency/rate
                    taper=2*math.pi/(len(samples)-1)
                    result=sum(v*(1-math.cos(taper*i))*complex(math.cos(phase*i),math.sin(phase*i))
                               for i,v in enumerate(samples))
                    return 2*abs(result)/(len(samples)-1)
                before=array("f",ffmpeg_decode(source,layer))[ch::2]
                assert amplitude(before[len(before)//4:3*len(before)//4],hz,high)>.09
                alias=20*math.log10(max(1e-12,amplitude(data,target,target-high)/tone))
                assert alias < -43,(layer,hz,divisor,ch,alias)
                worst_alias=max(worst_alias,alias)
    print(f"Reader-side and writer-side conversions agree at eight settings; maximum folded tone {worst_alias:.1f} dB")


def check_external_mp2(tmp,streams):
    src,result=tmp/"external.mp2",tmp/"edited.mp2"
    call("ffmpeg","-v","error","-f","lavfi","-i",
         "aevalsrc=0.16*sin(2*PI*431*t)|0.13*sin(2*PI*1231*t):s=44100:d=0.76",
         "-c:a","mp2","-b:a","160k","-f","mp2",src)
    call(streams,"transport",2,1,1,src,result)
    assert ffmpeg_decode(src,2)==ffmpeg_decode(result,2)
    print("Imported FFmpeg stereo MP2 reproduced sample-for-sample through the reader and writer")


def check_protection(tmp, core, streams):
    # Fixed independent fixture: mono, 48 kHz, 128 kb/s, no allocated bands.
    # Allocation occupies 128 bits in Layer I and 88 bits in this Layer II table.
    for layer,header,size,protected_bits in ((1,b'\xff\xfe\x44\xc0',128,128),(2,b'\xff\xfc\x84\xc0',384,88)):
        crc=65535
        bits=''.join(format(b,'08b') for b in header[2:])+('0'*protected_bits)
        for bit in bits:
            top=(crc>>15)^int(bit)
            crc=((crc<<1)&65535)^(0x8005 if top else 0)
        frame=header+crc.to_bytes(2,"big")+bytes(size-6)
        src, result, pcm=tmp/"protected",tmp/"copy",tmp/"pcm"
        src.write_bytes(frame*5)
        call(core,"compare",src,pcm,result)
        assert set(ffmpeg_decode(src,layer))=={0}
        assert set(ffmpeg_decode(result,layer))=={0}
        tag=b"ID3\x04\x00\x00\x00\x00\x00\x06"+b"source"
        src.write_bytes(tag+frame*5+b"TAG"+bytes(125))
        call(streams,"transport",layer,1,1,src,result)
        assert len(list(packets(result.read_bytes())))==5
        bad=bytearray(frame);bad[4]^=16
        src.write_bytes(bad)
        proc=subprocess.run([core,"compare",src,pcm,result],capture_output=True)
        assert proc.returncode!=0
        src.write_bytes(frame[:-1])
        proc=subprocess.run([core,"compare",src,pcm,result],capture_output=True)
        assert proc.returncode!=0
    print("CRC checks, ID3 boundaries and truncated frames passed for both layers")


def main():
    with TemporaryDirectory() as folder:
        tmp=Path(folder);core,streams=compile_tests(tmp)
        check_matrix(tmp,core,streams)
        check_conversions(tmp,core,streams)
        check_external_mp2(tmp,streams)
        check_protection(tmp,core,streams)
        if len(sys.argv)>1:
            sdk=Path(sys.argv[1]).resolve()/"c74support"
            for layer in (1,2):
                executable=tmp/f"fields{layer}"
                call("cc","-std=c11","-O2","-DMPEG_AUDIO_LAYERS_12",f"-DLAYER={layer}",
                     "-I",sdk/"max-includes","-I",sdk/"msp-includes",
                     SOURCE/"tests/check_fields.c",SOURCE/"subband.c",SOURCE/"filter.c","-o",executable)
                call(executable)
            print("Clamping, independent stereo holds, whole-frame freeze and seven host rates passed")


if __name__ == "__main__":
    main()
