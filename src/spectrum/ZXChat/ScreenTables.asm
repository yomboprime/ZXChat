#ifndef __LIBRARY_SCREENTABLES__

REM Avoid recursive / multiple inclusion

#define __LIBRARY_SCREENTABLES__

ASM

; +0 - Pixel address tables
; +192 - Character Address Tables
; +224 - Attribute Address Tables

ALIGN 256
ScreenAddressTables:
BLPixelTable:
DEFB 64   ; 0 - 0
DEFB 65   ; 0 - 1
DEFB 66   ; 0 - 2
DEFB 67   ; 0 - 3
DEFB 68   ; 0 - 4
DEFB 69   ; 0 - 5
DEFB 70   ; 0 - 6
DEFB 71   ; 0 - 7
DEFB 64   ; 1 - 8
DEFB 65   ; 1 - 9
DEFB 66   ; 1 - 10
DEFB 67   ; 1 - 11
DEFB 68   ; 1 - 12
DEFB 69   ; 1 - 13
DEFB 70   ; 1 - 14
DEFB 71   ; 1 - 15
DEFB 64   ; 2 - 16
DEFB 65   ; 2 - 17
DEFB 66   ; 2 - 18
DEFB 67   ; 2 - 19
DEFB 68   ; 2 - 20
DEFB 69   ; 2 - 21
DEFB 70   ; 2 - 22
DEFB 71   ; 2 - 23
DEFB 64   ; 3 - 24
DEFB 65   ; 3 - 25
DEFB 66   ; 3 - 26
DEFB 67   ; 3 - 27
DEFB 68   ; 3 - 28
DEFB 69   ; 3 - 29
DEFB 70   ; 3 - 30
DEFB 71   ; 3 - 31
DEFB 64   ; 4 - 32
DEFB 65   ; 4 - 33
DEFB 66   ; 4 - 34
DEFB 67   ; 4 - 35
DEFB 68   ; 4 - 36
DEFB 69   ; 4 - 37
DEFB 70   ; 4 - 38
DEFB 71   ; 4 - 39
DEFB 64   ; 5 - 40
DEFB 65   ; 5 - 41
DEFB 66   ; 5 - 42
DEFB 67   ; 5 - 43
DEFB 68   ; 5 - 44
DEFB 69   ; 5 - 45
DEFB 70   ; 5 - 46
DEFB 71   ; 5 - 47
DEFB 64   ; 6 - 48
DEFB 65   ; 6 - 49
DEFB 66   ; 6 - 50
DEFB 67   ; 6 - 51
DEFB 68   ; 6 - 52
DEFB 69   ; 6 - 53
DEFB 70   ; 6 - 54
DEFB 71   ; 6 - 55
DEFB 64   ; 7 - 56
DEFB 65   ; 7 - 57
DEFB 66   ; 7 - 58
DEFB 67   ; 7 - 59
DEFB 68   ; 7 - 60
DEFB 69   ; 7 - 61
DEFB 70   ; 7 - 62
DEFB 71   ; 7 - 63
DEFB 72   ; 8 - 64
DEFB 73   ; 8 - 65
DEFB 74   ; 8 - 66
DEFB 75   ; 8 - 67
DEFB 76   ; 8 - 68
DEFB 77   ; 8 - 69
DEFB 78   ; 8 - 70
DEFB 79   ; 8 - 71
DEFB 72   ; 9 - 72
DEFB 73   ; 9 - 73
DEFB 74   ; 9 - 74
DEFB 75   ; 9 - 75
DEFB 76   ; 9 - 76
DEFB 77   ; 9 - 77
DEFB 78   ; 9 - 78
DEFB 79   ; 9 - 79
DEFB 72   ; 10 - 80
DEFB 73   ; 10 - 81
DEFB 74   ; 10 - 82
DEFB 75   ; 10 - 83
DEFB 76   ; 10 - 84
DEFB 77   ; 10 - 85
DEFB 78   ; 10 - 86
DEFB 79   ; 10 - 87
DEFB 72   ; 11 - 88
DEFB 73   ; 11 - 89
DEFB 74   ; 11 - 90
DEFB 75   ; 11 - 91
DEFB 76   ; 11 - 92
DEFB 77   ; 11 - 93
DEFB 78   ; 11 - 94
DEFB 79   ; 11 - 95
DEFB 72   ; 12 - 96
DEFB 73   ; 12 - 97
DEFB 74   ; 12 - 98
DEFB 75   ; 12 - 99
DEFB 76   ; 12 - 100
DEFB 77   ; 12 - 101
DEFB 78   ; 12 - 102
DEFB 79   ; 12 - 103
DEFB 72   ; 13 - 104
DEFB 73   ; 13 - 105
DEFB 74   ; 13 - 106
DEFB 75   ; 13 - 107
DEFB 76   ; 13 - 108
DEFB 77   ; 13 - 109
DEFB 78   ; 13 - 110
DEFB 79   ; 13 - 111
DEFB 72   ; 14 - 112
DEFB 73   ; 14 - 113
DEFB 74   ; 14 - 114
DEFB 75   ; 14 - 115
DEFB 76   ; 14 - 116
DEFB 77   ; 14 - 117
DEFB 78   ; 14 - 118
DEFB 79   ; 14 - 119
DEFB 72   ; 15 - 120
DEFB 73   ; 15 - 121
DEFB 74   ; 15 - 122
DEFB 75   ; 15 - 123
DEFB 76   ; 15 - 124
DEFB 77   ; 15 - 125
DEFB 78   ; 15 - 126
DEFB 79   ; 15 - 127
DEFB 80   ; 16 - 128
DEFB 81   ; 16 - 129
DEFB 82   ; 16 - 130
DEFB 83   ; 16 - 131
DEFB 84   ; 16 - 132
DEFB 85   ; 16 - 133
DEFB 86   ; 16 - 134
DEFB 87   ; 16 - 135
DEFB 80   ; 17 - 136
DEFB 81   ; 17 - 137
DEFB 82   ; 17 - 138
DEFB 83   ; 17 - 139
DEFB 84   ; 17 - 140
DEFB 85   ; 17 - 141
DEFB 86   ; 17 - 142
DEFB 87   ; 17 - 143
DEFB 80   ; 18 - 144
DEFB 81   ; 18 - 145
DEFB 82   ; 18 - 146
DEFB 83   ; 18 - 147
DEFB 84   ; 18 - 148
DEFB 85   ; 18 - 149
DEFB 86   ; 18 - 150
DEFB 87   ; 18 - 151
DEFB 80   ; 19 - 152
DEFB 81   ; 19 - 153
DEFB 82   ; 19 - 154
DEFB 83   ; 19 - 155
DEFB 84   ; 19 - 156
DEFB 85   ; 19 - 157
DEFB 86   ; 19 - 158
DEFB 87   ; 19 - 159
DEFB 80   ; 20 - 160
DEFB 81   ; 20 - 161
DEFB 82   ; 20 - 162
DEFB 83   ; 20 - 163
DEFB 84   ; 20 - 164
DEFB 85   ; 20 - 165
DEFB 86   ; 20 - 166
DEFB 87   ; 20 - 167
DEFB 80   ; 21 - 168
DEFB 81   ; 21 - 169
DEFB 82   ; 21 - 170
DEFB 83   ; 21 - 171
DEFB 84   ; 21 - 172
DEFB 85   ; 21 - 173
DEFB 86   ; 21 - 174
DEFB 87   ; 21 - 175
DEFB 80   ; 22 - 176
DEFB 81   ; 22 - 177
DEFB 82   ; 22 - 178
DEFB 83   ; 22 - 179
DEFB 84   ; 22 - 180
DEFB 85   ; 22 - 181
DEFB 86   ; 22 - 182
DEFB 87   ; 22 - 183
DEFB 80   ; 23 - 184
DEFB 81   ; 23 - 185
DEFB 82   ; 23 - 186
DEFB 83   ; 23 - 187
DEFB 84   ; 23 - 188
DEFB 85   ; 23 - 189
DEFB 86   ; 23 - 190
DEFB 87   ; 23 - 191

