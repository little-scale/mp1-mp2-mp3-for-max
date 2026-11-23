# MP1, MP2 and MP3 for Max

Twenty-one Max/MSP externals for making music with MPEG audio frames. Drive a decoder with MC oscillators, freeze individual bands, or open an existing MPEG audio file and patch its compressed data straight into a new recording.

**[Download the macOS universal release](https://github.com/little-scale/mp1-mp2-mp3-for-max/releases/latest/download/mp1-mp2-mp3-for-max-macos-universal.zip)**

Unzip **MPEG Audio for Max** and keep the externals and help patches together. Open a help patch directly, or add the folder to Max’s search path. The download contains Apple silicon and Intel binaries. Compiled objects are distributed through GitHub Releases.

## Three patches to try

- **Move oscillators between subbands.** Open `mp2bands~.maxhelp`, turn on the speaker and click `spread 47 313` to give the oscillators different frequencies. Click `spread 31 -1` to reverse their band destinations while the allocation and scalefactor sliders stay in place.
- **Edit a file without a PCM round trip.** Open `mp2unpack~.maxhelp`, turn on DSP, click `read`, then `write`, then `start`. Mute or freeze bands while the file plays. The patch finishes the take at the end of the source. The MP1 and MP3 unpackers have their own examples.
- **Hear what a bitrate change does.** Open `mp3encode~.maxhelp`, or either of the new encoder help patches. Send `bitrate` or `kbps` followed by a number to the encoder or packer during a take. Open a decoder’s help patch to audition a bitrate directly from fields.

## The objects

| Role | Layer I | Layer II | Layer III |
| --- | --- | --- | --- |
| Analyze audio into MC fields and output encoded bytes | `mp1encode~` | `mp2encode~` | `mp3encode~` |
| Synthesize audio from fields or encoded bytes | `mp1decode~` | `mp2decode~` | `mp3decode~` |
| Fit edited fields into frames and record a file | `mp1pack~` | `mp2pack~` | `mp3pack~` |
| Read compressed fields from a file | `mp1unpack~` | `mp2unpack~` | `mp3unpack~` |
| Recall different frames for different bands | `mp1frames~` | `mp2frames~` | `mp3frames~` |
| Generate or follow codec timing | `mp1clock~` | `mp2clock~` | `mp3clock~` |
| Layer-specific instrument or editing tool | `mp1bands~` | `mp2bands~` | `mp3short~` |

Each layer exposes 32 subbands per audio channel. Layer I carries 12 subband words per band per frame. Layer II carries 36 and lets you edit three consecutive scalefactor groups. Layer III exposes 18 hybrid spectral coefficients per subband per granule and includes short-window controls. Use matching objects within each layer. For the five-cable Layer I interface, create `mp1decode~ mc`. The original four-inlet `mp1decode~` and `mp1bands~` instruments are included.

The object guides describe the cable layouts, freeze modes, bitrate ranges and sample-rate handling. [Layer I and II guide](Layer%20I%20and%20II%20guide.md) · [Layer III guide](Read%20me.txt)

## Build and test

From the repository root on macOS, use the Cycling ’74 Max SDK to build all 21 universal bundles.

```sh
sh source/build-macos.sh /absolute/path/to/max-sdk-base
python3 source/layers12/tests/check_layers.py /absolute/path/to/max-sdk-base
```

The Layer I and II checks use FFmpeg and mpg123. They exercise every supported bitrate and sample-rate combination, compare independent decoders, and check frame fitting, file transport, downsampling and band holds. The MP3 tests include packed-monitor, file editing, dynamic bitrate and frame-memory regressions. [Layer III tests](source/tests) · [Layer III verification results](Packing%20and%20freeze%20tests.txt) · [Third-party notices](licenses/Third-party.txt)

The original seven-object MP3 release remains available in the same repository. [Download v1.0.0](https://github.com/little-scale/mp1-mp2-mp3-for-max/releases/tag/v1.0.0)
