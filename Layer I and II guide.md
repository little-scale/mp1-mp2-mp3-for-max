# Patching MPEG Layer I and Layer II in Max

The instruments expose the quantized samples that an MPEG decoder feeds into its 32-band synthesis filter bank. One MC lane represents one band. With oscillators patched into those lanes, their values are sampled once every 32 codec samples. An oscillator in the lowest band and the same oscillator in a high band produce different audio because they drive different synthesis filters.

## Start with a file

1. Open `mp1unpack~.maxhelp` or `mp2unpack~.maxhelp` and turn on DSP.
2. Click `read` to select a file of the matching layer. Wait for `ready`, then click `write` and choose a destination.
3. Click `start`. The five field cables carry the samples, allocation, scales, timing and format to the packer. In the example, `setvalue 3 0` mutes the third band in the word multiplier. Send `setvalue 3 1` to restore it.
4. Use `monitor 1` to listen to the actual packed frames while recording. The example routes the reader’s `done` to the packer’s `stop`. Click `start` again to replay the source from the beginning.

At the native rate with unedited fields and the original bitrate, the packer can preserve the decoded samples exactly. It writes a new MPEG elementary stream. File tags, ancillary data, CRC words, encoder padding contents and private header flags can differ. A lower bitrate or an edit that exceeds a frame’s budget can change the allocation and the resulting audio.

## Start with oscillators

Open `mp1pack~.maxhelp` or `mp2pack~.maxhelp`. The first three inlets receive MC oscillator words, quantizer allocations and scalefactor indices. The object supplies its own frame clock when inlet 4 is open. Click the speaker for DSP, `write` to record and `stop` to finish. In the decoder help patch the same three inputs feed audio straight to the speakers. Keep the listening gain low when several bands have small scalefactor indices.

Word inputs normally take signed integers centered on zero. Send `normalized 1` to let an oscillator in the range −1 to +1 span the selected quantizer. `normalized 0` restores integer input. With allocation 5, Layer I spans −31 to +31 and Layer II spans −7 to +7. Out-of-range values are clipped, while NaN and infinity become zero. With no allocation or scale cable the defaults are allocation 5 and scale index 18.

The included recordings were made in Max by patching a bank of oscillators into each packer, opening four subbands, holding lane 3, then freezing and releasing the whole frame. [Layer I recording](Oscillator%20freeze.mp1) · [Layer II recording](Oscillator%20freeze.mp2)

## Field cables

| Port | Signal | Meaning |
| --- | --- | --- |
| 1 | MC words | One signed sample per band at each slot marker. |
| 2 | MC allocation | Layer I uses 0–14. Values 1–14 select 2–15 bits per word. Layer II uses quantizer classes 0–17, including grouped 3-, 5- and 9-level codes. Zero mutes a band. Allocation is latched at frame start. |
| 3 | MC scalefactor index | 0–62. Adding 3 halves the amplitude. Layer I takes one index per band per frame. Layer II samples the indices at slots 1, 13 and 25. |
| 4 | Slot marker | A signal of 1–12 for MP1 or 1–36 for MP2 labels new slots. Zero holds the current slot between markers. −1 marks idle transport. |
| 5 | MC format | Seven lanes carry codec Hz, kb/s, source channels, MPEG channel mode, joint-stereo boundary, padding and original codec Hz. Connect this cable when editing imported fields. |
| 6 | MC control | On packers and field decoders this is a per-band freeze gate. On frame memory this is an age measured in complete frames. |

The first three inlets accept a single signal broadcast to all bands, 32 lanes shared between sides or 64 independent lanes. MC lanes 1–32 are left and 33–64 are right. A partial bank uses defaults for missing lanes. The file reader always supplies 64 field lanes and reports the file’s actual channel count on the format cable. Unedited mono stays mono when packed. Channel mode is 0 for stereo, 1 for joint stereo, 2 for dual channel and 3 for mono. A padding value of −1 asks the writer to calculate padding.