; Character Table - Addresses of First row of Character Row
; Starts after pixel row table which is 192 bytes long.
; Second half of table aligned to next page.

CharTable:
DEFB 64   ; 0 - 0
DEFB 64   ; 1 - 8
DEFB 64   ; 2 - 16
DEFB 64   ; 3 - 24
DEFB 64   ; 4 - 32
DEFB 64   ; 5 - 40
DEFB 64   ; 6 - 48
DEFB 64   ; 7 - 56
DEFB 72   ; 8 - 64
DEFB 72   ; 9 - 72
DEFB 72   ; 10 - 80
DEFB 72   ; 11 - 88
DEFB 72   ; 12 - 96
DEFB 72   ; 13 - 104
DEFB 72   ; 14 - 112
DEFB 72   ; 15 - 120
DEFB 80   ; 16 - 128
DEFB 80   ; 17 - 136
DEFB 80   ; 18 - 144
DEFB 80   ; 19 - 152
DEFB 80   ; 20 - 160
DEFB 80   ; 21 - 168
DEFB 80   ; 22 - 176
DEFB 80   ; 23 - 184

ALIGN 32
; Attribute Table - address of attribute by row
AttrTable:
DEFB 88   ;0
DEFB 88   ;1
DEFB 88   ;2
DEFB 88   ;3
DEFB 88   ;4
DEFB 88   ;5
DEFB 88   ;6
DEFB 88   ;7
DEFB 89   ;8
DEFB 89   ;9
DEFB 89   ;10
DEFB 89   ;11
DEFB 89   ;12
DEFB 89   ;13
DEFB 89   ;14
DEFB 89   ;15
DEFB 90   ;16
DEFB 90   ;17
DEFB 90   ;18
DEFB 90   ;19
DEFB 90   ;20
DEFB 90   ;21
DEFB 90   ;22
DEFB 90   ;23

