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
   940,
   770
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
      892,
      38
     ],
     "fontsize": 26.0,
     "fontface": 1,
     "text": "mp3frames~"
    }
   },
   {
    "box": {
     "id": "sub",
     "maxclass": "comment",
     "patching_rect": [
      24,
      54,
      892,
      34
     ],
     "text": "Capture a short phrase of spectral frames, park individual bands in its history, and record the result."
    }
   },
   {
    "box": {
     "id": "bank",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      138,
      104,
      22
     ],
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
       550,
       260
      ],
      "default_fontsize": 13.0,
      "default_fontname": "Arial",
      "openinpresentation": 0,
      "boxes": [
       {
        "box": {
         "id": "o0",
         "maxclass": "newobj",
         "patching_rect": [
          18,
          25,
          90,
          22
         ],
         "text": "cycle~ 5.1"
        }
       },
       {
        "box": {
         "id": "g0",
         "maxclass": "newobj",
         "patching_rect": [
          18,
          75,
          72,
          22
         ],
         "text": "*~ 6."
        }
       },
       {
        "box": {
         "id": "o1",
         "maxclass": "newobj",
         "patching_rect": [
          143,
          25,
          90,
          22
         ],
         "text": "cycle~ 8.7"
        }
       },
       {
        "box": {
         "id": "g1",
         "maxclass": "newobj",
         "patching_rect": [
          143,
          75,
          72,
          22
         ],
         "text": "*~ 6."
        }
       },
       {
        "box": {
         "id": "o2",
         "maxclass": "newobj",
         "patching_rect": [
          268,
          25,
          90,
          22
         ],
         "text": "cycle~ 3.8"
        }
       },
       {
        "box": {
         "id": "g2",
         "maxclass": "newobj",
         "patching_rect": [
          268,
          75,
          72,
          22
         ],
         "text": "*~ 6."
        }
       },
       {
        "box": {
         "id": "o3",
         "maxclass": "newobj",
         "patching_rect": [
          393,
          25,
          90,
          22
         ],
         "text": "cycle~ 11.3"
        }
       },
       {
        "box": {
         "id": "g3",
         "maxclass": "newobj",
         "patching_rect": [
          393,
          75,
          72,
          22
         ],
         "text": "*~ 6."
        }
       },
       {
        "box": {
         "id": "pack",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          133,
          450,
          22
         ],
         "text": "mc.pack~ 32"
        }
       },
       {
        "box": {
         "id": "out",
         "maxclass": "outlet",
         "patching_rect": [
          20,
          201,
          25,
          25
         ],
         "index": 1
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "o0",
          0
         ],
         "destination": [
          "g0",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "g0",
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
          "o1",
          0
         ],
         "destination": [
          "g1",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "g1",
          0
         ],
         "destination": [
          "pack",
          3
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "o2",
          0
         ],
         "destination": [
          "g2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "g2",
          0
         ],
         "destination": [
          "pack",
          9
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "o3",
          0
         ],
         "destination": [
          "g3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "g3",
          0
         ],
         "destination": [
          "pack",
          17
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
          "out",
          0
         ]
        }
       }
      ]
     },
     "text": "p oscillators"
    }
   },
   {
    "box": {
     "id": "clocklabel",
     "maxclass": "comment",
     "patching_rect": [
      162,
      107,
      105,
      22
     ],
     "text": "Frame sync"
    }
   },
   {
    "box": {
     "id": "clk",
     "maxclass": "newobj",
     "patching_rect": [
      162,
      138,
      90,
      22
     ],
     "text": "mp3clock~"
    }
   },
   {
    "box": {
     "id": "agelabel",
     "maxclass": "comment",
     "patching_rect": [
      313,
      93,
      390,
      22
     ],
     "text": "Read age in frames · 0 follows live"
    }
   },
   {
    "box": {
     "id": "age0",
     "maxclass": "message",
     "patching_rect": [
      313,
      122,
      101,
      22
     ],
     "text": "setvalue 1 6"
    }
   },
   {
    "box": {
     "id": "age1",
     "maxclass": "message",
     "patching_rect": [
      421,
      122,
      106,
      22
     ],
     "text": "setvalue 4 14"
    }
   },
   {
    "box": {
     "id": "age2",
     "maxclass": "message",
     "patching_rect": [
      533,
      122,
      111,
      22
     ],
     "text": "setvalue 10 3"
    }
   },
   {
    "box": {
     "id": "age3",
     "maxclass": "message",
     "patching_rect": [
      651,
      122,
      253,
      22
     ],
     "text": "applyvalues 0 0 0 0 0 0 0 0 0 0"
    }
   },
   {
    "box": {
     "id": "ages",
     "maxclass": "newobj",
     "patching_rect": [
      313,
      162,
      177,
      22
     ],
     "text": "mc.sig~ 0 @chans 32"
    }
   },
   {
    "box": {
     "id": "off",
     "maxclass": "message",
     "patching_rect": [
      24,
      193,
      76,
      22
     ],
     "text": "record 0"
    }
   },
   {
    "box": {
     "id": "on",
     "maxclass": "message",
     "patching_rect": [
      106,
      193,
      76,
      22
     ],
     "text": "record 1"
    }
   },
   {
    "box": {
     "id": "clr",
     "maxclass": "message",
     "patching_rect": [
      192,
      193,
      52,
      22
     ],
     "text": "clear"
    }
   },
   {
    "box": {
     "id": "rlabel",
     "maxclass": "comment",
     "patching_rect": [
      302,
      202,
      535,
      22
     ],
     "text": "Pause memory capture to keep the read positions fixed."
    }
   },
   {
    "box": {
     "id": "mem",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      247,
      610,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.3,
      0.55,
      0.47,
      1.0
     ],
     "text": "mp3frames~ 32"
    }
   },
   {
    "box": {
     "id": "gainlabel",
     "maxclass": "comment",
     "patching_rect": [
      24,
      83,
      100,
      22
     ],
     "text": "Source gain"
    }
   },
   {
    "box": {
     "id": "g0",
     "maxclass": "message",
     "patching_rect": [
      24,
      102,
      45,
      22
     ],
     "text": "153"
    }
   },
   {
    "box": {
     "id": "g1",
     "maxclass": "message",
     "patching_rect": [
      79,
      102,
      45,
      22
     ],
     "text": "165"
    }
   },
   {
    "box": {
     "id": "gain",
     "maxclass": "newobj",
     "patching_rect": [
      178,
      167,
      85,
      22
     ],
     "text": "sig~ 153"
    }
   },
   {
    "box": {
     "id": "mask",
     "maxclass": "newobj",
     "patching_rect": [
      660,
      288,
      175,
      22
     ],
     "text": "mc.sig~ 0 @chans 32"
    }
   },
   {
    "box": {
     "id": "hold",
     "maxclass": "message",
     "patching_rect": [
      652,
      249,
      105,
      22
     ],
     "text": "setvalue 1 1"
    }
   },
   {
    "box": {
     "id": "release",
     "maxclass": "message",
     "patching_rect": [
      774,
      249,
      108,
      22
     ],
     "text": "setvalue 1 0"
    }
   },
   {
    "box": {
     "id": "m0",
     "maxclass": "message",
     "patching_rect": [
      24,
      324,
      55,
      22
     ],
     "text": "write"
    }
   },
   {
    "box": {
     "id": "m1",
     "maxclass": "message",
     "patching_rect": [
      85,
      324,
      53,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "m2",
     "maxclass": "message",
     "patching_rect": [
      145,
      324,
      54,
      22
     ],
     "text": "stats"
    }
   },
   {
    "box": {
     "id": "m3",
     "maxclass": "message",
     "patching_rect": [
      227,
      324,
      118,
      22
     ],
     "text": "holdmode level"
    }
   },
   {
    "box": {
     "id": "m4",
     "maxclass": "message",
     "patching_rect": [
      351,
      324,
      118,
      22
     ],
     "text": "holdmode coeff"
    }
   },
   {
    "box": {
     "id": "monon",
     "maxclass": "newobj",
     "patching_rect": [
      490,
      324,
      148,
      22
     ],
     "text": "loadmess monitor 1"
    }
   },
   {
    "box": {
     "id": "packer",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      371,
      767,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.3,
      0.55,
      0.47,
      1.0
     ],
     "text": "mp3pack~ 64"
    }
   },
   {
    "box": {
     "id": "decoder",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      423,
      284,
      22
     ],
     "text": "mp3decode~"
    }
   },
   {
    "box": {
     "id": "route",
     "maxclass": "newobj",
     "patching_rect": [
      641,
      424,
      81,
      22
     ],
     "text": "route fit"
    }
   },
   {
    "box": {
     "id": "unfit",
     "maxclass": "newobj",
     "patching_rect": [
      641,
      462,
      115,
      22
     ],
     "text": "unpack i i i i"
    }
   },
   {
    "box": {
     "id": "fits",
     "maxclass": "number",
     "patching_rect": [
      641,
      499,
      90,
      22
     ]
    }
   },
   {
    "box": {
     "id": "fitlbl",
     "maxclass": "comment",
     "patching_rect": [
      641,
      531,
      225,
      22
     ],
     "text": "Fitted channel-granules"
    }
   },
   {
    "box": {
     "id": "choiceLabel",
     "maxclass": "comment",
     "patching_rect": [
      24,
      478,
      75,
      22
     ],
     "text": "Listen to"
    }
   },
   {
    "box": {
     "id": "choice",
     "maxclass": "umenu",
     "patching_rect": [
      24,
      508,
      170,
      22
     ],
     "items": [
      "Live fields",
      ",",
      "Packed recording"
     ]
    }
   },
   {
    "box": {
     "id": "index",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      546,
      54,
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
      219,
      509,
      175,
      22
     ],
     "text": "mc.selector~ 2 1"
    }
   },
   {
    "box": {
     "id": "vol",
     "maxclass": "newobj",
     "patching_rect": [
      219,
      552,
      101,
      22
     ],
     "text": "mc.*~ 0.1"
    }
   },
   {
    "box": {
     "id": "clip",
     "maxclass": "newobj",
     "patching_rect": [
      219,
      589,
      170,
      22
     ],
     "text": "mc.clip~ -0.75 0.75"
    }
   },
   {
    "box": {
     "id": "un",
     "maxclass": "newobj",
     "patching_rect": [
      219,
      626,
      130,
      22
     ],
     "text": "mc.unpack~ 2"
    }
   },
   {
    "box": {
     "id": "dac",
     "maxclass": "ezdac~",
     "patching_rect": [
      24,
      630,
      48,
      48
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "guide",
     "maxclass": "comment",
     "patching_rect": [
      475,
      576,
      432,
      101
     ],
     "text": "Start audio. Pause capture with record 0 and choose band ages. Hold band 1 and switch Source gain to compare the two hold modes. Click write and select Packed recording to hear fitted frames. Stop finishes the take."
    }
   },
   {
    "box": {
     "id": "guide2",
     "maxclass": "comment",
     "patching_rect": [
      24,
      696,
      845,
      22
     ],
     "text": "A frame is 24 ms at 48 kHz. This memory holds 32 frames (768 ms)."
    }
   },
   {
    "box": {
     "id": "dstitle",
     "maxclass": "comment",
     "patching_rect": [
      24,
      725,
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
      725,
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
      725,
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
      756,
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
      725,
      515,
      31
     ],
     "text": "Set before recording. Changing rate starts a fresh frame memory."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "age0",
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
      "age1",
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
      "age2",
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
      "age3",
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
      "bank",
      0
     ],
     "destination": [
      "mem",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "off",
      0
     ],
     "destination": [
      "mem",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "on",
      0
     ],
     "destination": [
      "mem",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clr",
      0
     ],
     "destination": [
      "mem",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clk",
      1
     ],
     "destination": [
      "mem",
      6
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
      "mem",
      5
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "g0",
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
      "g1",
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
      "mem",
      2
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
      "mask",
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
      "mask",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m0",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m1",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m2",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m3",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m4",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "monon",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mem",
      0
     ],
     "destination": [
      "packer",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mem",
      1
     ],
     "destination": [
      "packer",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mem",
      2
     ],
     "destination": [
      "packer",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mem",
      3
     ],
     "destination": [
      "packer",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mem",
      4
     ],
     "destination": [
      "packer",
      4
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
      "packer",
      5
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "packer",
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
      "packer",
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
      "packer",
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
      "packer",
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
      "packer",
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
      "packer",
      5
     ],
     "destination": [
      "route",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "route",
      0
     ],
     "destination": [
      "unfit",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "unfit",
      0
     ],
     "destination": [
      "fits",
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
      "index",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "index",
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
      "decoder",
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
      "packer",
      6
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
      "select",
      0
     ],
     "destination": [
      "vol",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "vol",
      0
     ],
     "destination": [
      "clip",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clip",
      0
     ],
     "destination": [
      "un",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "un",
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
      "un",
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
      "clk",
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
      "mem",
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
      "packer",
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
      "decoder",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "MP3 frame memory and recording"
 }
}
