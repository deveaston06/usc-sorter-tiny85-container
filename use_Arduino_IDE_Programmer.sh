#!/usr/bin/env bash

lastElement="${1##*/}"
echo "$lastElement"

# Copy files
cp include/iic_manager.h "$1/iic_manager.h"
cp include/led_manager.h "$1/led_manager.h"
cp src/iic_manager.cpp "$1/iic_manager.cpp"
cp src/led_manager.cpp "$1/led_manager.cpp"
cp src/main.cpp "$1/$lastElement.ino"

# Function to replace includes
replace_includes() {
	local file="$1"
	sed -i 's/#include <led_manager.h>/#include "led_manager.h"/g' "$file"
	sed -i 's/#include <iic_manager.h>/#include "iic_manager.h"/g' "$file"
}

# Apply to relevant files
replace_includes "$1/$lastElement.ino"
replace_includes "$1/led_manager.cpp"
replace_includes "$1/iic_manager.cpp"
replace_includes "$1/led_manager.h"
replace_includes "$1/iic_manager.h"