; Second half of pixel row table.
ALIGN 256

DEFB 0   ; 0 - 0
DEFB 0   ; 0 - 1
DEFB 0   ; 0 - 2
DEFB 0   ; 0 - 3
DEFB 0   ; 0 - 4
DEFB 0   ; 0 - 5
DEFB 0   ; 0 - 6
DEFB 0   ; 0 - 7
DEFB 32   ; 1 - 8
DEFB 32   ; 1 - 9
DEFB 32   ; 1 - 10
DEFB 32   ; 1 - 11
DEFB 32   ; 1 - 12
DEFB 32   ; 1 - 13
DEFB 32   ; 1 - 14
DEFB 32   ; 1 - 15
DEFB 64   ; 2 - 16
DEFB 64   ; 2 - 17
DEFB 64   ; 2 - 18
DEFB 64   ; 2 - 19
DEFB 64   ; 2 - 20
DEFB 64   ; 2 - 21
DEFB 64   ; 2 - 22
DEFB 64   ; 2 - 23
DEFB 96   ; 3 - 24
DEFB 96   ; 3 - 25
DEFB 96   ; 3 - 26
DEFB 96   ; 3 - 27
DEFB 96   ; 3 - 28
DEFB 96   ; 3 - 29
DEFB 96   ; 3 - 30
DEFB 96   ; 3 - 31
DEFB 128   ; 4 - 32
DEFB 128   ; 4 - 33
DEFB 128   ; 4 - 34
DEFB 128   ; 4 - 35
DEFB 128   ; 4 - 36
DEFB 128   ; 4 - 37
DEFB 128   ; 4 - 38
DEFB 128   ; 4 - 39
DEFB 160   ; 5 - 40
DEFB 160   ; 5 - 41
DEFB 160   ; 5 - 42
DEFB 160   ; 5 - 43
DEFB 160   ; 5 - 44
DEFB 160   ; 5 - 45
DEFB 160   ; 5 - 46
DEFB 160   ; 5 - 47
DEFB 192   ; 6 - 48
DEFB 192   ; 6 - 49
DEFB 192   ; 6 - 50
DEFB 192   ; 6 - 51
DEFB 192   ; 6 - 52
DEFB 192   ; 6 - 53
DEFB 192   ; 6 - 54
DEFB 192   ; 6 - 55
DEFB 224   ; 7 - 56
DEFB 224   ; 7 - 57
DEFB 224   ; 7 - 58
DEFB 224   ; 7 - 59
DEFB 224   ; 7 - 60
DEFB 224   ; 7 - 61
DEFB 224   ; 7 - 62
DEFB 224   ; 7 - 63
DEFB 0   ; 8 - 64
DEFB 0   ; 8 - 65
DEFB 0   ; 8 - 66
DEFB 0   ; 8 - 67
DEFB 0   ; 8 - 68
DEFB 0   ; 8 - 69
DEFB 0   ; 8 - 70
DEFB 0   ; 8 - 71
DEFB 32   ; 9 - 72
DEFB 32   ; 9 - 73
DEFB 32   ; 9 - 74
DEFB 32   ; 9 - 75
DEFB 32   ; 9 - 76
DEFB 32   ; 9 - 77
DEFB 32   ; 9 - 78
DEFB 32   ; 9 - 79
DEFB 64   ; 10 - 80
DEFB 64   ; 10 - 81
DEFB 64   ; 10 - 82
DEFB 64   ; 10 - 83
DEFB 64   ; 10 - 84
DEFB 64   ; 10 - 85
DEFB 64   ; 10 - 86
DEFB 64   ; 10 - 87
DEFB 96   ; 11 - 88
DEFB 96   ; 11 - 89
DEFB 96   ; 11 - 90
DEFB 96   ; 11 - 91
DEFB 96   ; 11 - 92
DEFB 96   ; 11 - 93
DEFB 96   ; 11 - 94
DEFB 96   ; 11 - 95
DEFB 128   ; 12 - 96
DEFB 128   ; 12 - 97
DEFB 128   ; 12 - 98
DEFB 128   ; 12 - 99
DEFB 128   ; 12 - 100
DEFB 128   ; 12 - 101
DEFB 128   ; 12 - 102
DEFB 128   ; 12 - 103
DEFB 160   ; 13 - 104
DEFB 160   ; 13 - 105
DEFB 160   ; 13 - 106
DEFB 160   ; 13 - 107
DEFB 160   ; 13 - 108
DEFB 160   ; 13 - 109
DEFB 160   ; 13 - 110
DEFB 160   ; 13 - 111
DEFB 192   ; 14 - 112
DEFB 192   ; 14 - 113
DEFB 192   ; 14 - 114
DEFB 192   ; 14 - 115
DEFB 192   ; 14 - 116
DEFB 192   ; 14 - 117
DEFB 192   ; 14 - 118
DEFB 192   ; 14 - 119
DEFB 224   ; 15 - 120
DEFB 224   ; 15 - 121
DEFB 224   ; 15 - 122
DEFB 224   ; 15 - 123
DEFB 224   ; 15 - 124
DEFB 224   ; 15 - 125
DEFB 224   ; 15 - 126
DEFB 224   ; 15 - 127
DEFB 0   ; 16 - 128
DEFB 0   ; 16 - 129
DEFB 0   ; 16 - 130
DEFB 0   ; 16 - 131
DEFB 0   ; 16 - 132
DEFB 0   ; 16 - 133
DEFB 0   ; 16 - 134
DEFB 0   ; 16 - 135
DEFB 32   ; 17 - 136
DEFB 32   ; 17 - 137
DEFB 32   ; 17 - 138
DEFB 32   ; 17 - 139
DEFB 32   ; 17 - 140
DEFB 32   ; 17 - 141
DEFB 32   ; 17 - 142
DEFB 32   ; 17 - 143
DEFB 64   ; 18 - 144
DEFB 64   ; 18 - 145
DEFB 64   ; 18 - 146
DEFB 64   ; 18 - 147
DEFB 64   ; 18 - 148
DEFB 64   ; 18 - 149
DEFB 64   ; 18 - 150
DEFB 64   ; 18 - 151
DEFB 96   ; 19 - 152
DEFB 96   ; 19 - 153
DEFB 96   ; 19 - 154
DEFB 96   ; 19 - 155
DEFB 96   ; 19 - 156
DEFB 96   ; 19 - 157
DEFB 96   ; 19 - 158
DEFB 96   ; 19 - 159
DEFB 128   ; 20 - 160
DEFB 128   ; 20 - 161
DEFB 128   ; 20 - 162
DEFB 128   ; 20 - 163
DEFB 128   ; 20 - 164
DEFB 128   ; 20 - 165
DEFB 128   ; 20 - 166
DEFB 128   ; 20 - 167
DEFB 160   ; 21 - 168
DEFB 160   ; 21 - 169
DEFB 160   ; 21 - 170
DEFB 160   ; 21 - 171
DEFB 160   ; 21 - 172
DEFB 160   ; 21 - 173
DEFB 160   ; 21 - 174
DEFB 160   ; 21 - 175
DEFB 192   ; 22 - 176
DEFB 192   ; 22 - 177
DEFB 192   ; 22 - 178
DEFB 192   ; 22 - 179
DEFB 192   ; 22 - 180
DEFB 192   ; 22 - 181
DEFB 192   ; 22 - 182
DEFB 192   ; 22 - 183
DEFB 224   ; 23 - 184
DEFB 224   ; 23 - 185
DEFB 224   ; 23 - 186
DEFB 224   ; 23 - 187
DEFB 224   ; 23 - 188
DEFB 224   ; 23 - 189
DEFB 224   ; 23 - 190
DEFB 224   ; 23 - 191

