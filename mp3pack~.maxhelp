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
   945,
   769
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
      14,
      780,
      38
     ],
     "fontsize": 27.0,
     "fontface": 1,
     "text": "Record MP3 fields"
    }
   },
   {
    "box": {
     "id": "intro",
     "maxclass": "comment",
     "patching_rect": [
      26,
      61,
      883,
      45
     ],
     "linecount": 2,
     "fontsize": 15.0,
     "text": "Turn on audio, click write for either source, then click stop to finish the file. Double-click a source to edit its patch."
    }
   },
   {
    "box": {
     "id": "heading0",
     "maxclass": "comment",
     "patching_rect": [
      28,
      120,
      393,
      29
     ],
     "fontsize": 19.0,
     "fontface": 1,
     "text": "Oscillator synthesis"
    }
   },
   {
    "box": {
     "id": "src0",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      165,
      385,
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
       660,
       285
      ],
      "default_fontsize": 13.0,
      "default_fontname": "Arial",
      "openinpresentation": 0,
      "boxes": [
       {
        "box": {
         "id": "t",
         "maxclass": "comment",
         "patching_rect": [
          24,
          14,
          590,
          28
         ],
         "fontsize": 17.0,
         "text": "Four oscillators supply the first four subbands."
        }
       },
       {
        "box": {
         "id": "spread",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          55,
          200,
          22
         ],
         "text": "loadmess spread 9 137"
        }
       },
       {
        "box": {
         "id": "bank",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          99,
          190,
          22
         ],
         "text": "mc.cycle~ 37 @chans 4"
        }
       },
       {
        "box": {
         "id": "amp",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          148,
          90,
          22
         ],
         "text": "mc.*~ 4."
        }
       },
       {
        "box": {
         "id": "g",
         "maxclass": "newobj",
         "patching_rect": [
          295,
          148,
          90,
          22
         ],
         "text": "sig~ 169"
        }
       },
       {
        "box": {
         "id": "out0",
         "maxclass": "outlet",
         "patching_rect": [
          25,
          215,
          25,
          25
         ],
         "index": 1
        }
       },
       {
        "box": {
         "id": "out1",
         "maxclass": "outlet",
         "patching_rect": [
          160,
          215,
          25,
          25
         ],
         "index": 2
        }
       },
       {
        "box": {
         "id": "out2",
         "maxclass": "outlet",
         "patching_rect": [
          295,
          215,
          25,
          25
         ],
         "index": 3
        }
       },
       {
        "box": {
         "id": "zero",
         "maxclass": "newobj",
         "patching_rect": [
          160,
          148,
          62,
          22
         ],
         "text": "sig~ 0"
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
          "bank",
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
          "amp",
          0
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
          "out0",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "zero",
          0
         ],
         "destination": [
          "out1",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "g",
          0
         ],
         "destination": [
          "out2",
          0
         ]
        }
       }
      ]
     },
     "text": "p Oscillator_fields"
    }
   },
   {
    "box": {
     "id": "write0",
     "maxclass": "message",
     "patching_rect": [
      28,
      223,
      45,
      22
     ],
     "text": "write"
    }
   },
   {
    "box": {
     "id": "stop0",
     "maxclass": "message",
     "patching_rect": [
      83,
      223,
      43,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "freeze0",
     "maxclass": "message",
     "patching_rect": [
      153,
      223,
      66,
      22
     ],
     "text": "freeze 1"
    }
   },
   {
    "box": {
     "id": "release0",
     "maxclass": "message",
     "patching_rect": [
      229,
      223,
      66,
      22
     ],
     "text": "freeze 0"
    }
   },
   {
    "box": {
     "id": "pack0",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      278,
      382,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3pack~"
    }
   },
   {
    "box": {
     "id": "dec0",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      352,
      353,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3decode~"
    }
   },
   {
    "box": {
     "id": "status0",
     "maxclass": "newobj",
     "patching_rect": [
      279,
      314,
      117,
      22
     ],
     "text": "print mp3-take"
    }
   },
   {
    "box": {
     "id": "chan0",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      399,
      110,
      22
     ],
     "text": "mc.unpack~ 1"
    }
   },
   {
    "box": {
     "id": "heading1",
     "maxclass": "comment",
     "patching_rect": [
      483,
      120,
      393,
      29
     ],
     "fontsize": 19.0,
     "fontface": 1,
     "text": "Edited encoder bands"
    }
   },
   {
    "box": {
     "id": "src1",
     "maxclass": "newobj",
     "patching_rect": [
      483,
      165,
      385,
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
       800,
       410
      ],
      "default_fontsize": 13.0,
      "default_fontname": "Arial",
      "openinpresentation": 0,
      "boxes": [
       {
        "box": {
         "id": "t",
         "maxclass": "comment",
         "patching_rect": [
          25,
          12,
          745,
          28
         ],
         "fontsize": 17.0,
         "text": "Edit the spectral integers from a live MP3 encoder."
        }
       },
       {
        "box": {
         "id": "s",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          62,
          90,
          22
         ],
         "text": "saw~ 173"
        }
       },
       {
        "box": {
         "id": "lv",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          102,
          80,
          22
         ],
         "text": "*~ 0.18"
        }
       },
       {
        "box": {
         "id": "enc",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          151,
          560,
          22
         ],
         "text": "mp3encode~"
        }
       },
       {
        "box": {
         "id": "mask",
         "maxclass": "newobj",
         "patching_rect": [
          105,
          215,
          605,
          22
         ],
         "text": "mc.sig~ @chans 32 @initialvalues -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1 0 -1 -1"
        }
       },
       {
        "box": {
         "id": "edit",
         "maxclass": "newobj",
         "patching_rect": [
          25,
          272,
          80,
          22
         ],
         "text": "mc.*~"
        }
       },
       {
        "box": {
         "id": "m",
         "maxclass": "comment",
         "patching_rect": [
          105,
          250,
          640,
          22
         ],
         "text": "The mask mutes every third band and reverses the sign of the others."
        }
       },
       {
        "box": {
         "id": "out0",
         "maxclass": "outlet",
         "patching_rect": [
          25,
          340,
          25,
          25
         ],
         "index": 1
        }
       },
       {
        "box": {
         "id": "out1",
         "maxclass": "outlet",
         "patching_rect": [
          160,
          340,
          25,
          25
         ],
         "index": 2
        }
       },
       {
        "box": {
         "id": "out2",
         "maxclass": "outlet",
         "patching_rect": [
          295,
          340,
          25,
          25
         ],
         "index": 3
        }
       },
       {
        "box": {
         "id": "out3",
         "maxclass": "outlet",
         "patching_rect": [
          430,
          340,
          25,
          25
         ],
         "index": 4
        }
       },
       {
        "box": {
         "id": "out4",
         "maxclass": "outlet",
         "patching_rect": [
          565,
          340,
          25,
          25
         ],
         "index": 5
        }
       },
       {
        "box": {
         "id": "downIn",
         "maxclass": "inlet",
         "patching_rect": [
          746,
          25,
          26,
          26
         ],
         "index": 1
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "s",
          0
         ],
         "destination": [
          "lv",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "lv",
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
          "edit",
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
          "edit",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "edit",
          0
         ],
         "destination": [
          "out0",
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
          "out1",
          0
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
          "out2",
          0
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
          "out3",
          0
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
          "out4",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "downIn",
          0
         ],
         "destination": [
          "enc",
          0
         ],
         "hidden": 1
        }
       }
      ]
     },
     "text": "p Edited_encoder_fields"
    }
   },
   {
    "box": {
     "id": "write1",
     "maxclass": "message",
     "patching_rect": [
      483,
      223,
      45,
      22
     ],
     "text": "write"
    }
   },
   {
    "box": {
     "id": "stop1",
     "maxclass": "message",
     "patching_rect": [
      538,
      223,
      43,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "freeze1",
     "maxclass": "message",
     "patching_rect": [
      608,
      223,
      66,
      22
     ],
     "text": "freeze 1"
    }
   },
   {
    "box": {
     "id": "release1",
     "maxclass": "message",
     "patching_rect": [
      684,
      223,
      66,
      22
     ],
     "text": "freeze 0"
    }
   },
   {
    "box": {
     "id": "pack1",
     "maxclass": "newobj",
     "patching_rect": [
      483,
      278,
      382,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3pack~"
    }
   },
   {
    "box": {
     "id": "dec1",
     "maxclass": "newobj",
     "patching_rect": [
      483,
      352,
      353,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3decode~"
    }
   },
   {
    "box": {
     "id": "status1",
     "maxclass": "newobj",
     "patching_rect": [
      734,
      314,
      117,
      22
     ],
     "text": "print mp3-take"
    }
   },
   {
    "box": {
     "id": "chan1",
     "maxclass": "newobj",
     "patching_rect": [
      483,
      399,
      110,
      22
     ],
     "text": "mc.unpack~ 1"
    }
   },
   {
    "box": {
     "id": "listen",
     "maxclass": "comment",
     "patching_rect": [
      27,
      464,
      105,
      23
     ],
     "fontface": 1,
     "text": "Monitor"
    }
   },
   {
    "box": {
     "id": "choose",
     "maxclass": "umenu",
     "patching_rect": [
      27,
      497,
      148,
      23
     ],
     "items": [
      "Oscillators",
      ",",
      "Edited bands"
     ]
    }
   },
   {
    "box": {
     "id": "idx",
     "maxclass": "newobj",
     "patching_rect": [
      183,
      496,
      43,
      22
     ],
     "text": "+ 1"
    }
   },
   {
    "box": {
     "id": "selector",
     "maxclass": "newobj",
     "patching_rect": [
      27,
      535,
      151,
      22
     ],
     "text": "selector~ 2 1"
    }
   },
   {
    "box": {
     "id": "vol",
     "maxclass": "newobj",
     "patching_rect": [
      195,
      535,
      70,
      22
     ],
     "text": "*~ 0.16"
    }
   },
   {
    "box": {
     "id": "clip",
     "maxclass": "newobj",
     "patching_rect": [
      285,
      535,
      108,
      22
     ],
     "text": "clip~ -0.8 0.8"
    }
   },
   {
    "box": {
     "id": "speaker",
     "maxclass": "ezdac~",
     "patching_rect": [
      27,
      589,
      45,
      45
     ]
    }
   },
   {
    "box": {
     "id": "guide",
     "maxclass": "comment",
     "patching_rect": [
      466,
      458,
      415,
      83
     ],
     "linecount": 4,
     "text": "Open Band_holds to hold individual subbands. The freeze messages hold or release the entire frame. The five field outlets drive the monitor decoder."
    }
   },
   {
    "box": {
     "id": "results",
     "maxclass": "comment",
     "patching_rect": [
      466,
      550,
      415,
      82
     ],
     "linecount": 4,
     "text": "On stop, the status outlet reports the path, frames, fitted channel-granules, clamped values, corrected windows, file rate, channels and bitrate."
    }
   },
   {
    "box": {
     "id": "bandMask0",
     "maxclass": "newobj",
     "patching_rect": [
      307,
      223,
      96,
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
       660,
       317
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
          20,
          14,
          575,
          32
         ],
         "fontsize": 22.0,
         "fontface": 1,
         "text": "Band hold"
        }
       },
       {
        "box": {
         "id": "inst",
         "maxclass": "comment",
         "patching_rect": [
          21,
          53,
          610,
          25
         ],
         "text": "Draw along the top to hold bands and along the bottom to release them."
        }
       },
       {
        "box": {
         "id": "sliders",
         "maxclass": "multislider",
         "patching_rect": [
          23,
          90,
          609,
          71
         ],
         "size": 32,
         "setminmax": [
          0.0,
          1.0
         ],
         "settype": 0,
         "setstyle": 1,
         "spacing": 1
        }
       },
       {
        "box": {
         "id": "lo",
         "maxclass": "comment",
         "patching_rect": [
          22,
          165,
          140,
          20
         ],
         "text": "1 • lowest"
        }
       },
       {
        "box": {
         "id": "hi",
         "maxclass": "comment",
         "patching_rect": [
          546,
          165,
          100,
          20
         ],
         "text": "32 • highest"
        }
       },
       {
        "box": {
         "id": "values",
         "maxclass": "newobj",
         "patching_rect": [
          23,
          203,
          145,
          22
         ],
         "text": "prepend applyvalues"
        }
       },
       {
        "box": {
         "id": "mask",
         "maxclass": "newobj",
         "patching_rect": [
          23,
          242,
          167,
          22
         ],
         "text": "mc.sig~ 0 @chans 32"
        }
       },
       {
        "box": {
         "id": "out",
         "maxclass": "outlet",
         "patching_rect": [
          23,
          283,
          24,
          24
         ],
         "index": 1
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "sliders",
          0
         ],
         "destination": [
          "values",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "values",
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
          "mask",
          0
         ],
         "destination": [
          "out",
          0
         ]
        }
       }
      ],
      "title": "Hold individual bands"
     },
     "text": "p Band_holds"
    }
   },
   {
    "box": {
     "id": "bandMask1",
     "maxclass": "newobj",
     "patching_rect": [
      762,
      223,
      96,
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
       660,
       317
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
          20,
          14,
          575,
          32
         ],
         "fontsize": 22.0,
         "fontface": 1,
         "text": "Band hold"
        }
       },
       {
        "box": {
         "id": "inst",
         "maxclass": "comment",
         "patching_rect": [
          21,
          53,
          610,
          25
         ],
         "text": "Draw along the top to hold bands and along the bottom to release them."
        }
       },
       {
        "box": {
         "id": "sliders",
         "maxclass": "multislider",
         "patching_rect": [
          23,
          90,
          609,
          71
         ],
         "size": 32,
         "setminmax": [
          0.0,
          1.0
         ],
         "settype": 0,
         "setstyle": 1,
         "spacing": 1
        }
       },
       {
        "box": {
         "id": "lo",
         "maxclass": "comment",
         "patching_rect": [
          22,
          165,
          140,
          20
         ],
         "text": "1 • lowest"
        }
       },
       {
        "box": {
         "id": "hi",
         "maxclass": "comment",
         "patching_rect": [
          546,
          165,
          100,
          20
         ],
         "text": "32 • highest"
        }
       },
       {
        "box": {
         "id": "values",
         "maxclass": "newobj",
         "patching_rect": [
          23,
          203,
          145,
          22
         ],
         "text": "prepend applyvalues"
        }
       },
       {
        "box": {
         "id": "mask",
         "maxclass": "newobj",
         "patching_rect": [
          23,
          242,
          167,
          22
         ],
         "text": "mc.sig~ 0 @chans 32"
        }
       },
       {
        "box": {
         "id": "out",
         "maxclass": "outlet",
         "patching_rect": [
          23,
          283,
          24,
          24
         ],
         "index": 1
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "sliders",
          0
         ],
         "destination": [
          "values",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "values",
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
          "mask",
          0
         ],
         "destination": [
          "out",
          0
         ]
        }
       }
      ],
      "title": "Hold individual bands"
     },
     "text": "p Band_holds"
    }
   },
   {
    "box": {
     "id": "newfeatures",
     "maxclass": "message",
     "patching_rect": [
      24,
      658,
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
      658,
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
      658,
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
      689,
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
      689,
      485,
      28
     ],
     "text": "Changes both recorders. Adjust it during the take."
    }
   },
   {
    "box": {
     "id": "kbnumber",
     "maxclass": "number",
     "patching_rect": [
      192,
      689,
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
      689,
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
      724,
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
      724,
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
      724,
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
      755,
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
      724,
      520,
      31
     ],
     "text": "Set before write. Changing the file rate finishes both active takes."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "write0",
      0
     ],
     "destination": [
      "pack0",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "stop0",
      0
     ],
     "destination": [
      "pack0",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "freeze0",
      0
     ],
     "destination": [
      "pack0",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "release0",
      0
     ],
     "destination": [
      "pack0",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "src0",
      0
     ],
     "destination": [
      "pack0",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src0",
      1
     ],
     "destination": [
      "pack0",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src0",
      2
     ],
     "destination": [
      "pack0",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack0",
      0
     ],
     "destination": [
      "dec0",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack0",
      1
     ],
     "destination": [
      "dec0",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack0",
      2
     ],
     "destination": [
      "dec0",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack0",
      3
     ],
     "destination": [
      "dec0",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack0",
      4
     ],
     "destination": [
      "dec0",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack0",
      5
     ],
     "destination": [
      "status0",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "dec0",
      0
     ],
     "destination": [
      "chan0",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "write1",
      0
     ],
     "destination": [
      "pack1",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "stop1",
      0
     ],
     "destination": [
      "pack1",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "freeze1",
      0
     ],
     "destination": [
      "pack1",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "release1",
      0
     ],
     "destination": [
      "pack1",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "src1",
      0
     ],
     "destination": [
      "pack1",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src1",
      1
     ],
     "destination": [
      "pack1",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src1",
      2
     ],
     "destination": [
      "pack1",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src1",
      3
     ],
     "destination": [
      "pack1",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src1",
      4
     ],
     "destination": [
      "pack1",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack1",
      0
     ],
     "destination": [
      "dec1",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack1",
      1
     ],
     "destination": [
      "dec1",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack1",
      2
     ],
     "destination": [
      "dec1",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack1",
      3
     ],
     "destination": [
      "dec1",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack1",
      4
     ],
     "destination": [
      "dec1",
      4
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pack1",
      5
     ],
     "destination": [
      "status1",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "dec1",
      0
     ],
     "destination": [
      "chan1",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "choose",
      0
     ],
     "destination": [
      "idx",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "idx",
      0
     ],
     "destination": [
      "selector",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "chan0",
      0
     ],
     "destination": [
      "selector",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "chan1",
      0
     ],
     "destination": [
      "selector",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "selector",
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
      "speaker",
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
      "speaker",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bandMask0",
      0
     ],
     "destination": [
      "pack0",
      5
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bandMask1",
      0
     ],
     "destination": [
      "pack1",
      5
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
      "pack0",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "kbmsg",
      0
     ],
     "destination": [
      "pack1",
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
      "src1",
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
      "pack0",
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
      "pack1",
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
      "dec0",
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
      "dec1",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "MP3 field recorder"
 }
}
