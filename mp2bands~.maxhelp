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
   978,
   620
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
      25,
      13,
      410,
      38
     ],
     "fontsize": 27.0,
     "fontface": 1,
     "text": "mp2bands~"
    }
   },
   {
    "box": {
     "id": "sub",
     "maxclass": "comment",
     "patching_rect": [
      28,
      58,
      920,
      25
     ],
     "fontsize": 15.0,
     "text": "Route up to 32 oscillators into MPEG Layer II synthesis bands. Draw allocation and scale for each destination band."
    }
   },
   {
    "box": {
     "id": "datahead",
     "maxclass": "comment",
     "patching_rect": [
      26,
      106,
      164,
      22
     ],
     "fontface": 1,
     "text": "Subband data"
    }
   },
   {
    "box": {
     "id": "freeze",
     "maxclass": "message",
     "patching_rect": [
      26,
      142,
      39,
      22
     ],
     "text": "73"
    }
   },
   {
    "box": {
     "id": "spread",
     "maxclass": "message",
     "patching_rect": [
      76,
      142,
      108,
      22
     ],
     "text": "spread 47 313"
    }
   },
   {
    "box": {
     "id": "osc",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      202,
      164,
      22
     ],
     "text": "mc.cycle~ 73 @chans 32"
    }
   },
   {
    "box": {
     "id": "addresshead",
     "maxclass": "comment",
     "patching_rect": [
      205,
      106,
      156,
      22
     ],
     "fontface": 1,
     "text": "Band addresses"
    }
   },
   {
    "box": {
     "id": "normal",
     "maxclass": "message",
     "patching_rect": [
      205,
      142,
      83,
      22
     ],
     "text": "spread 0 32"
    }
   },
   {
    "box": {
     "id": "reverse",
     "maxclass": "message",
     "patching_rect": [
      205,
      174,
      92,
      22
     ],
     "text": "spread 31 -1"
    }
   },
   {
    "box": {
     "id": "adr",
     "maxclass": "newobj",
     "patching_rect": [
      205,
      230,
      137,
      22
     ],
     "text": "mc.sig~ @chans 32"
    }
   },
   {
    "box": {
     "id": "adrinit",
     "maxclass": "newobj",
     "patching_rect": [
      205,
      117,
      110,
      22
     ],
     "hidden": 1,
     "text": "loadmess spread 0 32"
    }
   },
   {
    "box": {
     "id": "bitscaption",
     "maxclass": "comment",
     "patching_rect": [
      373,
      106,
      260,
      20
     ],
     "fontface": 1,
     "text": "Allocation class (0–17, 0 mutes)"
    }
   },
   {
    "box": {
     "id": "bits",
     "maxclass": "multislider",
     "patching_rect": [
      373,
      136,
      260,
      76
     ],
     "size": 32,
     "setminmax": [
      0.0,
      17.0
     ],
     "settype": 0,
     "orientation": 1,
     "contdata": 1
    }
   },
   {
    "box": {
     "id": "bitsinit",
     "maxclass": "newobj",
     "patching_rect": [
      373,
      110,
      80,
      22
     ],
     "hidden": 1,
     "text": "loadmess 7 0 5 0 3 0 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
    }
   },
   {
    "box": {
     "id": "bitslist",
     "maxclass": "newobj",
     "patching_rect": [
      373,
      228,
      144,
      22
     ],
     "text": "prepend applyvalues"
    }
   },
   {
    "box": {
     "id": "bitssig",
     "maxclass": "newobj",
     "patching_rect": [
      373,
      268,
      140,
      22
     ],
     "text": "mc.sig~ @chans 32"
    }
   },
   {
    "box": {
     "id": "scalecaption",
     "maxclass": "comment",
     "patching_rect": [
      665,
      106,
      266,
      20
     ],
     "fontface": 1,
     "text": "Scale index  •  lower is louder"
    }
   },
   {
    "box": {
     "id": "scale",
     "maxclass": "multislider",
     "patching_rect": [
      665,
      136,
      266,
      76
     ],
     "size": 32,
     "setminmax": [
      0.0,
      62.0
     ],
     "settype": 0,
     "orientation": 1,
     "contdata": 1
    }
   },
   {
    "box": {
     "id": "scaleinit",
     "maxclass": "newobj",
     "patching_rect": [
      665,
      110,
      80,
      22
     ],
     "hidden": 1,
     "text": "loadmess 12 12 12 13 13 13 14 14 14 15 15 15 16 16 16 17 17 17 18 18 18 19 19 19 20 20 20 21 21 21 22 22"
    }
   },
   {
    "box": {
     "id": "scalelist",
     "maxclass": "newobj",
     "patching_rect": [
      665,
      228,
      144,
      22
     ],
     "text": "prepend applyvalues"
    }
   },
   {
    "box": {
     "id": "scalesig",
     "maxclass": "newobj",
     "patching_rect": [
      665,
      268,
      140,
      22
     ],
     "text": "mc.sig~ @chans 32"
    }
   },
   {
    "box": {
     "id": "decode",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      348,
      798,
      22
     ],
     "fontsize": 16.0,
     "color": [
      0.32,
      0.64,
      0.72,
      1.0
     ],
     "text": "mp2bands~"
    }
   },
   {
    "box": {
     "id": "gain",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      396,
      70,
      22
     ],
     "text": "*~ 0.16"
    }
   },
   {
    "box": {
     "id": "dac",
     "maxclass": "ezdac~",
     "patching_rect": [
      26,
      446,
      50,
      50
     ]
    }
   },
   {
    "box": {
     "id": "local",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      430,
      90,
      22
     ],
     "hidden": 1,
     "text": "loadmess local 1"
    }
   },
   {
    "box": {
     "id": "spec",
     "maxclass": "spectroscope~",
     "patching_rect": [
      129,
      403,
      334,
      110
     ]
    }
   },
   {
    "box": {
     "id": "listen",
     "maxclass": "comment",
     "patching_rect": [
      24,
      510,
      105,
      22
     ],
     "text": "Click to listen."
    }
   },
   {
    "box": {
     "id": "details",
     "maxclass": "comment",
     "patching_rect": [
      485,
      396,
      463,
      72
     ],
     "linecount": 4,
     "text": "Click spread 47 313, then reverse the addresses. Each oscillator changes destination. Allocation and scale stay with their destination bands. A mono address rotates the bank."
    }
   },
   {
    "box": {
     "id": "fields",
     "maxclass": "comment",
     "patching_rect": [
      485,
      473,
      463,
      74
     ],
     "linecount": 4,
     "text": "The default input is normalized (−1…+1). Use mp2bands~ signed for centered integer words or mp2bands~ raw for unsigned quantizer digits."
    }
   },
   {
    "box": {
     "id": "time",
     "maxclass": "comment",
     "patching_rect": [
      25,
      562,
      920,
      39
     ],
     "text": "Words and addresses latch every 32 codec samples. Allocation latches every 1,152 samples and scale every 384. The outlets carry mono audio, scan column (0–31) and a frame-start pulse."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "adrinit",
      0
     ],
     "destination": [
      "adr",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "normal",
      0
     ],
     "destination": [
      "adr",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "reverse",
      0
     ],
     "destination": [
      "adr",
      0
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
      "osc",
      0
     ]
    }
   },
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
      "bitsinit",
      0
     ],
     "destination": [
      "bits",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "bits",
      0
     ],
     "destination": [
      "bitslist",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bitslist",
      0
     ],
     "destination": [
      "bitssig",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "scaleinit",
      0
     ],
     "destination": [
      "scale",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "scale",
      0
     ],
     "destination": [
      "scalelist",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "scalelist",
      0
     ],
     "destination": [
      "scalesig",
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
      "decode",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "adr",
      0
     ],
     "destination": [
      "decode",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bitssig",
      0
     ],
     "destination": [
      "decode",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "scalesig",
      0
     ],
     "destination": [
      "decode",
      3
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
      "dac",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "decode",
      0
     ],
     "destination": [
      "gain",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "gain",
      0
     ],
     "destination": [
      "dac",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "gain",
      0
     ],
     "destination": [
      "dac",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "gain",
      0
     ],
     "destination": [
      "spec",
      0
     ]
    }
   }
  ],
  "title": "mp2bands~"
 }
}
