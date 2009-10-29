#!/bin/csh
foreach file ( `ls`  )
	cd "$file/archives"
	set CURPATH=`pwd`
	echo "Processing $CURPATH"
	find . -name "*.list" | xargs -l ~/bin/archives.sh
	cd ../..
end
