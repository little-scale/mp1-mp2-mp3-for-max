{
 "patcher": {
  "fileversion": 1,
  "appversion": {
   "major": 9,
   "minor": 1,
   "revision": 4,
   "architecture": "x64",
   "modernui": 1
  },
  "classnamespace": "box",
  "rect": [
   80,
   90,
   965,
   801
  ],
  "default_fontsize": 13.0,
  "default_fontname": "Arial",
  "openinpresentation": 0,
  "boxes": [
   {
    "box": {
     "id": "title",
     "maxclass": "comment",
     "patching_rect": [
      24,
      13,
      850,
      38
     ],
     "fontsize": 27.0,
     "fontface": 1,
     "text": "Patch inside an MP3"
    }
   },
   {
    "box": {
     "id": "intro",
     "maxclass": "comment",
     "patching_rect": [
      26,
      58,
      910,
      35
     ],
     "fontsize": 15.0,
     "text": "Move between the two decodes while the source plays. Both come from the same encoded stereo frames."
    }
   },
   {
    "box": {
     "id": "left",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      121,
      82,
      22
     ],
     "text": "saw~ 197"
    }
   },
   {
    "box": {
     "id": "right",
     "maxclass": "newobj",
     "patching_rect": [
      159,
      121,
      82,
      22
     ],
     "text": "cycle~ 997"
    }
   },
   {
    "box": {
     "id": "pack",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      162,
      117,
      22
     ],
     "text": "mc.pack~ 2"
    }
   },
   {
    "box": {
     "id": "level",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      200,
      105,
      22
     ],
     "text": "mc.*~ 0.35"
    }
   },
   {
    "box": {
     "id": "enc",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      243,
      600,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3encode~"
    }
   },
   {
    "box": {
     "id": "src",
     "maxclass": "comment",
     "patching_rect": [
      320,
      101,
      560,
      32
     ],
     "text": "Patch a sound file or an MC signal into the encoder to use your own source."
    }
   },
   {
    "box": {
     "id": "qchoice",
     "maxclass": "comment",
     "patching_rect": [
      310,
      145,
      143,
      21
     ],
     "fontface": 1,
     "text": "Spectral source"
    }
   },
   {
    "box": {
     "id": "source",
     "maxclass": "umenu",
     "patching_rect": [
      310,
      172,
      137,
      23
     ],
     "items": [
      "Encoded integers",
      ",",
      "Oscillator bank"
     ]
    }
   },
   {
    "box": {
     "id": "srcplus",
     "maxclass": "newobj",
     "patching_rect": [
      310,
      206,
      41,
      22
     ],
     "text": "+ 1"
    }
   },
   {
    "box": {
     "id": "bank",
     "maxclass": "newobj",
     "patching_rect": [
      516,
      144,
      177,
      22
     ],
     "text": "mc.cycle~ 31 @chans 64"
    }
   },
   {
    "box": {
     "id": "rawq",
     "maxclass": "newobj",
     "patching_rect": [
      516,
      181,
      80,
      22
     ],
     "text": "mc.*~ 8."
    }
   },
   {
    "box": {
     "id": "srcsel",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      278,
      166,
      22
     ],
     "text": "mc.selector~ 2 1"
    }
   },
   {
    "box": {
     "id": "editlabel",
     "maxclass": "comment",
     "patching_rect": [
      682,
      211,
      232,
      22
     ],
     "fontface": 1,
     "text": "Spectral edit"
    }
   },
   {
    "box": {
     "id": "pass",
     "maxclass": "message",
     "patching_rect": [
      682,
      246,
      32,
      22
     ],
     "text": "1."
    }
   },
   {
    "box": {
     "id": "comb",
     "maxclass": "message",
     "patching_rect": [
      730,
      246,
      163,
      22
     ],
     "text": "replicatevalues 0 1 1 1"
    }
   },
   {
    "box": {
     "id": "mask",
     "maxclass": "newobj",
     "patching_rect": [
      682,
      288,
      161,
      22
     ],
     "text": "mc.sig~ 1 @chans 64"
    }
   },
   {
    "box": {
     "id": "mul",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      321,
      82,
      22
     ],
     "text": "mc.*~ 1."
    }
   },
   {
    "box": {
     "id": "dec",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      370,
      467,
      22
     ],
     "fontsize": 15.0,
     "text": "mp3decode~"
    }
   },
   {
    "box": {
     "id": "packed",
     "maxclass": "newobj",
     "patching_rect": [
      610,
      370,
      161,
      22
     ],
     "fontsize": 15.0,
     "text": "mp3decode~ bytes"
    }
   },
   {
    "box": {
     "id": "choice",
     "maxclass": "umenu",
     "patching_rect": [
      28,
      414,
      174,
      23
     ],
     "items": [
      "Edited fields",
      ",",
      "Original MP3 bytes"
     ]
    }
   },
   {
    "box": {
     "id": "one",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      456,
      39,
      22
     ],
     "text": "+ 1"
    }
   },
   {
    "box": {
     "id": "select",
     "maxclass": "newobj",
     "patching_rect": [
      225,
      462,
      160,
      22
     ],
     "text": "mc.selector~ 2 1"
    }
   },
   {
    "box": {
     "id": "volume",
     "maxclass": "newobj",
     "patching_rect": [
      225,
      502,
      92,
      22
     ],
     "text": "mc.*~ 0.18"
    }
   },
   {
    "box": {
     "id": "limit",
     "maxclass": "newobj",
     "patching_rect": [
      225,
      541,
      121,
      22
     ],
     "text": "mc.clip~ -0.8 0.8"
    }
   },
   {
    "box": {
     "id": "unpack",
     "maxclass": "newobj",
     "patching_rect": [
      225,
      580,
      102,
      22
     ],
     "text": "mc.unpack~ 2"
    }
   },
   {
    "box": {
     "id": "speaker",
     "maxclass": "ezdac~",
     "patching_rect": [
      225,
      622,
      45,
      45
     ]
    }
   },
   {
    "box": {
     "id": "local",
     "maxclass": "newobj",
     "patching_rect": [
      275,
      621,
      60,
      22
     ],
     "hidden": 1,
     "text": "loadmess local 1"
    }
   },
   {
    "box": {
     "id": "tip",
     "maxclass": "comment",
     "patching_rect": [
      446,
      448,
      458,
      74
     ],
     "linecount": 4,
     "text": "Click the speaker. Choose Oscillator bank to replace the spectral integers while keeping the source’s gains and windows. The repeating mask erases every fourth subband."
    }
   },
   {
    "box": {
     "id": "pins",
     "maxclass": "comment",
     "patching_rect": [
      446,
      545,
      468,
      80
     ],
     "linecount": 4,
     "text": "Encoder outlets, left to right: spectral integers, attenuation, global gain, window type, granule sync, packed bytes. The first five patch to the matching decoder inlets."
    }
   },
   {
    "box": {
     "id": "newfeatures",
     "maxclass": "message",
     "patching_rect": [
      24,
      690,
      140,
      22
     ],
     "text": "help mp3frames~"
    }
   },
   {
    "box": {
     "id": "more",
     "maxclass": "newobj",
     "patching_rect": [
      183,
      690,
      71,
      22
     ],
     "text": "pcontrol"
    }
   },
   {
    "box": {
     "id": "newlabel",
     "maxclass": "comment",
     "patching_rect": [
      285,
      690,
      560,
      22
     ],
     "text": "Frame memory, level-aware hold and packed-frame audition"
    }
   },
   {
    "box": {
     "id": "kblabel",
     "maxclass": "comment",
     "patching_rect": [
      25,
      721,
      155,
      23
     ],
     "fontface": 1,
     "text": "Live bitrate (kb/s)"
    }
   },
   {
    "box": {
     "id": "kbdesc",
     "maxclass": "comment",
     "patching_rect": [
      412,
      721,
      485,
      28
     ],
     "text": "Changes the live encoder. 0 restores its automatic bitrate."
    }
   },
   {
    "box": {
     "id": "kbnumber",
     "maxclass": "number",
     "patching_rect": [
      192,
      721,
      59,
      23
     ],
     "minimum": 0,
     "maximum": 320
    }
   },
   {
    "box": {
     "id": "kbmsg",
     "maxclass": "newobj",
     "patching_rect": [
      265,
      721,
      122,
      22
     ],
     "text": "prepend bitrate"
    }
   },
   {
    "box": {
     "id": "dstitle",
     "maxclass": "comment",
     "patching_rect": [
      24,
      756,
      160,
      22
     ],
     "text": "Codec divisor (1 = full)"
    }
   },
   {
    "box": {
     "id": "dsn",
     "maxclass": "number",
     "patching_rect": [
      192,
      756,
      53,
      22
     ],
     "minimum": 1,
     "maximum": 6,
     "parameter_enable": 0
    }
   },
   {
    "box": {
     "id": "dsmsg",
     "maxclass": "newobj",
     "patching_rect": [
      261,
      756,
      142,
      22
     ],
     "text": "prepend downsample"
    }
   },
   {
    "box": {
     "id": "dsinit",
     "maxclass": "newobj",
     "patching_rect": [
      192,
      787,
      119,
      22
     ],
     "hidden": 1,
     "text": "loadmess set 1"
    }
   },
   {
    "box": {
     "id": "dstip",
     "maxclass": "comment",
     "patching_rect": [
      413,
      756,
      540,
      31
     ],
     "text": "2 gives half rate. The encoder and both listening paths follow together."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "left",
      0
     ],
     "destination": [
      "pack",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "right",
      0
     ],
     "destination": [
      "pack",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack",
      0
     ],
     "destination": [
      "level",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "level",
      0
     ],
     "destination": [
      "enc",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "source",
      0
     ],
     "destination": [
      "srcplus",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bank",
      0
     ],
     "destination": [
      "rawq",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "srcplus",
      0
     ],
     "destination": [
      "srcsel",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "enc",
      0
     ],
     "destination": [
      "srcsel",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "rawq",
      0
     ],
     "destination": [
      "srcsel",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pass",
      0
     ],
     "destination": [
      "mask",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "comb",
      0
     ],
     "destination": [
      "mask",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "srcsel",
      0
     ],
     "destination": [
      "mul",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mask",
      0
     ],
     "destination": [
      "mul",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mul",
      0
     ],
     "destination": [
      "dec",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "enc",
      1
     ],
     "destination": [
      "dec",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "enc",
      2
     ],
     "destination": [
      "dec",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "enc",
      3
     ],
     "destination": [
      "dec",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "enc",
      4
     ],
     "destination": [
      "dec",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "enc",
      5
     ],
     "destination": [
      "packed",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "choice",
      0
     ],
     "destination": [
      "one",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "one",
      0
     ],
     "destination": [
      "select",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "dec",
      0
     ],
     "destination": [
      "select",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "packed",
      0
     ],
     "destination": [
      "select",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "local",
      0
     ],
     "destination": [
      "speaker",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "select",
      0
     ],
     "destination": [
      "volume",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "volume",
      0
     ],
     "destination": [
      "limit",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "limit",
      0
     ],
     "destination": [
      "unpack",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "unpack",
      0
     ],
     "destination": [
      "speaker",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "unpack",
      1
     ],
     "destination": [
      "speaker",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "newfeatures",
      0
     ],
     "destination": [
      "more",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "kbnumber",
      0
     ],
     "destination": [
      "kbmsg",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "kbmsg",
      0
     ],
     "destination": [
      "enc",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "dsn",
      0
     ],
     "destination": [
      "dsmsg",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "dsinit",
      0
     ],
     "destination": [
      "dsn",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "dsmsg",
      0
     ],
     "destination": [
      "enc",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "dsmsg",
      0
     ],
     "destination": [
      "dec",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "dsmsg",
      0
     ],
     "destination": [
      "packed",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "MP3 encoder and decoder"
 }
}
