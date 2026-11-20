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
   930,
   771
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
      23,
      14,
      800,
      38
     ],
     "fontsize": 27.0,
     "fontface": 1,
     "text": "Play the MP3 decoder"
    }
   },
   {
    "box": {
     "id": "intro",
     "maxclass": "comment",
     "patching_rect": [
      25,
      59,
      850,
      30
     ],
     "fontsize": 15.0,
     "text": "Generate Layer III coefficients with oscillators. Each of the 32 MC lanes addresses one subband."
    }
   },
   {
    "box": {
     "id": "freq",
     "maxclass": "comment",
     "patching_rect": [
      26,
      107,
      244,
      22
     ],
     "fontface": 1,
     "text": "Coefficient oscillators"
    }
   },
   {
    "box": {
     "id": "spread",
     "maxclass": "message",
     "patching_rect": [
      26,
      143,
      113,
      22
     ],
     "text": "spread 13 227"
    }
   },
   {
    "box": {
     "id": "osc",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      185,
      169,
      22
     ],
     "text": "mc.cycle~ 73 @chans 32"
    }
   },
   {
    "box": {
     "id": "mag",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      232,
      89,
      22
     ],
     "text": "mc.*~ 8."
    }
   },
   {
    "box": {
     "id": "masklabel",
     "maxclass": "comment",
     "patching_rect": [
      258,
      108,
      164,
      22
     ],
     "fontface": 1,
     "text": "Subband pattern"
    }
   },
   {
    "box": {
     "id": "sparse",
     "maxclass": "message",
     "patching_rect": [
      258,
      143,
      204,
      22
     ],
     "text": "replicatevalues 1 0 0 0 0 0 0 0"
    }
   },
   {
    "box": {
     "id": "all",
     "maxclass": "message",
     "patching_rect": [
      475,
      143,
      30,
      22
     ],
     "text": "1."
    }
   },
   {
    "box": {
     "id": "mask",
     "maxclass": "newobj",
     "patching_rect": [
      258,
      185,
      250,
      22
     ],
     "text": "mc.sig~ 0 @chans 32 @initialvalues 1"
    }
   },
   {
    "box": {
     "id": "multiply",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      282,
      191,
      22
     ],
     "text": "mc.*~"
    }
   },
   {
    "box": {
     "id": "gainlabel",
     "maxclass": "comment",
     "patching_rect": [
      556,
      108,
      144,
      22
     ],
     "fontface": 1,
     "text": "Global gain"
    }
   },
   {
    "box": {
     "id": "num",
     "maxclass": "number",
     "patching_rect": [
      556,
      143,
      65,
      23
     ],
     "minimum": 0,
     "maximum": 190
    }
   },
   {
    "box": {
     "id": "init",
     "maxclass": "newobj",
     "patching_rect": [
      552,
      110,
      68,
      22
     ],
     "hidden": 1,
     "text": "loadmess 167"
    }
   },
   {
    "box": {
     "id": "gg",
     "maxclass": "newobj",
     "patching_rect": [
      556,
      185,
      79,
      22
     ],
     "text": "sig~ 167"
    }
   },
   {
    "box": {
     "id": "typelabel",
     "maxclass": "comment",
     "patching_rect": [
      714,
      108,
      165,
      22
     ],
     "fontface": 1,
     "text": "Window"
    }
   },
   {
    "box": {
     "id": "menu",
     "maxclass": "umenu",
     "patching_rect": [
      714,
      143,
      126,
      23
     ],
     "items": [
      "Long",
      ",",
      "Start",
      ",",
      "Short",
      ",",
      "Stop",
      ",",
      "Mixed"
     ]
    }
   },
   {
    "box": {
     "id": "window",
     "maxclass": "newobj",
     "patching_rect": [
      714,
      185,
      64,
      22
     ],
     "text": "sig~ 0"
    }
   },
   {
    "box": {
     "id": "dec",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      340,
      768,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3decode~"
    }
   },
   {
    "box": {
     "id": "st",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      400,
      91,
      22
     ],
     "text": "mc.dup~ 2"
    }
   },
   {
    "box": {
     "id": "volume",
     "maxclass": "newobj",
     "patching_rect": [
      26,
      442,
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
      26,
      481,
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
      26,
      520,
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
      26,
      562,
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
      76,
      561,
      60,
      22
     ],
     "hidden": 1,
     "text": "loadmess local 1"
    }
   },
   {
    "box": {
     "id": "notes",
     "maxclass": "comment",
     "patching_rect": [
      267,
      413,
      593,
      101
     ],
     "linecount": 5,
     "text": "Click the speaker to hear the lowest subband. Add the repeating subband pattern, spread the oscillators, then switch the window between Long and Short. The same coefficient data goes through a different synthesis window."
    }
   },
   {
    "box": {
     "id": "clock",
     "maxclass": "comment",
     "patching_rect": [
      267,
      536,
      590,
      63
     ],
     "linecount": 3,
     "text": "Each lane supplies 18 integer values per granule, one every 32 codec samples. The decoder runs on its own clock when the sync inlet is empty."
    }
   },
   {
    "box": {
     "id": "freezeOn",
     "maxclass": "message",
     "patching_rect": [
      380,
      281,
      69,
      22
     ],
     "text": "freeze 1"
    }
   },
   {
    "box": {
     "id": "freezeOff",
     "maxclass": "message",
     "patching_rect": [
      463,
      281,
      69,
      22
     ],
     "text": "freeze 0"
    }
   },
   {
    "box": {
     "id": "freezeHelp",
     "maxclass": "comment",
     "patching_rect": [
      370,
      247,
      420,
      22
     ],
     "text": "Freeze the whole frame or open Band_holds for individual bands."
    }
   },
   {
    "box": {
     "id": "bandMask",
     "maxclass": "newobj",
     "patching_rect": [
      678,
      282,
      107,
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
      660,
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
      660,
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
      660,
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
      691,
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
      691,
      485,
      28
     ],
     "text": "Audition the fitted fields. 0 returns to direct synthesis."
    }
   },
   {
    "box": {
     "id": "kbnumber",
     "maxclass": "number",
     "patching_rect": [
      192,
      691,
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
      691,
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
      726,
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
      726,
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
      726,
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
      757,
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
      726,
      505,
      31
     ],
     "text": "2 gives half rate. The 32 synthesis bands span half the bandwidth."
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
      "mag",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "sparse",
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
      "all",
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
      "mag",
      0
     ],
     "destination": [
      "multiply",
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
      "multiply",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "init",
      0
     ],
     "destination": [
      "num",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "num",
      0
     ],
     "destination": [
      "gg",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "menu",
      0
     ],
     "destination": [
      "window",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "multiply",
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
      "gg",
      0
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
      "window",
      0
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
      "dec",
      0
     ],
     "destination": [
      "st",
      0
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
      "st",
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
      "freezeOn",
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
      "freezeOff",
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
      "bandMask",
      0
     ],
     "destination": [
      "dec",
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
      "dec",
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
      "dec",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "MP3 decoder as an instrument"
 }
}
