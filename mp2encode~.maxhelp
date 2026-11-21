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
     "text": "mp2encode~"
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
     "text": "Analyze mono or stereo audio into MPEG Layer II subband words. Hear the same analysis through the field decoder or the byte decoder."
    }
   },
   {
    "box": {
     "id": "tone",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      126,
      100,
      22
     ],
     "text": "cycle~ 220."
    }
   },
   {
    "box": {
     "id": "amplitude",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      171,
      82,
      22
     ],
     "text": "*~ 0.35"
    }
   },
   {
    "box": {
     "id": "t1",
     "maxclass": "message",
     "patching_rect": [
      154,
      124,
      54,
      22
     ],
     "text": "220."
    }
   },
   {
    "box": {
     "id": "t2",
     "maxclass": "message",
     "patching_rect": [
      221,
      124,
      54,
      22
     ],
     "text": "330."
    }
   },
   {
    "box": {
     "id": "encoder",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      223,
      510,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp2encode~ 128"
    }
   },
   {
    "box": {
     "id": "decoder",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      310,
      410,
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
     "id": "bytes",
     "maxclass": "newobj",
     "patching_rect": [
      490,
      310,
      220,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp2decode~ bytes"
    }
   },
   {
    "box": {
     "id": "a",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      364,
      104,
      22
     ],
     "text": "mc.unpack~ 1"
    }
   },
   {
    "box": {
     "id": "b",
     "maxclass": "newobj",
     "patching_rect": [
      490,
      364,
      104,
      22
     ],
     "text": "mc.unpack~ 1"
    }
   },
   {
    "box": {
     "id": "select",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      409,
      192,
      22
     ],
     "text": "selector~ 2 1"
    }
   },
   {
    "box": {
     "id": "choose1",
     "maxclass": "message",
     "patching_rect": [
      266,
      408,
      36,
      22
     ],
     "text": "1"
    }
   },
   {
    "box": {
     "id": "choose2",
     "maxclass": "message",
     "patching_rect": [
      321,
      408,
      36,
      22
     ],
     "text": "2"
    }
   },
   {
    "box": {
     "id": "choice",
     "maxclass": "comment",
     "patching_rect": [
      263,
      443,
      272,
      36
     ],
     "linecount": 2,
     "text": "1 selects fields. 2 selects bytes."
    }
   },
   {
    "box": {
     "id": "trim",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      456,
      76,
      22
     ],
     "text": "*~ 0.2"
    }
   },
   {
    "box": {
     "id": "audio",
     "maxclass": "ezdac~",
     "patching_rect": [
      28,
      503,
      45,
      45
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "list",
     "maxclass": "comment",
     "patching_rect": [
      86,
      512,
      156,
      36
     ],
     "linecount": 2,
     "text": "Click to listen."
    }
   },
   {
    "box": {
     "id": "low",
     "maxclass": "message",
     "patching_rect": [
      622,
      123,
      89,
      22
     ],
     "text": "bitrate 64"
    }
   },
   {
    "box": {
     "id": "high",
     "maxclass": "message",
     "patching_rect": [
      730,
      123,
      96,
      22
     ],
     "text": "bitrate 192"
    }
   },
   {
    "box": {
     "id": "try",
     "maxclass": "comment",
     "patching_rect": [
      620,
      157,
      290,
      42
     ],
     "linecount": 2,
     "text": "Change the bitrate while the oscillator runs."
    }
   },
   {
    "box": {
     "id": "log",
     "maxclass": "newobj",
     "patching_rect": [
      744,
      230,
      150,
      22
     ],
     "text": "print encoder-status"
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "tone",
      0
     ],
     "destination": [
      "amplitude",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "t1",
      0
     ],
     "destination": [
      "tone",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "t2",
      0
     ],
     "destination": [
      "tone",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "amplitude",
      0
     ],
     "destination": [
      "encoder",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      0
     ],
     "destination": [
      "decoder",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      1
     ],
     "destination": [
      "decoder",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      2
     ],
     "destination": [
      "decoder",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      3
     ],
     "destination": [
      "decoder",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      4
     ],
     "destination": [
      "decoder",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      5
     ],
     "destination": [
      "bytes",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "decoder",
      0
     ],
     "destination": [
      "a",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bytes",
      0
     ],
     "destination": [
      "b",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "a",
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
      "b",
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
      "choose1",
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
      "choose2",
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
      "select",
      0
     ],
     "destination": [
      "trim",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim",
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
      "trim",
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
      "low",
      0
     ],
     "destination": [
      "encoder",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "high",
      0
     ],
     "destination": [
      "encoder",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "encoder",
      6
     ],
     "destination": [
      "log",
      0
     ]
    }
   }
  ],
  "title": "mp2encode~"
 }
}
