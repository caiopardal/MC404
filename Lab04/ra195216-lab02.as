# Caio Henrique Pardal dos Santos RA:195216 LAB02.as

.org 0x000
  LOAD MQ,M(distance)
  MUL M(gravity)
  LOAD MQ
  STOR M(Y)
  RSH
  STOR M(K)
laco:
  LOAD M(Y)
  DIV M(K)
  LOAD MQ
  ADD M(K)
  RSH
  STOR M(K)
  LOAD M(counter)
  SUB M(Constant1)
  STOR M(counter)
  JUMP+ M(laco)
  LOAD M(K)
  JUMP M(0x0000000400)

.org 0x101
  gravity:
    .word 0x000000000A
  counter:
    .word 0x0000000009
  Constant1:
    .word 0x0000000001
  K:
    .word 0x0000000000
  Y:
    .word 0x0000000000

.org 0x110
  distance:
    .word 0x0000000DAC