#!/bin/bash

#for dir in $(find . -name "Makefile" -printf "%h\n");do
#    CURRENT_DIR=$(pwd);
#    cd "$dir";
#    make clean;
#    rm Makefile;
#    cd "$CURRENT_DIR";
#done;

IFS=$'\n';
for name in $(find . -name "CMakeCache.txt";
              find . -name "cmake_install.cmake";
              find . -name "create_bin_package.list.tmp";
              find . -name "create_source_package.list.tmp";
              find . -name "_CPack_Packages";
              find . -name "*.dir";
              find . -name "*.sln";
              find . -name "*.i";
              find . -name "*.s";
              find . -name "*.suo";
              find . -name "*.ncb";
              find . -name "*sasfit*.dll*";
              find . -name "*sasfit*.so";
              find . -name "*sasfit*.a";
              find . -name "sasfit";
              find . -name "sasfit.exe";
              find . -name "*.vcproj*";
              find . -iname "release";
              find . -iname "debug";
              find . -name "Makefile";
              find . -name "CMakeFiles";);do

	if echo $name | grep -qF '.hg'; then
		continue;
	fi;
	
	echo "removing '$name'"
#	rm -Rf "$name";
done;

exit 0;

