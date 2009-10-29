#!/bin/csh
if ( "$1" == "" ) then
	exit(0)
endif

echo Processing $1 ...

set ARCHIVE=`echo $1 | sed 's/.list//g' | sed 's/.\///g'`

set ARCHIVE_SPECIAL=`echo $ARCHIVE | sed 's/\:/-/g'`
if ( "$ARCHIVE" != "$ARCHIVE_SPECIAL" ) then
        mv "$ARCHIVE" "$ARCHIVE_SPECIAL"
        mv "$ARCHIVE.list" "$ARCHIVE_SPECIAL.list"
        set ARCHIVE=$ARCHIVE_SPECIAL
endif

set ARCHIVE_TAR=$ARCHIVE.tar.gz

echo "Considering tarred archive $ARCHIVE_TAR ..."

if ( ! { test -e "$ARCHIVE.list" } ) then
	echo "archive.sh :: error : $ARCHIVE does not exist!"
endif 

if ( { test -e "$ARCHIVE_TAR" } ) then
	echo "archive.sh: tar exists, cleaning up ..."
	if ( { test -e "$ARCHIVE" } ) then
		rm -r "$ARCHIVE"
	endif
else
	echo "archive.sh: tar does not exist, creating ..."
	tar vfcz "$ARCHIVE_TAR" "$ARCHIVE"
	rm -r "$ARCHIVE"
endif
