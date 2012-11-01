; ---------- Generated by Tile Studio ----------

Const LAYER_sonix_blox_Level001 = (1 Shl 8) + 0

; get map width (tiles)
Function MapWidth (map)
  Select map
    Case LAYER_sonix_blox_Level001
      Return 40
  End Select
End Function

; get map height (tiles)
Function MapHeight (map)
  Select map
    Case LAYER_sonix_blox_Level001
      Return 30
  End Select
End Function

; get tile width (pixels)
Function TileWidth (map)
  Select map
    Case LAYER_sonix_blox_Level001
      Return 16
  End Select
End Function

; get tile height (pixels)
Function TileHeight (map)
  Select map
    Case LAYER_sonix_blox_Level001
      Return 16
  End Select
End Function


Global Tiles1Bitmap = LoadAnimImage ("graphics\Tiles1.bmp", 32, 32, 0, 0)
MaskImage Tiles1Bitmap, 255, 0, 255




Dim Tiles1Sequences$ (0 + 1)


Global sonix_bloxBitmap = LoadAnimImage ("graphics\sonix_blox.bmp", 16, 16, 0, 32)
MaskImage sonix_bloxBitmap, 255, 0, 255

Dim sonix_bloxLevel001Map (30, 40)
Restore sonix_bloxLevel001MapData
For y = 0 To 30 - 1
  For x = 0 To 40 - 1
    Read sonix_bloxLevel001Map (y, x)
  Next
Next

.sonix_bloxLevel001MapData
Data   1,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  0,  0,  0,  0,  0,  5,  6
Data   6,  7,  1,  6,  6,  8,  0,  0,  0,  0,  0,  9,  1,  1,  1,  1,  1,  1
Data   1,  1,  9,  1
Data   1,  2, 10, 10, 10, 10, 10, 10, 10, 10,  8,  0,  0,  0,  0,  0,  0,  0
Data   0,  5,  8,  0,  0,  0,  0,  0,  0,  0, 11, 12,  6,  6,  6,  6,  6,  6
Data   6,  6,  9,  1
Data   2, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0, 11, 12,  9
Data   2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  9
Data   2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  9
Data  14, 15, 16, 17,  0,  0,  0, 11, 18, 15, 18, 15, 18, 15, 18, 15, 18, 16
Data  17,  0,  0, 11, 19, 20, 18, 15, 18, 15, 18, 15, 18, 16, 17,  0,  0,  0
Data  11, 19, 18, 21
Data   6,  6, 13,  0,  0,  0,  0,  0, 12,  6,  6,  6,  6,  6,  6,  6,  6, 13
Data   0,  0,  0,  0, 12,  6,  6, 22,  6,  6,  6,  6,  6, 13,  0,  0,  0,  0
Data   0, 12,  6,  6
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0
Data  23, 15, 15, 15, 16, 17,  0,  0,  0,  0,  0, 11, 24, 24, 25, 26, 25, 26
Data  25, 26, 25, 26, 25, 26, 25, 26, 25, 26, 17,  0,  0,  0,  0,  0, 11, 19
Data  18, 15, 18, 23
Data   1,  1,  1,  2, 13,  0,  0,  0,  0,  0,  0,  0,  5,  7,  4, 10, 10, 10
Data   7,  3,  3,  3,  3,  3, 10, 10, 10,  8,  0,  0,  0,  0,  0,  0,  0, 12
Data   9,  1,  1,  1
Data   1,  1,  2, 13, 17,  0,  0,  0,  0,  0,  0,  0,  0,  5,  8,  0,  0, 27
Data   5,  3,  3,  4, 10,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data  12,  9,  1,  1
Data   1,  1,  2,  0,  0,  0, 11, 19, 15, 16, 17,  0,  0,  0,  0,  0,  0,  0
Data   0,  5, 10,  8,  0,  0,  0,  0,  0,  0,  0, 11, 19, 15, 16, 17,  0,  0
Data  11,  9,  1,  1
Data   1,  1,  2, 17,  0,  0,  0, 12,  6, 13,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 12,  6, 13,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1, 14, 15, 18, 15, 18, 15, 18, 15, 16, 17,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11, 19, 15, 18, 15, 18, 15, 18
Data  15, 21,  1,  1
Data   1,  1,  1,  2,  6,  6,  6,  6,  6,  6, 13,  0,  0,  0,  0, 11, 24, 28
Data  29, 17, 11, 24, 25, 29, 17,  0,  0,  0,  0, 12,  6,  6,  6,  6,  6,  6
Data   6,  1,  1,  1
Data   1,  1,  2,  2, 17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  3
Data   4,  0,  0,  7,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data  12,  9,  1,  1
Data   1,  1,  2, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11,  7,  4
Data   8,  0, 11,  7,  7,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0, 11, 24, 25, 26, 25, 30,  4
Data  17,  0,  0,  5,  7, 31, 25, 26, 25, 29, 17,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0, 11, 24, 29, 17,  0,  0,  0,  0,  5, 10, 10, 10,  3,  4
Data   0,  0,  0,  0,  7,  3, 10, 10, 10,  8,  0,  0,  0,  0, 11, 24, 29, 17
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  7, 31, 24, 29, 29, 17,  0,  0,  0,  0,  0,  7,  4
Data   0,  0,  0,  0,  7,  4,  0,  0,  0,  0,  0, 11, 24, 24, 29, 30,  4,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0, 11,  5, 10,  7,  4,  8,  0,  0,  0,  0,  0,  0,  5,  8
Data   0,  0,  0,  0,  5,  8,  0,  0,  0,  0,  0,  0,  5,  7,  4, 10,  8,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  5,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  8,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
Data   0,  9,  1,  1
Data   1,  1, 14, 15, 18, 15, 18, 15, 18, 15, 18, 17,  0,  0,  0, 27, 25, 26
Data  25, 26, 25, 26, 25, 26, 32,  0,  0,  0, 27, 25, 15, 18, 15, 18, 15, 18
Data  15, 21,  1,  1

Dim sonix_bloxLevel001BoundMap (30, 40)
Restore sonix_bloxLevel001BoundMapData
For y = 0 To 30 - 1
  For x = 0 To 40 - 1
    Read sonix_bloxLevel001BoundMap (y, x)
  Next
Next

; tile size: w = 16, h = 16
.sonix_bloxLevel001BoundMapData
Data  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
Data  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
Data  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
Data  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 8
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 8
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2
Data  8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Data  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2




Dim sonix_bloxSequences$ (0 + 1)

; sonix_bloxLevel001Layer
; movement (x, y) = (x = x, y = y) 



; get tile number at position (x,y)
Function GetTile (map, x, y)
  Select map
    Case LAYER_sonix_blox_Level001
      Return sonix_bloxLevel001Map (y, x) - 1
  End Select
End Function

; get bounds/map code at position (x,y)
Function GetBounds (map, x, y)
  Select map
    Case LAYER_sonix_blox_Level001
      Return sonix_bloxLevel001BoundMap (y, x)
  End Select
End Function

Function DrawTile (map, x, y, tile)
  Select map
    Case LAYER_sonix_blox_Level001
      DrawImage sonix_bloxBitmap, x, y, tile
  End Select
End Function

; ---------- end of generated file ----------