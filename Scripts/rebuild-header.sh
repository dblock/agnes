#!/bin/csh

foreach file ( `ls`  )
	awk '/\0/ { split($0, fsections, "[\0]"); print fsections[2]; next }' $file
end
