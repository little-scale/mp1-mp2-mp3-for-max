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
     "text": "mp1frames~"
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
     "text": "Record up to 64 completed Layer I frames in this example. An MC age signal selects a different frame for each subband."
    }
   },
   {
    "box": {
     "id": "tone",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      132,
      96,
      22
     ],
     "text": "cycle~ 180."
    }
   },
   {
    "box": {
     "id": "a",
     "maxclass": "message",
     "patching_rect": [
      171,
      129,
      52,
      22
     ],
     "text": "180."
    }
   },
   {
    "box": {
     "id": "b",
     "maxclass": "message",
     "patching_rect": [
      240,
      129,
      52,
      22
     ],
     "text": "550."
    }
   },
   {
    "box": {
     "id": "amplitude",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      174,
      78,
      22
     ],
     "text": "*~ 0.3"
    }
   },
   {
    "box": {
     "id": "enc",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      218,
      535,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp1encode~ 160"
    }
   },
   {
    "box": {
     "id": "history",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      303,
      535,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp1frames~ 64"
    }
   },
   {
    "box": {
     "id": "ages",
     "maxclass": "newobj",
     "patching_rect": [
      670,
      219,
      195,
      22
     ],
     "text": "mc.sig~ 0 @chans 32"
    }
   },
   {
    "box": {
     "id": "all",
     "maxclass": "message",
     "patching_rect": [
      670,
      169,
      36,
      22
     ],
     "text": "12"
    }
   },
   {
    "box": {
     "id": "lane",
     "maxclass": "message",
     "patching_rect": [
      730,
      169,
      98,
      22
     ],
     "text": "setvalue 1 40"
    }
   },
   {
    "box": {
     "id": "live",
     "maxclass": "message",
     "patching_rect": [
      851,
      169,
      36,
      22
     ],
     "text": "0"
    }
   },
   {
    "box": {
     "id": "agecap",
     "maxclass": "comment",
     "patching_rect": [
      668,
      131,
      226,
      23
     ],
     "linecount": 1,
     "text": "Frame age in each lane"
    }
   },
   {
    "box": {
     "id": "record",
     "maxclass": "message",
     "patching_rect": [
      617,
      269,
      75,
      22
     ],
     "text": "record 1"
    }
   },
   {
    "box": {
     "id": "pause",
     "maxclass": "message",
     "patching_rect": [
      709,
      269,
      75,
      22
     ],
     "text": "record 0"
    }
   },
   {
    "box": {
     "id": "clear",
     "maxclass": "message",
     "patching_rect": [
      800,
      269,
      59,
      22
     ],
     "text": "clear"
    }
   },
   {
    "box": {
     "id": "out",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      373,
      535,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp1decode~ mc"
    }
   },
   {
    "box": {
     "id": "left",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      436,
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
      473,
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
      515,
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
      523,
      212,
      36
     ],
     "linecount": 2,
     "text": "Click the speaker to listen."
    }
   },
   {
    "box": {
     "id": "tip",
     "maxclass": "comment",
     "patching_rect": [
      386,
      446,
      500,
      89
     ],
     "linecount": 5,
     "text": "With audio on, switch the pitch and try age 12. Age 0 follows the input. Age 1 selects the most recently completed frame. Record 0 keeps the current history available."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "a",
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
      "b",
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
      "amplitude",
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
      "enc",
      0
     ],
     "destination": [
      "history",
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
      "history",
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
      "history",
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
      "history",
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
      "history",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "ages",
      0
     ],
     "destination": [
      "history",
      5
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "all",
      0
     ],
     "destination": [
      "ages",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "lane",
      0
     ],
     "destination": [
      "ages",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "live",
      0
     ],
     "destination": [
      "ages",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "record",
      0
     ],
     "destination": [
      "history",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pause",
      0
     ],
     "destination": [
      "history",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clear",
      0
     ],
     "destination": [
      "history",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "history",
      0
     ],
     "destination": [
      "out",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "history",
      1
     ],
     "destination": [
      "out",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "history",
      2
     ],
     "destination": [
      "out",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "history",
      3
     ],
     "destination": [
      "out",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "history",
      4
     ],
     "destination": [
      "out",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "out",
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
   }
  ],
  "title": "mp1frames~"
 }
}
