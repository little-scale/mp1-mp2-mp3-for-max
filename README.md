# MP3 Encode & Decode for Max

Seven Max externals for making sound in the MP3 compressed domain. Drive a decoder with oscillators, freeze individual spectral bands, or edit the bands of an existing MP3 and write them straight to a new file. Each audio channel exposes 32 bands on a Max MC cable.

## Try it in Max

Download the repository ZIP from the **Code** menu and unzip it. The included macOS externals run on Apple silicon and Intel. Keep the `.mxo` bundles beside the help patches, or add their folder to Max’s search path.

- **Edit an existing MP3.** Turn on DSP, click `read` to choose the source, click `write` to choose a new file, then click `start`. The patch lets you mute and freeze bands during playback. [Open the file-editing help patch](mp3unpack~.maxhelp).
- **Build an instrument.** Connect the first five outlets of `mp3encode~` to `mp3decode~` for live synthesis or to `mp3pack~` for recording. Insert MC processing on the coefficient cable, or supply coefficients from your own oscillators. [Open the encoder and decoder example](mp3encode~.maxhelp).
- **Try 8 kb/s.** In a 48 kHz patch, send `downsample 2` to the objects sharing the live fields. This selects a 24 kHz codec rate. Send `bitrate 8` to the encoder and packer, then `write` to the packer.

`mp3frames~` stores frames for per-band recall, `mp3clock~` supplies row and frame timing, and `mp3short~` adjusts the three short windows. [Object guide and message reference](Read%20me.txt).

## Build and test

This command rebuilds all seven macOS bundles using the Max SDK.

```sh
sh source/build-macos.sh /path/to/max-sdk-base
```

The repository includes the source code, regression tests, audio examples, and third-party notices. [Tests](source/tests) · [Verification results](Packing%20and%20freeze%20tests.txt) · [Third-party notices](licenses/Third-party.txt).