**Encoder.** A mono MSP input or stereo MC input produces the five field outlets, a sixth MC outlet of packed bytes and a seventh status outlet. The byte cable carries two consecutive bytes per host sample, in lane order, and −1 during idle time. Patch it directly to `mp1decode~ bytes` or `mp2decode~ bytes`. Raw byte signals can exceed audio full scale. Use the decoder’s audio outlet for listening.

**Decoder.** Use `mp1decode~ mc` or `mp2decode~ mc` for the field layout. Bare `mp2decode~` also selects field mode. `bitrate 0` plays the fields directly. A positive bitrate auditions the fitted fields at the nearest legal rate. In `bytes` mode the stream supplies its own bitrate and the second inlet accepts MC holds. Byte-mode audio has two lanes and duplicates mono to both. Byte parsing and the synthesis filter introduce frame latency.

**Packer.** The first five outlets carry the edited fields before bitrate fitting. Outlet 6 reports recording status. Outlet 7 is MC audio reconstructed from written frames when `monitor 1` is enabled. Bit allocation, packing and file writes run on a worker thread. The packer finishes a temporary file on `stop`, then moves it into place. A take with no complete frame reports an error. The `done` message contains the path, number of frames, count of band allocations adjusted, codec Hz, channel count and final kb/s.

## Freeze and frame memory

- Send `freeze 1` to a packer or field/byte decoder to repeat the previous completed frame’s words, allocation and levels. `freeze 0` resumes live input. With an incoming stream, changes land on frame boundaries.
- Patch `mc.sig~ 0 @chans 32` to the hold inlet. `setvalue 3 1` holds lane 3 while the rest follow live data. `setvalue 3 0` releases it. A gate of at least 0.5 is on.
- `holdmode coeff` keeps the held words and allocation and lets the live scalefactors through. `holdmode level` holds the scalefactors too. The whole-frame freeze always includes levels. Bitrate fitting still applies downstream of a hold.
- `mp1frames~ 64` or `mp2frames~ 64` stores 64 completed frames. The capacity can be 1–1024. Age 0 selects live input and age 1 selects the most recently completed frame. Older ages reach further back and clamp to the oldest available frame. `record 0` stops overwriting history, `record 1` resumes and `clear` clears it at the next frame boundary.
- `mp1clock~` and `mp2clock~` output a zero-based slot index, a frame-start pulse, a scalefactor-group pulse and a slot marker. Connect a stream’s marker to the inlet to follow it, or leave the inlet open for the internal clock.

## Bitrates and sample rates

| Layer and version | Codec sample rates | Mono kb/s | Two-channel kb/s |
| --- | --- | --- | --- |
| Layer I, MPEG-1 | 32 / 44.1 / 48 kHz | 32–448 | 32–448 |
| Layer I, MPEG-2 | 16 / 22.05 / 24 kHz | 32–256 | 32–256 |
| Layer II, MPEG-1 | 32 / 44.1 / 48 kHz | 32–192 | 64, 96, 112, 128, 160, 192, 224, 256, 320, 384 |
| Layer II, MPEG-2 | 16 / 22.05 / 24 kHz | 8–160 | 8–160 |

`bitrate 96` and `kbps 96` are equivalent. Requests snap to an indexed bitrate, with ties going to the lower one. A packer at `bitrate 0` follows the source metadata or uses its layer default when patching oscillators. You can change the bitrate within a single take. `getbitrate` reports the current selection and `getrate` reports host Hz, codec Hz and channel count. The band count stays at 32, while some Layer II allocation tables can transmit only 8, 12, 27 or 30 bands at a given format. At very tight Layer I stereo budgets the writer can use shared high-band words with separate left and right scales.

`downsample 2` or the creation setting `@downsample 2` chooses half rate when the native family supports it. At a 48 or 96 kHz host the normal codec rate is 48 kHz, so divisors 2 and 3 select 24 and 16 kHz. At 44.1 kHz, divisor 2 selects 22.05 kHz. The minimum Layer I/II rate here is 16 kHz. Larger divisors stop at the lowest supported member of the source family. `downsample 1` restores the normal rate. Encode and playback convert between the native rate and the host, including fractional ratios.

