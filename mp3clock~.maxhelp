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
   760,
   419
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
      22,
      12,
      712,
      38
     ],
     "fontsize": 26.0,
     "fontface": 1,
     "text": "mp3clock~"
    }
   },
   {
    "box": {
     "id": "sub",
     "maxclass": "comment",
     "patching_rect": [
      24,
      54,
      712,
      34
     ],
     "text": "Drive oscillator patches on the same row, granule and frame boundaries as the MP3 objects."
    }
   },
   {
    "box": {
     "id": "clock",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      123,
      330,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.3,
      0.55,
      0.47,
      1.0
     ],
     "text": "mp3clock~"
    }
   },
   {
    "box": {
     "id": "row",
     "maxclass": "newobj",
     "patching_rect": [
      23,
      197,
      102,
      22
     ],
     "text": "number~"
    }
   },
   {
    "box": {
     "id": "frame",
     "maxclass": "newobj",
     "patching_rect": [
      185,
      196,
      75,
      22
     ],
     "text": "edge~"
    }
   },
   {
    "box": {
     "id": "frameb",
     "maxclass": "button",
     "patching_rect": [
      185,
      237,
      24,
      24
     ]
    }
   },
   {
    "box": {
     "id": "granule",
     "maxclass": "newobj",
     "patching_rect": [
      357,
      196,
      75,
      22
     ],
     "text": "edge~"
    }
   },
   {
    "box": {
     "id": "granuleb",
     "maxclass": "button",
     "patching_rect": [
      357,
      237,
      24,
      24
     ]
    }
   },
   {
    "box": {
     "id": "r",
     "maxclass": "comment",
     "patching_rect": [
      23,
      163,
      140,
      22
     ],
     "text": "Row 0–17"
    }
   },
   {
    "box": {
     "id": "f",
     "maxclass": "comment",
     "patching_rect": [
      185,
      164,
      130,
      22
     ],
     "text": "New frame"
    }
   },
   {
    "box": {
     "id": "g",
     "maxclass": "comment",
     "patching_rect": [
      357,
      164,
      150,
      22
     ],
     "text": "New granule"
    }
   },
   {
    "box": {
     "id": "dac",
     "maxclass": "ezdac~",
     "patching_rect": [
      23,
      296,
      45,
      45
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "use",
     "maxclass": "comment",
     "patching_rect": [
      170,
      281,
      546,
      86
     ],
     "text": "Turn on audio to run the clock. For an encoded source, mp3encode~ outlets 7 and 8 already supply its row and frame pulse. A full-frame pulse can also synchronize the clock inlet."
    }
   },
   {
    "box": {
     "id": "dstitle",
     "maxclass": "comment",
     "patching_rect": [
      24,
      374,
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
      374,
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
      374,
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
      405,
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
      374,
      335,
      31
     ],
     "text": "2 gives half rate. Watch the row and frame pulses slow down."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "clock",
      0
     ],
     "destination": [
      "row",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clock",
      1
     ],
     "destination": [
      "frame",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "frame",
      0
     ],
     "destination": [
      "frameb",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clock",
      2
     ],
     "destination": [
      "granule",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "granule",
      0
     ],
     "destination": [
      "granuleb",
      0
     ]
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
      "clock",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "MP3 codec clock"
 }
}
