# Caio Henrique Pardal dos Santos RA:195216 LAB03.as

.org 0x000
  LOAD M(arraySize)
  SUB M(Constant1)
  STOR M(arraySize)
laco:
  LOAD M(firstArray)
  ADD M(i)
  STA M(pos1)
  LOAD M(secondArray)
  ADD M(i)
  STA M(pos2)
pos1:
  LOAD MQ, M(0x000)
pos2:
  MUL M(0x000)
  LOAD MQ
  ADD M(soma)
  STOR M(soma)
  LOAD M(i)
  ADD M(Constant1)
  STOR M(i)
  LOAD M(arraySize)
  SUB M(i)
  JUMP+ M(laco)
  LOAD M(soma)
  JUMP M(0x0000000400)


.org 0x05A
  soma:
    .word 0x0000000000
  i:
    .word 0x0000000000
  Constant1:
    .word 0x0000000001

.org 0x3FD
  arraySize:
    .word 0x2
  firstArray:
    .word 0x100
  secondArray:
    .word 0x200
  