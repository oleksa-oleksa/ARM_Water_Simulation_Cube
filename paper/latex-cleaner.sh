#!/bin/bash

flag_pdf=""
flag_all=0

function usage() {
	echo "usage:"
	echo "${0:2} [opt] <file>"
	echo "   -a    remove all temporary latex files"
	echo "         if this flag is set the <file> can"
	echo "         be omitted"
	echo "   -p    include the pdf file"
}

function remove() {
	echo "remove()"
}

# no parameter set
if [ -z "$1" ]; then
	usage
	exit 1
fi

fileList="$@"

# get all parameters from commandline
for param in "$@"
do
	if [ '-p' == "$param" ]; then
		flag_pdf=",pdf"
		# remove this parameter from file list
		fileList=$( echo $fileList | sed -e "s/$param//" )
	fi

	if [ '-a' = "$param" ]; then
		flag_all=1
		# remove this parameter from file list
		fileList=$( echo $fileList | sed -e "s/$param//" )
	fi
done

# flag 'remove all' is set
if [ 1 = "$flag_all" ]; then
	eval "rm *.{cb,cb2,lol,glo,acn,ist,nav,snm,aux,lof,log,lot,out,synctex.gz,toc$flag_pdf}" 2> /dev/null
	exit 0
fi

for _file in $fileList
do
	if [ -f "$_file" ]; then # file name with extension
		toRem=$_file
		if [ 3 -lt "${#_file}" -a 'tex' = "${_file:${#_file}-3}" ]; then
			toRem=${_file::-4}
		fi
		eval "rm $toRem.{cb,cb2,lol,glo,acn,ist,nav,snm,aux,lof,log,lot,out,synctex.gz,toc$flag_pdf}" 2> /dev/null

	elif [ -f "$_file.tex" ]; then # file without extention
		eval "rm $_file.{cb,cb2,lol,glo,acn,ist,nav,snm,aux,lof,log,lot,out,synctex.gz,toc$flag_pdf}" 2> /dev/null
	fi
done

