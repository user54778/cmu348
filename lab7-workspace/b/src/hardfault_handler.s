  .syntax unified
  .cpu cortex-m0
  .thumb


  .section .text
  .extern selection  
  .extern result
  .global HardFault_Handler
  .type HardFault_Handler, %function

@Read MSP to get the base address of the saved frame
@Load the saved register values from their known offsets within that frame
@Check the selection variable
@Write the appropriate byte into result
@Patch the saved PC (add 2 to skip the UDF instruction, which lives at frame offset +0x18)
@Return via EXC_RETURN — which is already sitting in LR when your handler starts, 
@placed there by the hardware
@0x01 → load from SP + 0x00 (saved R0), store into result
@0x02 → load from SP + 0x04 (saved R1), store into result
@0x04 → load from SP + 0x08 (saved R2), store into result
@0x08 → load from SP + 0x0C (saved R3), store into result
@0x40 → store sp
@0x80 → load from SP + 0x1C (saved xPSR), store into result
@0x00 → do nothing, leave result unchanged NOTE: this is fine since its the value 0x00

HardFault_Handler:
  @ TODO: load address of selection and its value (byte value)
  @ TODO: compare that value against cases (0x00..0x08)
  @ TODO: for a matching case, ld saved val from correct frame offset
  @ into a register.
  @ TODO: ld address of *result* and store byte into it
  @ TODO: load sp + 0x18, add 2 to bypass UDF instruction store it back
  @ TODO: return with special value EXC_RETURN (saved in lr)
  LDR r0, =selection
  LDRB r1, [r0] @NOTE: DO NOT use r0 past this point

  CMP r1, #0x00
  BEQ case_do_nothing

  CMP r1, #0x01
  BEQ case_reg_a

  CMP r1, #0x02
  BEQ case_reg_b

  CMP r1, #0x04
  BEQ case_reg_x

  CMP r1, #0x08
  BEQ case_reg_y

  CMP r1, #0x40
  BEQ case_sp

  CMP r1, #0x80
  BEQ case_xpsr

case_do_nothing:
  B cleanup
  
case_reg_a:
  @ load r0 from main
  @ store val of loaded r0 (main)
  MOV r1, sp
  LDRB r2, [r1, #0x00]

  LDR r3, =result @ ld addr glbl
  STRB r2, [r3]   @ store into res

  B cleanup

case_reg_b:
  MOV r1, sp
  LDRB r2, [r1, #0x04]

  LDR r3, =result @ ld addr glbl
  STRB r2, [r3]   @ store into res

  B cleanup

case_reg_x:
  MOV r1, sp
  LDRB r2, [r1, #0x08]

  LDR r3, =result @ ld addr glbl
  STRB r2, [r3]   @ store into res
  B cleanup

case_reg_y:
  MOV r1, sp
  LDRB r2, [r1, #0x0C]

  LDR r3, =result @ ld addr glbl
  STRB r2, [r3]   @ store into res
  B cleanup

case_sp:
  MOV r1, sp
  LDRB r2, [r1, #0x18]

  LDR r3, =result
  STRB r2, [r3]
  B cleanup

case_xpsr:
  MOV r1, sp
  LDRB r2, [r1, #0x1C]

  LDR r3, =result
  STRB r2, [r3]
  B cleanup

cleanup: 
  LDR r0, [sp, #0x18] 
  ADDS r0, r0, #2
  STR r0, [sp, #0x18]
  BX lr               @ return with special val EXC_RETURN saved in link reg (LR/R14)


@LDR r1, =selection  ; load address selection into r1
@LDRB r2, [r1]       ; load byte value sel into r2

@LDR r1, =result     ; load addr of res
@STRB r2, [r1]       ; store byte into res

@LDR r0, [sp, #0x18] ; grab the MSP
@ADDS r0, r0, #2     ; skip UDF
@STR r0, [sp, #0x18] ; store new sp
@LDR r2, [sp, #0x04] @ MSP + 1 byte
@LDRB r3, [r2] <- treats the value as a ptr and deref (which we dont want)
@LDRB r2, [sp, #0x00] @ since sp is not a low reg set, this wont work (only works r0-r7)
