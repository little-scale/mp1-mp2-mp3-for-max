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
   570
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
      14,
      390,
      37
     ],
     "fontsize": 27.0,
     "fontface": 1,
     "text": "mp1decode~"
    }
   },
   {
    "box": {
     "id": "intro",
     "maxclass": "comment",
     "patching_rect": [
      24,
      55,
      840,
      24
     ],
     "fontsize": 15.0,
     "text": "Drive the MPEG Layer I synthesis filter bank with MSP signals."
    }
   },
   {
    "box": {
     "id": "pitchlabel",
     "maxclass": "comment",
     "patching_rect": [
      28,
      103,
      93,
      20
     ],
     "fontface": 1,
     "text": "Source Hz"
    }
   },
   {
    "box": {
     "id": "pitch",
     "maxclass": "flonum",
     "patching_rect": [
      28,
      130,
      69,
      23
     ],
     "minimum": 0.0,
     "maximum": 20000.0
    }
   },
   {
    "box": {
     "id": "pitchload",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      100,
      69,
      22
     ],
     "hidden": 1,
     "text": "loadmess 73.0"
    }
   },
   {
    "box": {
     "id": "ml",
     "maxclass": "comment",
     "patching_rect": [
      142,
      103,
      85,
      20
     ],
     "fontface": 1,
     "text": "Routing"
    }
   },
   {
    "box": {
     "id": "mode",
     "maxclass": "umenu",
     "patching_rect": [
      142,
      130,
      104,
      23
     ],
     "items": [
      "Hold",
      ",",
      "Sweep",
      ",",
      "Raster"
     ]
    }
   },
   {
    "box": {
     "id": "modeinit",
     "maxclass": "newobj",
     "patching_rect": [
      142,
      97,
      85,
      22
     ],
     "hidden": 1,
     "text": "loadmess 0"
    }
   },
   {
    "box": {
     "id": "bandlabel",
     "maxclass": "comment",
     "patching_rect": [
      280,
      103,
      99,
      20
     ],
     "fontface": 1,
     "text": "Hold band"
    }
   },
   {
    "box": {
     "id": "band",
     "maxclass": "number",
     "patching_rect": [
      280,
      130,
      75,
      23
     ],
     "minimum": 0,
     "maximum": 31
    }
   },
   {
    "box": {
     "id": "bandload",
     "maxclass": "newobj",
     "patching_rect": [
      280,
      100,
      75,
      22
     ],
     "hidden": 1,
     "text": "loadmess 0"
    }
   },
   {
    "box": {
     "id": "driftlabel",
     "maxclass": "comment",
     "patching_rect": [
      403,
      103,
      107,
      20
     ],
     "fontface": 1,
     "text": "Raster drift Hz"
    }
   },
   {
    "box": {
     "id": "drift",
     "maxclass": "flonum",
     "patching_rect": [
      403,
      130,
      83,
      23
     ],
     "minimum": -500.0,
     "maximum": 500.0
    }
   },
   {
    "box": {
     "id": "driftload",
     "maxclass": "newobj",
     "patching_rect": [
      403,
      100,
      83,
      22
     ],
     "hidden": 1,
     "text": "loadmess 1.0"
    }
   },
   {
    "box": {
     "id": "bitslabel",
     "maxclass": "comment",
     "patching_rect": [
      552,
      103,
      96,
      20
     ],
     "fontface": 1,
     "text": "Allocation"
    }
   },
   {
    "box": {
     "id": "bits",
     "maxclass": "number",
     "patching_rect": [
      552,
      130,
      72,
      23
     ],
     "minimum": 0,
     "maximum": 14
    }
   },
   {
    "box": {
     "id": "bitsload",
     "maxclass": "newobj",
     "patching_rect": [
      552,
      100,
      72,
      22
     ],
     "hidden": 1,
     "text": "loadmess 7"
    }
   },
   {
    "box": {
     "id": "scalelabel",
     "maxclass": "comment",
     "patching_rect": [
      711,
      103,
      102,
      20
     ],
     "fontface": 1,
     "text": "Scale index"
    }
   },
   {
    "box": {
     "id": "scale",
     "maxclass": "number",
     "patching_rect": [
      711,
      130,
      78,
      23
     ],
     "minimum": 0,
     "maximum": 62
    }
   },
   {
    "box": {
     "id": "scaleload",
     "maxclass": "newobj",
     "patching_rect": [
      711,
      100,
      78,
      22
     ],
     "hidden": 1,
     "text": "loadmess 11"
    }
   },
   {
    "box": {
     "id": "osc",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      186,
      83,
      22
     ],
     "text": "cycle~ 73"
    }
   },
   {
    "box": {
     "id": "mask",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      237,
      111,
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
       530,
       410
      ],
      "default_fontsize": 13.0,
      "default_fontname": "Arial",
      "openinpresentation": 0,
      "boxes": [
       {
        "box": {
         "id": "data",
         "maxclass": "inlet",
         "patching_rect": [
          35,
          28,
          25,
          25
         ],
         "index": 1
        }
       },
       {
        "box": {
         "id": "mode",
         "maxclass": "inlet",
         "patching_rect": [
          155,
          28,
          25,
          25
         ],
         "index": 2
        }
       },
       {
        "box": {
         "id": "drift",
         "maxclass": "inlet",
         "patching_rect": [
          420,
          28,
          25,
          25
         ],
         "index": 3
        }
       },
       {
        "box": {
         "id": "is",
         "maxclass": "newobj",
         "patching_rect": [
          155,
          75,
          52,
          22
         ],
         "text": "== 2"
        }
       },
       {
        "box": {
         "id": "which",
         "maxclass": "newobj",
         "patching_rect": [
          155,
          115,
          45,
          22
         ],
         "text": "+ 1"
        }
       },
       {
        "box": {
         "id": "state",
         "maxclass": "newobj",
         "patching_rect": [
          270,
          40,
          76,
          22
         ],
         "text": "dspstate~"
        }
       },
       {
        "box": {
         "id": "div",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          75,
          58,
          22
         ],
         "text": "/ 32."
        }
       },
       {
        "box": {
         "id": "row",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          115,
          45,
          22
         ],
         "text": "sig~"
        }
       },
       {
        "box": {
         "id": "add",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          155,
          80,
          22
         ],
         "text": "+~ 1."
        }
       },
       {
        "box": {
         "id": "osc",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          190,
          60,
          22
         ],
         "text": "cycle~"
        }
       },
       {
        "box": {
         "id": "bias",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          225,
          60,
          22
         ],
         "text": "+~ 1."
        }
       },
       {
        "box": {
         "id": "half",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          260,
          60,
          22
         ],
         "text": "*~ 0.5"
        }
       },
       {
        "box": {
         "id": "square",
         "maxclass": "newobj",
         "patching_rect": [
          275,
          295,
          60,
          22
         ],
         "text": "*~"
        }
       },
       {
        "box": {
         "id": "am",
         "maxclass": "newobj",
         "patching_rect": [
          65,
          325,
          80,
          22
         ],
         "text": "*~"
        }
       },
       {
        "box": {
         "id": "sel",
         "maxclass": "newobj",
         "patching_rect": [
          155,
          360,
          95,
          22
         ],
         "text": "selector~ 2 1"
        }
       },
       {
        "box": {
         "id": "out",
         "maxclass": "outlet",
         "patching_rect": [
          155,
          390,
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
          "state",
          1
         ],
         "destination": [
          "div",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "mode",
          0
         ],
         "destination": [
          "is",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "is",
          0
         ],
         "destination": [
          "which",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "which",
          0
         ],
         "destination": [
          "sel",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "data",
          0
         ],
         "destination": [
          "sel",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "data",
          0
         ],
         "destination": [
          "am",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "drift",
          0
         ],
         "destination": [
          "add",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "div",
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
          "row",
          0
         ],
         "destination": [
          "add",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "add",
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
          "bias",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "bias",
          0
         ],
         "destination": [
          "half",
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
          "square",
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
          "square",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "square",
          0
         ],
         "destination": [
          "am",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "am",
          0
         ],
         "destination": [
          "sel",
          2
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "sel",
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
     "text": "p raster-mask"
    }
   },
   {
    "box": {
     "id": "addr",
     "maxclass": "newobj",
     "patching_rect": [
      270,
      237,
      116,
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
       470,
       260
      ],
      "default_fontsize": 13.0,
      "default_fontname": "Arial",
      "openinpresentation": 0,
      "boxes": [
       {
        "box": {
         "id": "mode",
         "maxclass": "inlet",
         "patching_rect": [
          30,
          28,
          25,
          25
         ],
         "index": 1
        }
       },
       {
        "box": {
         "id": "band",
         "maxclass": "inlet",
         "patching_rect": [
          145,
          28,
          25,
          25
         ],
         "index": 2
        }
       },
       {
        "box": {
         "id": "plus",
         "maxclass": "newobj",
         "patching_rect": [
          30,
          90,
          40,
          22
         ],
         "text": "+ 1"
        }
       },
       {
        "box": {
         "id": "park",
         "maxclass": "newobj",
         "patching_rect": [
          145,
          90,
          55,
          22
         ],
         "text": "sig~ 0"
        }
       },
       {
        "box": {
         "id": "sweep",
         "maxclass": "newobj",
         "patching_rect": [
          230,
          45,
          100,
          22
         ],
         "text": "phasor~ 0.35"
        }
       },
       {
        "box": {
         "id": "width",
         "maxclass": "newobj",
         "patching_rect": [
          230,
          90,
          55,
          22
         ],
         "text": "*~ 8."
        }
       },
       {
        "box": {
         "id": "auto",
         "maxclass": "newobj",
         "patching_rect": [
          350,
          90,
          60,
          22
         ],
         "text": "sig~ -1"
        }
       },
       {
        "box": {
         "id": "sel",
         "maxclass": "newobj",
         "patching_rect": [
          30,
          155,
          330,
          22
         ],
         "text": "selector~ 3 1"
        }
       },
       {
        "box": {
         "id": "out",
         "maxclass": "outlet",
         "patching_rect": [
          30,
          208,
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
          "mode",
          0
         ],
         "destination": [
          "plus",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "plus",
          0
         ],
         "destination": [
          "sel",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "band",
          0
         ],
         "destination": [
          "park",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "park",
          0
         ],
         "destination": [
          "sel",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "sweep",
          0
         ],
         "destination": [
          "width",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "width",
          0
         ],
         "destination": [
          "sel",
          2
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "auto",
          0
         ],
         "destination": [
          "sel",
          3
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "sel",
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
     "text": "p band-address"
    }
   },
   {
    "box": {
     "id": "bitsig",
     "maxclass": "newobj",
     "patching_rect": [
      552,
      237,
      60,
      22
     ],
     "text": "sig~ 7"
    }
   },
   {
    "box": {
     "id": "scsig",
     "maxclass": "newobj",
     "patching_rect": [
      711,
      237,
      64,
      22
     ],
     "text": "sig~ 11"
    }
   },
   {
    "box": {
     "id": "decoder",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      291,
      690,
      22
     ],
     "fontsize": 16.0,
     "color": [
      0.32,
      0.64,
      0.72,
      1.0
     ],
     "text": "mp1decode~"
    }
   },
   {
    "box": {
     "id": "atten",
     "maxclass": "newobj",
     "patching_rect": [
      28,
      346,
      68,
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
      28,
      399,
      50,
      50
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "play",
     "maxclass": "comment",
     "patching_rect": [
      20,
      462,
      199,
      35
     ],
     "text": "Click the speaker to listen."
    }
   },
   {
    "box": {
     "id": "spectrum",
     "maxclass": "spectroscope~",
     "patching_rect": [
      142,
      346,
      305,
      108
     ]
    }
   },
   {
    "box": {
     "id": "desc",
     "maxclass": "comment",
     "patching_rect": [
      468,
      342,
      430,
      80
     ],
     "linecount": 4,
     "text": "Hold parks the source in one band. Sweep moves it through bands 0\u20137. Raster writes across all 32 bands. Set drift to 0 to lock its gain pattern, then try 0.5 or \u22121 Hz."
    }
   },
   {
    "box": {
     "id": "clocks",
     "maxclass": "comment",
     "patching_rect": [
      468,
      428,
      430,
      72
     ],
     "linecount": 4,
     "text": "The middle outlet counts 0\u201331 every scan. The right outlet pulses once per 384 samples. Allocation and scale are captured on the first scan of each frame."
    }
   },
   {
    "box": {
     "id": "foot",
     "maxclass": "comment",
     "patching_rect": [
      22,
      513,
      880,
      39
     ],
     "text": "Use [mp1decode~ raw] to drive the first inlet with integer codewords. Disconnect the address inlet for automatic scanning."
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "pitchload",
      0
     ],
     "destination": [
      "pitch",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "modeinit",
      0
     ],
     "destination": [
      "mode",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "bandload",
      0
     ],
     "destination": [
      "band",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "driftload",
      0
     ],
     "destination": [
      "drift",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "bitsload",
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
      "scaleload",
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
      "pitch",
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
      "mask",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mode",
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
      "drift",
      0
     ],
     "destination": [
      "mask",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "mode",
      0
     ],
     "destination": [
      "addr",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "band",
      0
     ],
     "destination": [
      "addr",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "bits",
      0
     ],
     "destination": [
      "bitsig",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "scale",
      0
     ],
     "destination": [
      "scsig",
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
      "decoder",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "addr",
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
      "bitsig",
      0
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
      "scsig",
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
      "decoder",
      0
     ],
     "destination": [
      "atten",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "atten",
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
      "atten",
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
      "atten",
      0
     ],
     "destination": [
      "spectrum",
      0
     ]
    }
   }
  ],
  "title": "MP1 decoder"
 }
}
