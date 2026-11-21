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
   950,
   610
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
      15,
      650,
      42
     ],
     "fontsize": 28.0,
     "fontface": 1,
     "text": "mp2decode~"
    }
   },
   {
    "box": {
     "id": "intro",
     "maxclass": "comment",
     "patching_rect": [
      26,
      64,
      895,
      45
     ],
     "fontsize": 15.0,
     "linecount": 2,
     "text": "Sample 32 oscillators directly into Layer II subbands. Change the quantizer and level across the bank, or hold one lane."
    }
   },
   {
    "box": {
     "id": "wordlab",
     "maxclass": "comment",
     "patching_rect": [
      28,
      115,
      155,
      22
     ],
     "linecount": 1,
     "text": "Subband words"
    }
   },
   {
    "box": {
     "id": "spread",
     "maxclass": "message",
     "patching_rect": [
      28,
      144,
      110,
      22
     ],
     "text": "spread 9 73"
    }
   },
   {
    "box": {
     "id": "osc",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      184,
      206,
      22
     ],
     "text": "mc.cycle~ 11 @chans 32"
    }
   },
   {
    "box": {
     "id": "amp",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      226,
      100,
      22
     ],
     "text": "mc.*~ 10."
    }
   },
   {
    "box": {
     "id": "allab",
     "maxclass": "comment",
     "patching_rect": [
      258,
      115,
      135,
      22
     ],
     "linecount": 1,
     "text": "Allocation"
    }
   },
   {
    "box": {
     "id": "al1",
     "maxclass": "message",
     "patching_rect": [
      258,
      144,
      32,
      22
     ],
     "text": "3"
    }
   },
   {
    "box": {
     "id": "al2",
     "maxclass": "message",
     "patching_rect": [
      309,
      144,
      32,
      22
     ],
     "text": "7"
    }
   },
   {
    "box": {
     "id": "alloc",
     "maxclass": "newobj",
     "patching_rect": [
      258,
      185,
      175,
      22
     ],
     "text": "mc.sig~ 5 @chans 32"
    }
   },
   {
    "box": {
     "id": "sflab",
     "maxclass": "comment",
     "patching_rect": [
      455,
      115,
      162,
      22
     ],
     "linecount": 1,
     "text": "Scalefactor index"
    }
   },
   {
    "box": {
     "id": "sf1",
     "maxclass": "message",
     "patching_rect": [
      455,
      144,
      38,
      22
     ],
     "text": "12"
    }
   },
   {
    "box": {
     "id": "sf2",
     "maxclass": "message",
     "patching_rect": [
      511,
      144,
      38,
      22
     ],
     "text": "21"
    }
   },
   {
    "box": {
     "id": "sf",
     "maxclass": "newobj",
     "patching_rect": [
      455,
      185,
      184,
      22
     ],
     "text": "mc.sig~ 18 @chans 32"
    }
   },
   {
    "box": {
     "id": "gate-caption",
     "maxclass": "comment",
     "patching_rect": [
      692,
      113,
      202,
      22
     ],
     "linecount": 1,
     "text": "Hold one MC lane"
    }
   },
   {
    "box": {
     "id": "hold-band",
     "maxclass": "message",
     "patching_rect": [
      692,
      142,
      88,
      22
     ],
     "text": "setvalue 3 1"
    }
   },
   {
    "box": {
     "id": "release-band",
     "maxclass": "message",
     "patching_rect": [
      796,
      142,
      88,
      22
     ],
     "text": "setvalue 3 0"
    }
   },
   {
    "box": {
     "id": "gates",
     "maxclass": "newobj",
     "patching_rect": [
      692,
      184,
      195,
      22
     ],
     "text": "mc.sig~ 0 @chans 32"
    }
   },
   {
    "box": {
     "id": "main",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      305,
      690,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp2decode~ mc"
    }
   },
   {
    "box": {
     "id": "freeze",
     "maxclass": "message",
     "patching_rect": [
      485,
      252,
      75,
      22
     ],
     "text": "freeze 1"
    }
   },
   {
    "box": {
     "id": "thaw",
     "maxclass": "message",
     "patching_rect": [
      581,
      252,
      75,
      22
     ],
     "text": "freeze 0"
    }
   },
   {
    "box": {
     "id": "left",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      386,
      96,
      22
     ],
     "text": "mc.unpack~ 1"
    }
   },
   {
    "box": {
     "id": "volume",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      423,
      70,
      22
     ],
     "text": "*~ 0.16"
    }
   },
   {
    "box": {
     "id": "audio",
     "maxclass": "ezdac~",
     "patching_rect": [
      28,
      465,
      45,
      45
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "listen",
     "maxclass": "comment",
     "patching_rect": [
      90,
      473,
      212,
      36
     ],
     "linecount": 2,
     "text": "Click the speaker to listen."
    }
   },
   {
    "box": {
     "id": "kb0",
     "maxclass": "message",
     "patching_rect": [
      704,
      360,
      90,
      22
     ],
     "text": "bitrate 0"
    }
   },
   {
    "box": {
     "id": "kb64",
     "maxclass": "message",
     "patching_rect": [
      807,
      360,
      98,
      22
     ],
     "text": "bitrate 64"
    }
   },
   {
    "box": {
     "id": "tip",
     "maxclass": "comment",
     "patching_rect": [
      396,
      427,
      493,
      60
     ],
     "linecount": 3,
     "text": "Bitrate 0 plays the injected fields directly. Choose 64 to hear how the packer would fit them into 64 kb/s."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "spread",
      0
     ],
     "destination": [
      "osc",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "osc",
      0
     ],
     "destination": [
      "amp",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "al1",
      0
     ],
     "destination": [
      "alloc",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "al2",
      0
     ],
     "destination": [
      "alloc",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "sf1",
      0
     ],
     "destination": [
      "sf",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "sf2",
      0
     ],
     "destination": [
      "sf",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "hold-band",
      0
     ],
     "destination": [
      "gates",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "release-band",
      0
     ],
     "destination": [
      "gates",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "gates",
      0
     ],
     "destination": [
      "main",
      5
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "amp",
      0
     ],
     "destination": [
      "main",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "alloc",
      0
     ],
     "destination": [
      "main",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "sf",
      0
     ],
     "destination": [
      "main",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "freeze",
      0
     ],
     "destination": [
      "main",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "thaw",
      0
     ],
     "destination": [
      "main",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "main",
      0
     ],
     "destination": [
      "left",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "left",
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
      "audio",
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
      "audio",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "kb0",
      0
     ],
     "destination": [
      "main",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "kb64",
      0
     ],
     "destination": [
      "main",
      0
     ]
    }
   }
  ],
  "title": "mp2decode~"
 }
}
