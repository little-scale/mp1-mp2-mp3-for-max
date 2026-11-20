"""macOS file import regression. Requires cc and FFmpeg on PATH."""
from array import array
from pathlib import Path
from subprocess import run
from tempfile import TemporaryDirectory

src = Path(__file__).resolve().parents[1]

def command(*args, **options):
    return run([str(a) for a in args],check=True,capture_output=True,**options).stdout

def decoded(path):
    out=array("f")
    out.frombytes(command("ffmpeg","-nostdin","-v","error","-xerror","-err_detect","explode",
                          "-i",path,"-f","f32le","-"))
    return out

with TemporaryDirectory() as scratch:
    scratch=Path(scratch)
    exe=scratch/"unpack"
    command("cc","-O2","-std=c11","-I",src,src/"tests/check-file-unpack.c",
            src/"field_file.c",src/"spectral_rate.c",src/"pack.c",src/"layer3.c","-o",exe)
    for hz,ch,joint in [(8000,1,0),(22050,1,0),(44100,2,1),(48000,2,0)]:
        clean=scratch/"source.mp3"; marked=scratch/"tagged.mp3"; out=scratch/"edited.mp3"; tap=scratch/"fields.f32"
        command("ffmpeg","-nostdin","-v","error","-y","-f","lavfi","-i",
                f"sine=frequency=431:sample_rate={hz}:duration=1.25","-ac",ch,
                "-c:a","libmp3lame","-joint_stereo",joint,"-b:a","128k" if hz>=32000 else "32k",
                "-write_xing","0","-id3v2_version","0",clean)
        raw=clean.read_bytes()
        marked.write_bytes(b"ID3\x04\x00\x00\x00\x00\x00\x08"+bytes(8)+raw+b"TAG"+bytes(125))
        print(command(exe,marked,out,tap).decode().strip())
        a=decoded(clean);b=decoded(out);c=array("f");c.frombytes(tap.read_bytes())
        assert len(a)==len(b)==len(c)
        energy=sum(x*x for x in a)
        assert sum((x-y)**2 for x,y in zip(a,c))/energy<1e-5
        assert sum((x-y)**2 for x,y in zip(a,b))/energy<2e-4
        marked.write_bytes(raw[:-3])
        bad=run([str(exe),str(marked),str(out),str(tap)],capture_output=True)
        assert bad.returncode and b"truncated" in bad.stderr
