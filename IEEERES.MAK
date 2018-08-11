# IBM Developer's Workframe/2 Make File Creation run at 09:41:39 on 11/20/92

# Make File Creation run in directory:
#   D:\DATA\C;

.SUFFIXES:

.SUFFIXES: .c

IEEERES.EXE:  \
  IEEERES.OBJ \
  IEEERES.DEF \
  IEEERES.MAK
   @REM @<<IEEERES.@0
     /CO /PM:VIO +
     IEEERES.OBJ
     IEEERES.EXE
     
     
     IEEERES.DEF;
<<
   LINK386.EXE @IEEERES.@0

{.}.c.obj:
   ICC.EXE /Sp1 /Ss /Ti /C .\$*.c

!include IEEERES.DEP
