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
   883,
   616
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
      835,
      38
     ],
     "fontsize": 26.0,
     "fontface": 1,
     "text": "mp3short~"
    }
   },
   {
    "box": {
     "id": "sub",
     "maxclass": "comment",
     "patching_rect": [
      24,
      54,
      835,
      34
     ],
     "text": "Set a different level for each of the three short windows. The trims are in dB."
    }
   },
   {
    "box": {
     "id": "bank",
     "maxclass": "newobj",
     "patching_rect": [
      23,
      112,
      110,
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
     "id": "clk",
     "maxclass": "newobj",
     "patching_rect": [
      167,
      112,
      97,
      22
     ],
     "text": "mp3clock~"
    }
   },
   {
    "box": {
     "id": "window",
     "maxclass": "newobj",
     "patching_rect": [
      288,
      153,
      68,
      22
     ],
     "text": "sig~ 2"
    }
   },
   {
    "box": {
     "id": "w0",
     "maxclass": "message",
     "patching_rect": [
      290,
      111,
      38,
      22
     ],
     "text": "2"
    }
   },
   {
    "box": {
     "id": "w0l",
     "maxclass": "comment",
     "patching_rect": [
      288,
      84,
      65,
      22
     ],
     "text": "Short"
    }
   },
   {
    "box": {
     "id": "w1",
     "maxclass": "message",
     "patching_rect": [
      366,
      111,
      38,
      22
     ],
     "text": "4"
    }
   },
   {
    "box": {
     "id": "w1l",
     "maxclass": "comment",
     "patching_rect": [
      364,
      84,
      65,
      22
     ],
     "text": "Mixed"
    }
   },
   {
    "box": {
     "id": "w2",
     "maxclass": "message",
     "patching_rect": [
      442,
      111,
      38,
      22
     ],
     "text": "0"
    }
   },
   {
    "box": {
     "id": "w2l",
     "maxclass": "comment",
     "patching_rect": [
      440,
      84,
      65,
      22
     ],
     "text": "Long"
    }
   },
   {
    "box": {
     "id": "fall",
     "maxclass": "message",
     "patching_rect": [
      515,
      98,
      110,
      22
     ],
     "text": "0 -12 -24"
    }
   },
   {
    "box": {
     "id": "rise",
     "maxclass": "message",
     "patching_rect": [
      693,
      98,
      110,
      22
     ],
     "text": "-24 -12 0"
    }
   },
   {
    "box": {
     "id": "split",
     "maxclass": "newobj",
     "patching_rect": [
      525,
      126,
      250,
      22
     ],
     "text": "unpack f f f"
    }
   },
   {
    "box": {
     "id": "trim0label",
     "maxclass": "comment",
     "patching_rect": [
      522,
      151,
      100,
      22
     ],
     "text": "Early dB"
    }
   },
   {
    "box": {
     "id": "trim0",
     "maxclass": "flonum",
     "patching_rect": [
      522,
      173,
      76,
      22
     ],
     "minimum": -120.0,
     "maximum": 48.0
    }
   },
   {
    "box": {
     "id": "trim0sig",
     "maxclass": "newobj",
     "patching_rect": [
      522,
      202,
      63,
      22
     ],
     "text": "sig~"
    }
   },
   {
    "box": {
     "id": "trim1label",
     "maxclass": "comment",
     "patching_rect": [
      633,
      151,
      100,
      22
     ],
     "text": "Middle dB"
    }
   },
   {
    "box": {
     "id": "trim1",
     "maxclass": "flonum",
     "patching_rect": [
      633,
      173,
      76,
      22
     ],
     "minimum": -120.0,
     "maximum": 48.0
    }
   },
   {
    "box": {
     "id": "trim1sig",
     "maxclass": "newobj",
     "patching_rect": [
      633,
      202,
      63,
      22
     ],
     "text": "sig~"
    }
   },
   {
    "box": {
     "id": "trim2label",
     "maxclass": "comment",
     "patching_rect": [
      744,
      151,
      100,
      22
     ],
     "text": "Late dB"
    }
   },
   {
    "box": {
     "id": "trim2",
     "maxclass": "flonum",
     "patching_rect": [
      744,
      173,
      76,
      22
     ],
     "minimum": -120.0,
     "maximum": 48.0
    }
   },
   {
    "box": {
     "id": "trim2sig",
     "maxclass": "newobj",
     "patching_rect": [
      744,
      202,
      63,
      22
     ],
     "text": "sig~"
    }
   },
   {
    "box": {
     "id": "trims",
     "maxclass": "newobj",
     "patching_rect": [
      690,
      237,
      153,
      22
     ],
     "text": "mc.pack~ 3"
    }
   },
   {
    "box": {
     "id": "base",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      200,
      66,
      22
     ],
     "text": "sig~ 0"
    }
   },
   {
    "box": {
     "id": "short",
     "maxclass": "newobj",
     "patching_rect": [
      165,
      245,
      490,
      22
     ],
     "fontsize": 15.0,
     "color": [
      0.3,
      0.55,
      0.47,
      1.0
     ],
     "text": "mp3short~"
    }
   },
   {
    "box": {
     "id": "decoder",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      312,
      540,
      22
     ],
     "text": "mp3decode~"
    }
   },
   {
    "box": {
     "id": "level",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      362,
      107,
      22
     ],
     "text": "mc.*~ 0.12"
    }
   },
   {
    "box": {
     "id": "clip",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      404,
      156,
      22
     ],
     "text": "mc.clip~ -0.75 0.75"
    }
   },
   {
    "box": {
     "id": "unp",
     "maxclass": "newobj",
     "patching_rect": [
      24,
      447,
      121,
      22
     ],
     "text": "mc.unpack~ 1"
    }
   },
   {
    "box": {
     "id": "dac",
     "maxclass": "ezdac~",
     "patching_rect": [
      24,
      490,
      45,
      45
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "guide",
     "maxclass": "comment",
     "patching_rect": [
      266,
      354,
      505,
      100
     ],
     "text": "Click the speaker, edit the three dB boxes, or choose a trim pattern. In Mixed, the lowest two subbands keep their long windows. In Long, all bands pass their incoming attenuation."
    }
   },
   {
    "box": {
     "id": "guide2",
     "maxclass": "comment",
     "patching_rect": [
      266,
      460,
      514,
      80
     ],
     "text": "For independent stereo use six MC lanes, left trio then right trio. Connect the adjusted attenuation to inlet 2 of mp3pack~ to write these edits into a take."
    }
   },
   {
    "box": {
     "id": "dstitle",
     "maxclass": "comment",
     "patching_rect": [
      24,
      547,
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
      547,
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
      547,
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
      578,
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
      547,
      458,
      31
     ],
     "text": "The clock and decoder share the divisor. The short-window trims follow the row."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "w0",
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
      "w1",
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
      "w2",
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
      "fall",
      0
     ],
     "destination": [
      "split",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "rise",
      0
     ],
     "destination": [
      "split",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "split",
      0
     ],
     "destination": [
      "trim0",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim0",
      0
     ],
     "destination": [
      "trim0sig",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim0sig",
      0
     ],
     "destination": [
      "trims",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "split",
      1
     ],
     "destination": [
      "trim1",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim1",
      0
     ],
     "destination": [
      "trim1sig",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim1sig",
      0
     ],
     "destination": [
      "trims",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "split",
      2
     ],
     "destination": [
      "trim2",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim2",
      0
     ],
     "destination": [
      "trim2sig",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trim2sig",
      0
     ],
     "destination": [
      "trims",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "base",
      0
     ],
     "destination": [
      "short",
      0
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
      "short",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "clk",
      0
     ],
     "destination": [
      "short",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "trims",
      0
     ],
     "destination": [
      "short",
      3
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
      "decoder",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "short",
      0
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
      "window",
      0
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
      "clk",
      2
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
      "decoder",
      0
     ],
     "destination": [
      "level",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "level",
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
      "unp",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "unp",
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
      "unp",
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
      "decoder",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "MP3 short-window trims"
 }
}
