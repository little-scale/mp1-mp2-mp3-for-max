"""macOS: python3 check-packed-monitor.py /path/to/max-sdk-base (requires FFmpeg)."""
import argparse
import math
from array import array
from pathlib import Path
from subprocess import check_call, check_output
from tempfile import TemporaryDirectory

def main():
    args = argparse.ArgumentParser(description=__doc__)
    args.add_argument('sdk',type=Path)
    sdk = args.parse_args().sdk / 'c74support'
    src = Path(__file__).resolve().parent.parent
    with TemporaryDirectory() as temp:
        d = Path(temp)
        exe, mp3, audio, native, offline = [d / n for n in ('check','take.mp3','monitor','native','offline')]
        check_call(['cc','-O2','-std=c11','-fvisibility=hidden','-Wl,-dead_strip',
                    '-Wl,-undefined,dynamic_lookup','-I',sdk/'max-includes','-I',sdk/'msp-includes',
                    src/'tests/check-packed-monitor.c',src/'pack.c',src/'pack_writer.c',src/'spectral_rate.c',src/'layer3.c','-o',exe])
        memory = d / 'memory'
        check_call(['cc','-O2','-std=c11','-fvisibility=hidden','-Wl,-dead_strip',
                    '-Wl,-undefined,dynamic_lookup','-I',sdk/'max-includes','-I',sdk/'msp-includes',
                    src/'tests/check-frame-memory.c','-o',memory])
        check_call([memory])
        for host, hz in [(8000,8000),(11025,11025),(12000,12000),(16000,16000),
                         (22050,22050),(24000,24000),(32000,32000),(44100,44100),(48000,48000),
                         (96000,48000),(192000,48000),(44117.25,44100),(7350,8000),(1000,8000),(384000,48000)]:
            hop = 1152 if hz>=32000 else 576
            for ch in (1,2):
                for skip in (0,4):
                    opts=[str(host),str(ch),str(skip)]
                    check_call([exe,'record',*opts,mp3,audio])
                    data=check_output(['ffmpeg','-nostdin','-v','error','-xerror','-err_detect','explode',
                                       '-c:a','mp3float','-i',mp3,'-f','f32le','-'])
                    # The preview waits one native frame before starting. Mid-take
                    # enabling warms the decoder using the preceding file packets.
                    native.write_bytes(bytes(hop*ch*4)+data[skip*hop*ch*4:])
                    check_call([exe,'reference',*opts,native,offline])
                    a,b=array('f'),array('f')
                    a.frombytes(audio.read_bytes());b.frombytes(offline.read_bytes())
                    assert len(a)==len(b) and len(b)>0
                    power=sum(v*v for v in b); err=sum((v-w)**2 for v,w in zip(a,b))
                    db=10*math.log10(max(1e-30,err/power))
                    assert math.isfinite(db) and db < -100,(host,ch,skip,db)
            print(f'{host:g} Hz passed',flush=True)
if __name__=='__main__':
    main()