; Second half of Character Based Address Table
DEFB 0      ; 0 - 0
DEFB 32     ; 1 - 8
DEFB 64     ; 2 - 16
DEFB 96     ; 3 - 24
DEFB 128    ; 4 - 32
DEFB 160    ; 5 - 40
DEFB 192    ; 6 - 48
DEFB 224    ; 7 - 56
DEFB 0      ; 8 - 64
DEFB 32     ; 9 - 72
DEFB 64     ; 10 - 80
DEFB 96     ; 11 - 88
DEFB 128    ; 12 - 96
DEFB 160    ; 13 - 104
DEFB 192    ; 14 - 112
DEFB 224    ; 15 - 120
DEFB 0      ; 16 - 128
DEFB 32     ; 17 - 136
DEFB 64     ; 18 - 144
DEFB 96     ; 19 - 152
DEFB 128    ; 20 - 160
DEFB 160    ; 21 - 168
DEFB 192    ; 22 - 176
DEFB 224    ; 23 - 184

; Second half of Attribute address Table
ALIGN 32
DEFB 0      ;0
DEFB 32     ;1
DEFB 64     ;2
DEFB 96     ;3
DEFB 128    ;4
DEFB 160    ;5
DEFB 192    ;6
DEFB 224    ;7
DEFB 0      ;8
DEFB 32     ;9
DEFB 64     ;10
DEFB 96     ;11
DEFB 128    ;12
DEFB 160    ;13
DEFB 192    ;14
DEFB 224    ;15
DEFB 0      ;16
DEFB 32     ;17
DEFB 64     ;18
DEFB 96     ;19
DEFB 128    ;20
DEFB 160    ;21
DEFB 192    ;22
DEFB 224    ;23

END ASM
#endif