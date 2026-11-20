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
   988,
   783
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
      10,
      279,
      38
     ],
     "fontsize": 28.0,
     "fontface": 1,
     "text": "mp3unpack~"
    }
   },
   {
    "box": {
     "id": "sub",
     "maxclass": "comment",
     "patching_rect": [
      24,
      56,
      915,
      25
     ],
     "fontsize": 16.0,
     "text": "Read an MP3 into editable MC bands and write the result directly to another MP3."
    }
   },
   {
    "box": {
     "id": "guide",
     "maxclass": "comment",
     "patching_rect": [
      24,
      89,
      930,
      38
     ],
     "linecount": 2,
     "text": "Turn on DSP. Choose a file with read (or click the example), click write to choose a destination, then click start. The take closes at the end of the file."
    }
   },
   {
    "box": {
     "id": "read",
     "maxclass": "message",
     "patching_rect": [
      29,
      147,
      43,
      22
     ],
     "text": "read"
    }
   },
   {
    "box": {
     "id": "example",
     "maxclass": "message",
     "patching_rect": [
      83,
      147,
      221,
      22
     ],
     "text": "read \"Individual band holds.mp3\""
    }
   },
   {
    "box": {
     "id": "start",
     "maxclass": "message",
     "patching_rect": [
      317,
      147,
      48,
      22
     ],
     "text": "start"
    }
   },
   {
    "box": {
     "id": "stop",
     "maxclass": "message",
     "patching_rect": [
      375,
      147,
      45,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "src",
     "maxclass": "newobj",
     "patching_rect": [
      29,
      197,
      451,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3unpack~"
    }
   },
   {
    "box": {
     "id": "status",
     "maxclass": "newobj",
     "patching_rect": [
      500,
      197,
      289,
      22
     ],
     "text": "route sourceformat done ready error"
    }
   },
   {
    "box": {
     "id": "fmt",
     "maxclass": "newobj",
     "patching_rect": [
      500,
      241,
      151,
      22
     ],
     "text": "prepend sourceformat"
    }
   },
   {
    "box": {
     "id": "finish",
     "maxclass": "message",
     "patching_rect": [
      669,
      241,
      44,
      22
     ],
     "text": "stop"
    }
   },
   {
    "box": {
     "id": "sourceLog",
     "maxclass": "newobj",
     "patching_rect": [
      821,
      241,
      105,
      22
     ],
     "text": "print mp3-file"
    }
   },
   {
    "box": {
     "id": "t1",
     "maxclass": "comment",
     "patching_rect": [
      25,
      251,
      285,
      22
     ],
     "fontface": 1,
     "text": "Edit the lowest subband on both sides"
    }
   },
   {
    "box": {
     "id": "kill",
     "maxclass": "message",
     "patching_rect": [
      26,
      280,
      184,
      22
     ],
     "text": "setvalue 1 0, setvalue 33 0"
    }
   },
   {
    "box": {
     "id": "restore",
     "maxclass": "message",
     "patching_rect": [
      219,
      280,
      184,
      22
     ],
     "text": "setvalue 1 1, setvalue 33 1"
    }
   },
   {
    "box": {
     "id": "qgain",
     "maxclass": "newobj",
     "patching_rect": [
      119,
      319,
      153,
      22
     ],
     "text": "mc.sig~ 1. @chans 64"
    }
   },
   {
    "box": {
     "id": "mc",
     "maxclass": "newobj",
     "patching_rect": [
      29,
      359,
      106,
      22
     ],
     "text": "mc.*~ 1."
    }
   },
   {
    "box": {
     "id": "t2",
     "maxclass": "comment",
     "patching_rect": [
      504,
      280,
      342,
      22
     ],
     "fontface": 1,
     "text": "Hold the third subband on both sides"
    }
   },
   {
    "box": {
     "id": "hold",
     "maxclass": "message",
     "patching_rect": [
      505,
      311,
      92,
      22
     ],
     "text": "setvalue 3 1"
    }
   },
   {
    "box": {
     "id": "release",
     "maxclass": "message",
     "patching_rect": [
      609,
      311,
      92,
      22
     ],
     "text": "setvalue 3 0"
    }
   },
   {
    "box": {
     "id": "mask",
     "maxclass": "newobj",
     "patching_rect": [
      505,
      352,
      153,
      22
     ],
     "text": "mc.sig~ 0 @chans 32"
    }
   },
   {
    "box": {
     "id": "level",
     "maxclass": "message",
     "patching_rect": [
      717,
      311,
      110,
      22
     ],
     "text": "holdmode level"
    }
   },
   {
    "box": {
     "id": "coeff",
     "maxclass": "message",
     "patching_rect": [
      840,
      311,
      114,
      22
     ],
     "text": "holdmode coeff"
    }
   },
   {
    "box": {
     "id": "hm",
     "maxclass": "comment",
     "patching_rect": [
      717,
      345,
      236,
      54
     ],
     "linecount": 3,
     "text": "level holds the captured level. coeff follows the live global gain."
    }
   },
   {
    "box": {
     "id": "write",
     "maxclass": "message",
     "patching_rect": [
      29,
      405,
      48,
      22
     ],
     "text": "write"
    }
   },
   {
    "box": {
     "id": "whole",
     "maxclass": "message",
     "patching_rect": [
      91,
      405,
      68,
      22
     ],
     "text": "freeze 1"
    }
   },
   {
    "box": {
     "id": "thaw",
     "maxclass": "message",
     "patching_rect": [
      169,
      405,
      67,
      22
     ],
     "text": "freeze 0"
    }
   },
   {
    "box": {
     "id": "pk",
     "maxclass": "newobj",
     "patching_rect": [
      29,
      455,
      558,
      22
     ],
     "fontsize": 16.0,
     "text": "mp3pack~"
    }
   },
   {
    "box": {
     "id": "stat",
     "maxclass": "newobj",
     "patching_rect": [
      346,
      516,
      104,
      22
     ],
     "text": "print mp3-take"
    }
   },
   {
    "box": {
     "id": "on",
     "maxclass": "message",
     "patching_rect": [
      719,
      407,
      80,
      22
     ],
     "text": "monitor 1"
    }
   },
   {
    "box": {
     "id": "off",
     "maxclass": "message",
     "patching_rect": [
      813,
      407,
      80,
      22
     ],
     "text": "monitor 0"
    }
   },
   {
    "box": {
     "id": "aud",
     "maxclass": "comment",
     "patching_rect": [
      716,
      445,
      245,
      46
     ],
     "linecount": 2,
     "text": "Optional audition of the written frames"
    }
   },
   {
    "box": {
     "id": "un",
     "maxclass": "newobj",
     "patching_rect": [
      507,
      514,
      110,
      22
     ],
     "text": "mc.unpack~ 2"
    }
   },
   {
    "box": {
     "id": "left",
     "maxclass": "newobj",
     "patching_rect": [
      489,
      552,
      62,
      22
     ],
     "text": "*~ 0.2"
    }
   },
   {
    "box": {
     "id": "right",
     "maxclass": "newobj",
     "patching_rect": [
      567,
      552,
      62,
      22
     ],
     "text": "*~ 0.2"
    }
   },
   {
    "box": {
     "id": "dac",
     "maxclass": "ezdac~",
     "patching_rect": [
      520,
      600,
      48,
      48
     ],
     "local": 1
    }
   },
   {
    "box": {
     "id": "rate",
     "maxclass": "comment",
     "patching_rect": [
      25,
      578,
      305,
      67
     ],
     "linecount": 3,
     "text": "Change kb/s while the take runs. 0 selects the highest supported rate. The sourceformat cord preserves file rate and channels."
    }
   },
   {
    "box": {
     "id": "tips",
     "maxclass": "comment",
     "patching_rect": [
      672,
      536,
      275,
      70
     ],
     "linecount": 3,
     "text": "Click the speaker to enable DSP. Leave monitor off for a silent file export."
    }
   },
   {
    "box": {
     "id": "foot",
     "maxclass": "comment",
     "patching_rect": [
      24,
      653,
      928,
      42
     ],
     "linecount": 2,
     "text": "Ports 1–5 carry coefficients, attenuation, gain, window, and sync. Keep the sync cord connected when inserting MC effects. Stereo lanes 1–32 are left and 33–64 are right."
    }
   },
   {
    "box": {
     "id": "kblabel",
     "maxclass": "comment",
     "patching_rect": [
      25,
      509,
      230,
      22
     ],
     "fontface": 1,
     "text": "Recording bitrate (kb/s)"
    }
   },
   {
    "box": {
     "id": "kbnumber",
     "maxclass": "number",
     "patching_rect": [
      25,
      542,
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
      98,
      542,
      122,
      22
     ],
     "text": "prepend bitrate"
    }
   },
   {
    "box": {
     "id": "srcdstitle",
     "maxclass": "comment",
     "patching_rect": [
      24,
      702,
      160,
      22
     ],
     "text": "MC divisor (1 = full)"
    }
   },
   {
    "box": {
     "id": "srcdsn",
     "maxclass": "number",
     "patching_rect": [
      192,
      702,
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
     "id": "srcdsmsg",
     "maxclass": "newobj",
     "patching_rect": [
      261,
      702,
      142,
      22
     ],
     "text": "prepend downsample"
    }
   },
   {
    "box": {
     "id": "srcdsinit",
     "maxclass": "newobj",
     "patching_rect": [
      192,
      733,
      119,
      22
     ],
     "hidden": 1,
     "text": "loadmess set 1"
    }
   },
   {
    "box": {
     "id": "srcdstip",
     "maxclass": "comment",
     "patching_rect": [
      413,
      702,
      563,
      31
     ],
     "text": "Sets the bands presented for editing. A playing file adopts it on the next start."
    }
   },
   {
    "box": {
     "id": "filedstitle",
     "maxclass": "comment",
     "patching_rect": [
      24,
      738,
      160,
      22
     ],
     "text": "File divisor (1 = full)"
    }
   },
   {
    "box": {
     "id": "filedsn",
     "maxclass": "number",
     "patching_rect": [
      192,
      738,
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
     "id": "filedsmsg",
     "maxclass": "newobj",
     "patching_rect": [
      261,
      738,
      142,
      22
     ],
     "text": "prepend downsample"
    }
   },
   {
    "box": {
     "id": "filedsinit",
     "maxclass": "newobj",
     "patching_rect": [
      192,
      769,
      119,
      22
     ],
     "hidden": 1,
     "text": "loadmess set 1"
    }
   },
   {
    "box": {
     "id": "filedstip",
     "maxclass": "comment",
     "patching_rect": [
      413,
      738,
      563,
      31
     ],
     "text": "Sets the saved rate relative to the original file. Set before write."
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
      "src",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "example",
      0
     ],
     "destination": [
      "src",
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
      "src",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "stop",
      0
     ],
     "destination": [
      "src",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src",
      5
     ],
     "destination": [
      "status",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "status",
      0
     ],
     "destination": [
      "fmt",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "status",
      1
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
      "stop",
      0
     ],
     "destination": [
      "finish",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "status",
      2
     ],
     "destination": [
      "sourceLog",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "status",
      3
     ],
     "destination": [
      "sourceLog",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "kill",
      0
     ],
     "destination": [
      "qgain",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "restore",
      0
     ],
     "destination": [
      "qgain",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src",
      0
     ],
     "destination": [
      "mc",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "qgain",
      0
     ],
     "destination": [
      "mc",
      1
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
      "mc",
      0
     ],
     "destination": [
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src",
      1
     ],
     "destination": [
      "pk",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src",
      2
     ],
     "destination": [
      "pk",
      2
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src",
      3
     ],
     "destination": [
      "pk",
      3
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "src",
      4
     ],
     "destination": [
      "pk",
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
      "pk",
      5
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
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "whole",
      0
     ],
     "destination": [
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "thaw",
      0
     ],
     "destination": [
      "pk",
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
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "coeff",
      0
     ],
     "destination": [
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "fmt",
      0
     ],
     "destination": [
      "pk",
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
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pk",
      5
     ],
     "destination": [
      "stat",
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
      "pk",
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
      "pk",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "pk",
      6
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
      "left",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "un",
      1
     ],
     "destination": [
      "right",
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
      "dac",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "right",
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
      "pk",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "srcdsn",
      0
     ],
     "destination": [
      "srcdsmsg",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "srcdsinit",
      0
     ],
     "destination": [
      "srcdsn",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "srcdsmsg",
      0
     ],
     "destination": [
      "src",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "filedsn",
      0
     ],
     "destination": [
      "filedsmsg",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "filedsinit",
      0
     ],
     "destination": [
      "filedsn",
      0
     ],
     "hidden": 1
    }
   },
   {
    "patchline": {
     "source": [
      "filedsmsg",
      0
     ],
     "destination": [
      "pk",
      0
     ],
     "hidden": 1
    }
   }
  ],
  "title": "mp3unpack~ • edit an MP3 in MC"
 }
}