The five-cable interface carries the original clock so the same divisor on connected objects is applied once. For files you can divide in the reader or packer. Conversion filters and requantizes subband data directly. Duration is rounded up to the last complete target frame. Changing an active recording’s effective rate or channel layout finishes the current file. A reader adopts a new divisor on its next start. In byte mode the decoder treats the host’s divided native clock as a ceiling on the incoming stream. At very low host rates the encoder also limits the bitrate to what its two-byte cable can carry.

## Formats and verification

The reader handles indexed MPEG-1 and MPEG-2 Layer I/II elementary streams, mono, dual channel, stereo, joint stereo, VBR, CRC-protected frames and common ID3/APE tags. It checks protected bits, frame lengths and quantizer codes. Free-format streams, MPEG-2.5 Layer I/II and files that change rate or channel count midstream are outside the reader’s supported formats.

The encoder uses a polyphase analysis bank and an energy-based bit allocator. The decoder uses the matching 32-band synthesis bank. The packer fits edited subband words into the legal tables and available frame bits. The separate Layer III objects use the MP3 hybrid filter bank and have their own cable layout. [MPEG audio standard](https://csclub.uwaterloo.ca/~pbarfuss/ISO11172-3.pdf) · [Layer III object guide](Read%20me.txt)

The regression script covers 312 bitrate/rate/channel combinations and compares synthesis against FFmpeg and mpg123. With the Max SDK supplied it also checks stereo holds, input clamping and native-to-host synthesis from 8 to 192 kHz. In Max, unedited MP1 and MP2 file passes reproduced the source’s decoded audio sample-for-sample, and recordings switched from 160 to 96 to 192 kb/s during a take. [Regression tests](source/layers12/tests)

The original `mp1decode~` and `mp1decode~ raw` retain the serial scanning interface. `mp1bands~` retains the earlier MC-addressed instrument. Open `MP1 serial scanning.maxpat` or `mp1bands~.maxhelp` for those patches. Their legacy raw mode accepts unsigned codewords.

## Addressing Layer II bands with oscillators

Open `mp2bands~.maxhelp` to move MC oscillators between 32 Layer II synthesis bands. The four signal inlets take sample values, destination addresses, allocation classes and scalefactor indices. The three outlets carry mono audio, the current scan column (0–31) and a one-sample pulse at the start of each frame.

- Feed `mc.cycle~` into inlet 1 for normalized samples from −1 to +1. A mono signal feeds all 32 source lanes. A shorter MC bank uses only its connected lanes.
- Leave inlet 2 open for lane 1 to address band 0, lane 2 to address band 1, and so on. A mono address rotates the bank. An MC address gives each source lane an absolute destination from 0 to 31. If two lanes hit the same band, the higher-numbered lane wins. Unaddressed bands receive a zero sample.
- Inlets 3 and 4 always follow the destination band. Allocation class 0 mutes it and classes 1–17 select Layer II quantizers. The default class is 7 and the default scale index is 18. A mono control broadcasts to all destinations.
- Data and addresses are sampled every 32 codec samples. Allocation latches at the start of the 36-slot frame (1,152 codec samples). Scalefactors latch at slots 1, 13 and 25. `downsample 2` and the creation argument `@downsample 2` use the suite’s divided codec clock.
- Use `mp2bands~ signed` for centered integer words from an encoder. For unsigned individual quantizer digits, use `mp2bands~ raw`. For example, class 2 accepts signed words −2…+2 or raw digits 0…4. `mp2decode~ mc` also accepts the encoder’s slot marker and format when exact stream synchronisation is needed.

In Max at 48 kHz, two oscillators swapping between bands 0 and 1 produced the same samples as routing them to the corresponding MC lanes on `mp2decode~ mc`. In that comparison the field decoder’s output arrived 2,272 samples later because it buffers complete frames. The addressable instrument synthesizes each 32-sample slot as its words arrive.
