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
     "text": "mp2clock~"
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
     "text": "Inspect the 36 subband slots in each Layer II frame. Each slot spans 32 codec samples."
    }
   },
   {
    "box": {
     "id": "full",
     "maxclass": "message",
     "patching_rect": [
      28,
      136,
      115,
      22
     ],
     "text": "downsample 1"
    }
   },
   {
    "box": {
     "id": "half",
     "maxclass": "message",
     "patching_rect": [
      163,
      136,
      115,
      22
     ],
     "text": "downsample 2"
    }
   },
   {
    "box": {
     "id": "clock",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      206,
      676,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.27,
      0.58,
      0.62,
      1.0
     ],
     "text": "mp2clock~"
    }
   },
   {
    "box": {
     "id": "snap",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      291,
      111,
      22
     ],
     "text": "snapshot~ 73"
    }
   },
   {
    "box": {
     "id": "row",
     "maxclass": "number",
     "patching_rect": [
      28,
      343,
      66,
      22
     ]
    }
   },
   {
    "box": {
     "id": "rows",
     "maxclass": "comment",
     "patching_rect": [
      28,
      383,
      150,
      36
     ],
     "linecount": 2,
     "text": "Current slot"
    }
   },
   {
    "box": {
     "id": "frame-edge",
     "maxclass": "newobj",
     "patching_rect": [
      258,
      291,
      65,
      22
     ],
     "text": "edge~"
    }
   },
   {
    "box": {
     "id": "frame-sum",
     "maxclass": "newobj",
     "patching_rect": [
      258,
      331,
      69,
      22
     ],
     "text": "counter"
    }
   },
   {
    "box": {
     "id": "frame-display",
     "maxclass": "number",
     "patching_rect": [
      258,
      373,
      82,
      22
     ]
    }
   },
   {
    "box": {
     "id": "frame-label",
     "maxclass": "comment",
     "patching_rect": [
      258,
      414,
      190,
      36
     ],
     "linecount": 2,
     "text": "Frame starts"
    }
   },
   {
    "box": {
     "id": "group-edge",
     "maxclass": "newobj",
     "patching_rect": [
      480,
      291,
      65,
      22
     ],
     "text": "edge~"
    }
   },
   {
    "box": {
     "id": "group-sum",
     "maxclass": "newobj",
     "patching_rect": [
      480,
      331,
      69,
      22
     ],
     "text": "counter"
    }
   },
   {
    "box": {
     "id": "group-display",
     "maxclass": "number",
     "patching_rect": [
      480,
      373,
      82,
      22
     ]
    }
   },
   {
    "box": {
     "id": "group-label",
     "maxclass": "comment",
     "patching_rect": [
      480,
      414,
      190,
      36
     ],
     "linecount": 2,
     "text": "Scalefactor group starts"
    }
   },
   {
    "box": {
     "id": "audio",
     "maxclass": "ezdac~",
     "patching_rect": [
      28,
      482,
      43,
      43
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
      491,
      201,
      36
     ],
     "linecount": 2,
     "text": "Click to run the clock."
    }
   },
   {
    "box": {
     "id": "clockhelp",
     "maxclass": "comment",
     "patching_rect": [
      596,
      462,
      310,
      99
     ],
     "linecount": 5,
     "text": "Patch outlet 4 to the slot-marker inlet of a packer or decoder to synchronize your oscillators. Patch an encoder or file reader\u2019s slot-marker outlet into the clock to follow an existing stream."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "full",
      0
     ],
     "destination": [
      "clock",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "half",
      0
     ],
     "destination": [
      "clock",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clock",
      0
     ],
     "destination": [
      "snap",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "snap",
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
      "frame-edge",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "frame-edge",
      0
     ],
     "destination": [
      "frame-sum",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "frame-sum",
      0
     ],
     "destination": [
      "frame-display",
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
      "group-edge",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "group-edge",
      0
     ],
     "destination": [
      "group-sum",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "group-sum",
      0
     ],
     "destination": [
      "group-display",
      0
     ]
    }
   }
  ],
  "title": "mp2clock~"
 }
}
