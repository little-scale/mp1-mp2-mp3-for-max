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
     "text": "mp1unpack~"
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
     "text": "Open a Layer I file, edit its subband words and write a new file. Turn on DSP, choose read, choose write, then start playback."
    }
   },
   {
    "box": {
     "id": "read",
     "maxclass": "message",
     "patching_rect": [
      28,
      125,
      61,
      22
     ],
     "text": "read"
    }
   },
   {
    "box": {
     "id": "start",
     "maxclass": "message",
     "patching_rect": [
      108,
      125,
      61,
      22
     ],
     "text": "start"
    }
   },
   {
    "box": {
     "id": "stopread",
     "maxclass": "message",
     "patching_rect": [
      188,
      125,
      61,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "source",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      178,
      570,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp1unpack~"
    }
   },
   {
    "box": {
     "id": "mask",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      270,
      88,
      22
     ],
     "text": "mc.*~"
    }
   },
   {
    "box": {
     "id": "mute",
     "maxclass": "message",
     "patching_rect": [
      671,
      126,
      91,
      22
     ],
     "text": "setvalue 3 0"
    }
   },
   {
    "box": {
     "id": "unmute",
     "maxclass": "message",
     "patching_rect": [
      784,
      126,
      91,
      22
     ],
     "text": "setvalue 3 1"
    }
   },
   {
    "box": {
     "id": "maskbank",
     "maxclass": "newobj",
     "patching_rect": [
      671,
      173,
      193,
      22
     ],
     "text": "mc.sig~ 1 @chans 64"
    }
   },
   {
    "box": {
     "id": "mask-label",
     "maxclass": "comment",
     "patching_rect": [
      660,
      216,
      249,
      49
     ],
     "linecount": 2,
     "text": "Mute or restore the third subband of the first channel."
    }
   },
   {
    "box": {
     "id": "sink",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      355,
      570,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp1pack~"
    }
   },
   {
    "box": {
     "id": "done",
     "maxclass": "newobj",
     "patching_rect": [
      658,
      293,
      95,
      22
     ],
     "text": "route done"
    }
   },
   {
    "box": {
     "id": "finish",
     "maxclass": "message",
     "patching_rect": [
      658,
      329,
      55,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "write",
     "maxclass": "message",
     "patching_rect": [
      237,
      304,
      54,
      22
     ],
     "text": "write"
    }
   },
   {
    "box": {
     "id": "listen-on",
     "maxclass": "message",
     "patching_rect": [
      311,
      304,
      80,
      22
     ],
     "text": "monitor 1"
    }
   },
   {
    "box": {
     "id": "hold",
     "maxclass": "message",
     "patching_rect": [
      403,
      304,
      70,
      22
     ],
     "text": "freeze 1"
    }
   },
   {
    "box": {
     "id": "release",
     "maxclass": "message",
     "patching_rect": [
      493,
      304,
      70,
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
      416,
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
      453,
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
      495,
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
      503,
      212,
      36
     ],
     "linecount": 2,
     "text": "Click the speaker to listen."
    }
   },
   {
    "box": {
     "id": "status",
     "maxclass": "newobj",
     "patching_rect": [
      591,
      416,
      150,
      22
     ],
     "text": "print sink-status"
    }
   },
   {
    "box": {
     "id": "tip",
     "maxclass": "comment",
     "patching_rect": [
      329,
      472,
      560,
      68
     ],
     "linecount": 4,
     "text": "The fifth cable carries source format and bitrate. End of file closes the take. Send a bitrate message to the packer to override the source rate."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "read",
      0
     ],
     "destination": [
      "source",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "start",
      0
     ],
     "destination": [
      "source",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "stopread",
      0
     ],
     "destination": [
      "source",
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
      "mask",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mute",
      0
     ],
     "destination": [
      "maskbank",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "unmute",
      0
     ],
     "destination": [
      "maskbank",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "maskbank",
      0
     ],
     "destination": [
      "mask",
      1
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
      "sink",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "source",
      1
     ],
     "destination": [
      "sink",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "source",
      2
     ],
     "destination": [
      "sink",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "source",
      3
     ],
     "destination": [
      "sink",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "source",
      4
     ],
     "destination": [
      "sink",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "source",
      5
     ],
     "destination": [
      "done",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "done",
      0
     ],
     "destination": [
      "finish",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "finish",
      0
     ],
     "destination": [
      "sink",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "write",
      0
     ],
     "destination": [
      "sink",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "listen-on",
      0
     ],
     "destination": [
      "sink",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "hold",
      0
     ],
     "destination": [
      "sink",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "release",
      0
     ],
     "destination": [
      "sink",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "sink",
      6
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
      "sink",
      5
     ],
     "destination": [
      "status",
      0
     ]
    }
   }
  ],
  "title": "mp1unpack~"
 }
}
