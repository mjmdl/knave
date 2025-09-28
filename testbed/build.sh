CC="gcc"
CFLAGS="--std=c89 -Wall -Wextra -Wpedantic -I.."
LFLAGS="-lGL -lX11 -lXinerama"
SRC="main.c knave.c"
TARGET="testbed"

$CC $CFLAGS -o $TARGET $SRC $LFLAGS
